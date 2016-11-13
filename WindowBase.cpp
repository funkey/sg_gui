#include "config.h"

#ifdef HAVE_PNG
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
using namespace boost::gil;
#endif

#include <util/Logger.h>
#include <util/helpers.hpp>
#include <sg_gui/OpenGl.h>
#include <sg_gui/ContextSettings.h>
#include <sg_gui/Keys.h>
#include <sg_gui/error_handling.h>
#include "WindowBase.h"

namespace sg_gui {

logger::LogChannel winlog("winlog", "[WindowBase] ");

WindowBase::WindowBase(std::string caption) :
	_caption(caption),
	_region(0, 0, 1, 1), // region and resolution will be set with the first resize event
	_resolution(1, 1),
	_saveFrameRequest(false),
#ifdef HAVE_PNG
	_frameNumber(0),
#endif
	_frameBuffer(0),
	_clear_r(0.5),
	_clear_g(0.5),
	_clear_b(0.5) {

	// initiate first redraw
	setDirty();
}

WindowBase::~WindowBase() {

	sg_gui::OpenGl::Guard guard;

	LOG_DEBUG(winlog) << "[" << getCaption() << "] destructing..." << std::endl;

	deleteFrameBuffer();

	LOG_DEBUG(winlog) << "[" << getCaption() << "] destructed" << std::endl;
}

GlContext*
WindowBase::createGlContext() {

	LOG_ALL(winlog) << "[" << getCaption() << "] creating a new GlContext" << std::endl;

	GlContext* globalContext = OpenGl::getGlobalContext();
	globalContext->activate();

	ContextSettings settings;

	// create the context
	GlContext* glContext = createSharedGlContext(settings, globalContext);

	// activate it
	glContext->activate();

	configureViewport();

	// return it
	return glContext;
}

void
WindowBase::requestFrameSave() {

	_saveFrameRequest = true;
}

void
WindowBase::onInnerSignal(SizeChanged& /*signal*/) {

	// TODO:
	// Here, we could resize the window to fit the view. However, this should be
	// an optional feature.

	setDirty();
}

void
WindowBase::onInnerSignal(ContentChanged& /*signal*/) {

	LOG_ALL(winlog) << "[" << getCaption() << "] received a content change signal" << std::endl;

	setDirty();
}

void
WindowBase::onInnerSignal(WindowFullscreen& signal) {

	LOG_ALL(winlog) << "[" << getCaption() << "] received a fullscreen request" << std::endl;

	setFullscreen(signal.fullscreen);

	setDirty();
}

void
WindowBase::redraw() {

	// redraw only if needed
	if (!isDirty() || closed())
		return;

	setDirty(false);

	// ensure that our context is active
	OpenGl::Guard guard(this);

	clear();

	LOG_ALL(winlog) << "[" << getCaption() << "] redrawing my content" << std::endl;

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// draw opaque content first...
	LOG_ALL(winlog) << "[" << getCaption() << "] drawing opaque content" << std::endl;
	DrawOpaque drawOpaqueSignal;
	drawOpaqueSignal.roi() = _region;
	drawOpaqueSignal.resolution() = util::point<float,2>(1.0, 1.0);
	sendInner(drawOpaqueSignal);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ...followed by translucent content
	LOG_ALL(winlog) << "[" << getCaption() << "] drawing translucent content" << std::endl;
	DrawTranslucent drawTranslucentSignal;
	drawTranslucentSignal.roi() = _region;
	drawTranslucentSignal.resolution() = util::point<float,2>(1.0, 1.0);
	glDepthMask(false);
	sendInner(drawTranslucentSignal);
	glDepthMask(true);

	if (drawOpaqueSignal.needsRedraw() || drawTranslucentSignal.needsRedraw()) {

		LOG_ALL(winlog) << "[" << getCaption() << "] painter indicated redraw request -- set myself dirty again" << std::endl;
		setDirty();
	}

	GL_ASSERT;

	flush();

	GL_ASSERT;

	LOG_ALL(winlog) << "[" << getCaption() << "] finished redrawing" << std::endl;
}

void
WindowBase::processExposeEvent() {

	setDirty(true, false);
}

bool
WindowBase::processResizeEvent(int width, int height) {

	LOG_ALL(winlog) << "[" << getCaption() << "] got a resize event " << width << "x" << height << std::endl;

	// did the size of the window change?
	if (_region.max().x() == width && _region.max().y() == height && _resolution.x() == width && _resolution.y() == height)
		return false;

	_region.max().x() = width;
	_region.max().y() = height;
	_resolution.x() = width;
	_resolution.y() = height;

	{
		// ensure that our context is active
		OpenGl::Guard guard(this);

		configureViewport();
		createFrameBuffer();
	}

	// prepare painters
	sendInner<Resize>(_region);

	setDirty(true, false);

	return true;
}

void
WindowBase::processCloseEvent(){

	LOG_DEBUG(winlog) << "[" << getCaption() << "] invalidating my GlContext" << std::endl;

	// ensure that our context is destructed
	OpenGl::Guard guard(0);

	LOG_DEBUG(winlog) << "[" << getCaption() << "] closing window now" << std::endl;

	close();
}

void
WindowBase::clear() {

	glClearColor(_clear_r, _clear_g, _clear_b, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GL_ASSERT;
}

void
WindowBase::flush() {

	OpenGl::flush();
}

void
WindowBase::configureViewport() {

	LOG_DEBUG(winlog) << "[" << getCaption() << "] configuring viewport" << std::endl;

	// we want to draw in the entire window
	glViewport(
			0,
			0,
			_resolution.x(),
			_resolution.y());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(
			0,
			_resolution.x(),
			_resolution.y(),
			0);

	GL_ASSERT;
}

void
WindowBase::createFrameBuffer() {

	deleteFrameBuffer();

	_frameBuffer = new unsigned char[(int)_resolution.x()*(int)_resolution.y()*3];
}

void
WindowBase::deleteFrameBuffer() {

	if (_frameBuffer) {

		delete[] _frameBuffer;
		_frameBuffer = 0;
	}
}

void
WindowBase::saveFrame() {

#ifdef HAVE_PNG
	if (closed())
		return;

	// ensure that our context is active
	OpenGl::Guard guard(this);

	glReadPixels(
			0, 0,
			_resolution.x(), _resolution.y(),
			GL_RGB, GL_UNSIGNED_BYTE,
			_frameBuffer);
	GL_ASSERT;

	rgb8c_view_t frameView =
			interleaved_view(
					_resolution.x(), _resolution.y(),
					(const rgb8_pixel_t*)_frameBuffer,
					_resolution.x()*3);

	png_write_view(
			"./shots/" + getCaption() + to_string_with_leading_zeros(_frameNumber, 8) + ".png",
			flipped_up_down_view(frameView));

	_frameNumber++;
#endif
}

} // namespace sg_gui
