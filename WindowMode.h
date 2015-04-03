#ifndef SG_GUI_WINDOW_MODE_H__
#define SG_GUI_WINDOW_MODE_H__

#include <util/point.hpp>

using namespace util;

namespace sg_gui {

struct WindowMode {

	WindowMode(
			int  width  = 320,
			int  height = 240,
			int  x = 0,
			int  y = 0,
			int  depth_      = 32,
			bool fullscreen_ = false,
			bool hideCursor_ = false) :
		size(width, height),
		position((fullscreen_ ? 0 : x), (fullscreen_ ? 0 : y)),
		depth(depth_),
		fullscreen(fullscreen_),
		hideCursor(hideCursor_) {}

	WindowMode(
			const point<int,2>& size_,
			const point<int,2>& position_ = point<int,2>(0, 0),
			int   depth_                  = 32,
			bool  fullscreen_             = false) :
		size(size_),
		position(fullscreen_ ? point<int,2>(0, 0) : position_),
		depth(depth_),
		fullscreen(fullscreen_) {}

	point<int,2> size;

	point<int,2> position;

	int depth;

	bool fullscreen;

	bool hideCursor;
};

} // namespace sg_gui

#endif // SG_GUI_WINDOW_MODE_H__

