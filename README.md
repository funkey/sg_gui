<!--- vim:spell:tw=80:fo=tawn
--->
sg_gui
======

**sg_gui** is a collection of [scopegraph](http://github.com/funkey/scopegraph)
agents and scopes for simple OpenGl GUIs.

Usage Example
-------------

The following example code creates a window, a zoom view, and shows a simple
painter. You can pan the conent of the zoom view with `CTRL` + mouse drag, and
zoom with `CTRL` + mouse wheel.

```c++
#include <sg_gui/Window.h>
#include <sg_gui/ZoomView.h>

using namespace sg;
using namespace sg_gui;

/**
 * A simple example painter, that draws a yellow square. A painter is just an
 * Agent that accepts the Draw signal.
 */
class SimplePainter :
		public Agent<
			SimplePainter,
			Accepts<Draw>> {

public:

	void onSignal(Draw&) {

		glDisable(GL_CULL_FACE);

		glColor3f(1.0, 1.0, 0.0);

		// a square
		glBegin(GL_QUADS);
		glVertex2f(0.0, 0.0);
		glVertex2f(0.0, 100.0);
		glVertex2f(100.0, 100.0);
		glVertex2f(100.0, 0.0);
		glVertex2f(0.0, 0.0);
		glEnd();
	}
};

int main(int argc, char** argv) {

  auto window  = std::make_shared<Window>("test window");
  auto zoom    = std::make_shared<ZoomView>();
  auto painter = std::make_shared<SimplePainter>();

  window->add(zoom);
  zoom->add(painter);

  // enter the window's main loop
  window->processEvents();

  return 0;
}
```
