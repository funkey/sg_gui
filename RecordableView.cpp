#include "RecordableView.h"

namespace sg_gui {

RecordableView::RecordableView() :
	_displayList(0), /* glGenLists will never create list with number 0 */
	_displayListTranslucent(0) {}

RecordableView::~RecordableView() {

	if (glIsList(_displayList))
		glCheck(glDeleteLists(_displayList, 1));
	if (glIsList(_displayListTranslucent))
		glCheck(glDeleteLists(_displayListTranslucent, 1));
}

void
RecordableView::draw() {

	if (!glIsList(_displayList))
		return;

	glCallList(_displayList);
}

void
RecordableView::startRecording() {

	// create a new display list, if needed
	if (!glIsList(_displayList)) {

		_displayList = glGenLists(1);

		if (!glIsList(_displayList))
			UTIL_THROW_EXCEPTION(
					OpenGlError,
					"Couldn't create display list");
	}

	glCheck(glNewList(_displayList, GL_COMPILE));
}

void
RecordableView::stopRecording() {

	glCheck(glEndList());
}

void
RecordableView::drawTranslucent() {

	if (!glIsList(_displayListTranslucent))
		return;

	glCallList(_displayListTranslucent);
}

void
RecordableView::startRecordingTranslucent() {

	// create a new display list, if needed
	if (!glIsList(_displayListTranslucent)) {

		_displayListTranslucent = glGenLists(1);

		if (!glIsList(_displayListTranslucent))
			UTIL_THROW_EXCEPTION(
					OpenGlError,
					"Couldn't create display list");
	}

	glCheck(glNewList(_displayListTranslucent, GL_COMPILE));
}

void
RecordableView::stopRecordingTranslucent() {

	glCheck(glEndList());
}

} // namespace gui

