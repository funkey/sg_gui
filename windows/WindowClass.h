#define NOMINMAX
#include <windows.h>

namespace sg_gui {

class WindowClass {

public:

	static char* getDummyWindowClass();

	static char* getDefaultWindowClass();

private:

	static bool _dummyWindowClassRegistered;

	static bool _defaultWindowClassRegistered;
};

} // namespace sg_gui
