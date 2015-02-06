#ifndef SG_GUI_RECORDABLE_VIEW_H__
#define SG_GUI_RECORDABLE_VIEW_H__

#include <sg_gui/OpenGl.h>

namespace sg_gui {

class RecordableView {

public:

	RecordableView();

	/**
	 * Destructor to free display list.
	 */
	virtual ~RecordableView();

protected:

	/**
	 * Start the recording of OpenGl commands.
	 */
	void startRecording();

	/**
	 * Stop the recording of OpenGl commands.
	 */
	void stopRecording();

	/**
	 * Draws the recorded content of this painter.
	 */
	void draw();

private:

	// the display list holding the OpenGl commands
	GLuint _displayList;
};

} // namespace sg_gui

#endif // SG_GUI_RECORDABLE_VIEW_H__

