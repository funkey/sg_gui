#ifndef SG_GUI_SIGNALS_H__
#define SG_GUI_SIGNALS_H__

#include <util/rect.hpp>
#include <util/box.hpp>
#include <scopegraph/Signals.h>

namespace sg_gui {

/**
 * Base class for all gui signals.
 */
class GuiSignal : public sg::Signal { public: typedef sg::Signal parent_type; };

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
 * A signal to query the size of all downstream views.
 */
class QuerySize : public GuiSignal {

public:

	typedef GuiSignal parent_type;

	/**
	 * Answer the size query. To be called by accepting agents.
	 */
	void setSize(const util::box<double>& size) {

		_size.fit(size);
	}

	/**
	 * Answer the size query. To be called by accepting agents.
	 */
	void setSize(const util::rect<double>& size) {

		_size.fit(size);
	}

	/**
	 * Get the size of all downstream views combined.
	 */
	const util::box<double>& getSize() const {

		return _size;
	}

private:

	util::box<double> _size;
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
 * Base class for signals limited to a certain region of interest. The 
 * resolution of the region in terms of pixels per unit is passed along as well.
 */
class RoiSignal : public GuiSignal {

public:

	typedef GuiSignal parent_type;

	RoiSignal() :
		_roi(0, 0, 0, 0),
		_resolution(0, 0) {}

	RoiSignal(
			const util::rect<double>&    roi,
			const util::point<double,2>& resolution) :
		_roi(roi),
		_resolution(resolution) {}

	util::rect<double>&  roi()        { return _roi; }
	util::point<double,2>& resolution() { return _resolution; }

private:

	util::rect<double>  _roi;
	util::point<double,2> _resolution;
};

/**
 * Base class for all draw signals.
 */
class DrawBase : public RoiSignal {

public:

	typedef RoiSignal parent_type;

	DrawBase() : _needsRedraw(false) {}

	bool needsRedraw() { return _needsRedraw; }

private:

	bool _needsRedraw;
};

/**
 * Draw signal for drawing opaque content.
 */
class DrawOpaque : public DrawBase { public: typedef DrawBase parent_type; };

/**
 * Convenience typedef, if you don't care about opaque versus translucent.
 */
typedef DrawOpaque Draw;

/**
 * Draw signal for drawing translucent content.
 */
class DrawTranslucent : public DrawBase { public: typedef DrawBase parent_type; };

} // namespace sg_gui

#endif // SG_GUI_SIGNALS_H__

