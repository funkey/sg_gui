#ifndef GUI_SIGNALS_H__
#define GUI_SIGNALS_H__

#include <util/rect.hpp>

namespace gui {

/**
 * Base class for all gui signals.
 */
class GuiSignal {};

/**
 * Indicates a change of the content of a gui element.
 */
class ContentChanged : public GuiSignal { public: typedef GuiSignal parent_type; };

/**
 * SizeChanged is a ContentChanged, since the size of a gui element cannot
 * change without a change in the content.
 */
class SizeChanged : public ContentChanged {

public:

	typedef ContentChanged parent_type;

	SizeChanged() {}

	SizeChanged(const util::rect<double>& newSize) :
		_newSize(newSize) {}

	const util::rect<double>& getNewSize() {

		return _newSize;
	}

private:

	util::rect<double> _newSize;
};

/**
 * A resize signal, to be sent downstream to request the resize of painters.
 */
class Resize : public GuiSignal {

public:

	typedef GuiSignal parent_type;

	Resize() {}

	Resize(const util::rect<double>& toSize) :
		_toSize(toSize) {}

	const util::rect<double>& getSize() const {

		return _toSize;
	}

private:

	util::rect<double> _toSize;
};

/**
 * Base class for user input signals.
 */
class InputSignal : public GuiSignal {

public:

	typedef GuiSignal parent_type;

	InputSignal() :
		processed(false),
		timestamp(0) {}

	InputSignal(unsigned long timestamp_) :
		processed(false),
		timestamp(timestamp_) {}

	bool processed;

	unsigned long timestamp;
};

/*
 * Request drawing.
 */
class Draw : public GuiSignal {

public:

	typedef GuiSignal parent_type;

	Draw() :
		_roi(0, 0, 0, 0),
		_resolution(0, 0),
		_needsRedraw(false) {}

	Draw(
			const util::rect<double>&  roi,
			const util::point<double>& resolution) :
		_roi(roi),
		_resolution(resolution),
		_needsRedraw(false) {}

	util::rect<double>&  roi()        { return _roi; }
	util::point<double>& resolution() { return _resolution; }

	bool needsRedraw() { return _needsRedraw; }

private:

	util::rect<double>  _roi;
	util::point<double> _resolution;

	bool _needsRedraw;
};

} // namespace gui

#endif // GUI_SIGNALS_H__

