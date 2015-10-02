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
#include "Window.h"

using std::cout;
using std::endl;

using namespace logger;

namespace sg_gui {

LogChannel winlog("winlog", "[Window] ");

Window::Window(
		string caption,
		const WindowMode& mode) :
	WindowType(caption, mode),
	_region(0, 0, mode.size.x(), mode.size.y()),
	_resolution(mode.size.x(), mode.size.y()),
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

Window::~Window() {

	sg_gui::OpenGl::Guard guard;

	LOG_DEBUG(winlog) << "[" << getCaption() << "] destructing..." << endl;

	deleteFrameBuffer();

	LOG_DEBUG(winlog) << "[" << getCaption() << "] destructed" << endl;
}

void
Window::createFrameBuffer() {

	deleteFrameBuffer();

	_frameBuffer = new unsigned char[(int)_resolution.x()*(int)_resolution.y()*3];
}

void
Window::deleteFrameBuffer() {

	if (_frameBuffer) {

		delete[] _frameBuffer;
		_frameBuffer = 0;
	}
}

void
Window::configureViewport() {

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
Window::redraw() {

	// ensure that our context is active
	OpenGl::Guard guard(this);

	clear();

	LOG_ALL(winlog) << "[" << getCaption() << "] redrawing my content" << endl;

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// draw opaque content first...
	DrawOpaque drawOpaqueSignal;
	drawOpaqueSignal.roi() = _region;
	drawOpaqueSignal.resolution() = point<float,2>(1.0, 1.0);
	sendInner(drawOpaqueSignal);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ...followed by translucent content
	DrawTranslucent drawTranslucentSignal;
	drawTranslucentSignal.roi() = _region;
	drawTranslucentSignal.resolution() = point<float,2>(1.0, 1.0);
	glDepthMask(false);
	sendInner(drawTranslucentSignal);
	glDepthMask(true);

	if (drawOpaqueSignal.needsRedraw() || drawTranslucentSignal.needsRedraw()) {

		LOG_ALL(winlog) << "[" << getCaption() << "] painter indicated redraw request -- set myself dirty again" << endl;
		setDirty();
	}

	GL_ASSERT;

	flush();

	GL_ASSERT;

	LOG_ALL(winlog) << "[" << getCaption() << "] finished redrawing" << endl;
}

const point<float,2>&
Window::getResolution() {

	return _resolution;
}

bool
Window::processResizeEvent(int width, int height) {

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

	return true;
}

void
Window::processKeyUpEvent(const keys::Key& key, const Modifiers& modifiers) {

	sendInner<KeyUp>(key, modifiers);
}

void
Window::processKeyDownEvent(const keys::Key& key, const Modifiers& modifiers) {

	sendInner<KeyDown>(key, modifiers);
}

void
Window::processFingerUpEvent(
		unsigned long               timestamp,
		const util::point<float,2>& position,
		int                         id,
		const Modifiers&            modifiers) {

	sendInner<FingerUp>(timestamp, position, id, modifiers);
}

void
Window::processFingerDownEvent(
		unsigned long               timestamp,
		const util::point<float,2>& position,
		int                         id,
		const Modifiers&            modifiers) {

	sendInner<FingerDown>(timestamp, position, id, modifiers);
}

void
Window::processFingerMoveEvent(
		unsigned long               timestamp,
		const util::point<float,2>& position,
		int                         id,
		const Modifiers&            modifiers) {

	sendInner<FingerMove>(timestamp, position, id, modifiers);
}

void
Window::processPenUpEvent(
		unsigned long               timestamp,
		const buttons::Button&      button,
		const util::point<float,2>& position,
		float                       pressure,
		const Modifiers&            modifiers) {

	sendInner<PenUp>(timestamp, button, position, pressure, modifiers);
}

void
Window::processPenDownEvent(
		unsigned long               timestamp,
		const buttons::Button&      button,
		const util::point<float,2>& position,
		float                       pressure,
		const Modifiers&            modifiers) {

	sendInner<PenDown>(timestamp, button, position, pressure, modifiers);
}

void
Window::processPenMoveEvent(
		unsigned long               timestamp,
		const util::point<float,2>& position,
		float                       pressure,
		const Modifiers&            modifiers) {

	LOG_ALL(winlog) << "[Window] sending signal pen move" << std::endl;

	sendInner<PenMove>(timestamp, position, pressure, modifiers);
}

void
Window::processPenInEvent(unsigned long timestamp) {

	sendInner<PenIn>(timestamp);
}

void
Window::processPenOutEvent(unsigned long timestamp) {

	sendInner<PenOut>(timestamp);
}

void
Window::processPenAwayEvent(unsigned long timestamp) {

	sendInner<PenAway>(timestamp);
}

void
Window::processButtonUpEvent(
		unsigned long               timestamp,
		const buttons::Button&      button,
		const util::point<float,2>& position,
		const Modifiers&            modifiers) {

	sendInner<MouseUp>(timestamp, button, position, modifiers);
}

void
Window::processButtonDownEvent(
		unsigned long               timestamp,
		const buttons::Button&      button,
		const util::point<float,2>& position,
		const Modifiers&            modifiers) {

	sendInner<MouseDown>(timestamp, button, position, modifiers);
}

void
Window::processMouseMoveEvent(
		unsigned long               timestamp,
		const util::point<float,2>& position,
		const Modifiers&            modifiers) {

	sendInner<MouseMove>(timestamp, position, modifiers);
}

GlContext*
Window::createGlContext() {

	LOG_ALL(winlog) << "[" << getCaption() << "] creating a new GlContext" << std::endl;

	GlContext* globalContext = OpenGl::getGlobalContext();
	globalContext->activate();

	ContextSettings settings;

	// create the context
	GlContext* glContext = new GlContext(this, settings, globalContext);

	// activate it
	glContext->activate();

	configureViewport();

	// return it
	return glContext;
}

void
Window::clear() {

	glClearColor(_clear_r, _clear_g, _clear_b, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GL_ASSERT;
}

void
Window::flush() {

	OpenGl::flush();
}

void
Window::requestFrameSave() {

	_saveFrameRequest = true;
}

void
Window::saveFrame() {

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

void
Window::onInnerSignal(SizeChanged& /*signal*/) {

	// TODO:
	// Here, we could resize the window to fit the view. However, this should be
	// an optional feature.

	setDirty();
}

void
Window::onInnerSignal(ContentChanged& /*signal*/) {

	LOG_ALL(winlog) << "[" << getCaption() << "] received a content change signal" << endl;

	setDirty();
}

void
Window::onInnerSignal(WindowFullscreen& signal) {

	LOG_ALL(winlog) << "[" << getCaption() << "] received a fullscreen request" << endl;

	setFullscreen(signal.fullscreen);

	setDirty();
}

void
Window::processCloseEvent(){

	LOG_DEBUG(winlog) << "[" << getCaption() << "] invalidating my GlContext" << endl;

	// ensure that our context is destructed
	OpenGl::Guard guard(0);

	LOG_DEBUG(winlog) << "[" << getCaption() << "] closing window now" << endl;

	close();
}

} // namespace sg_gui
