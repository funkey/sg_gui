#ifndef SG_GUI_VOLUME_VIEW_H__
#define SG_GUI_VOLUME_VIEW_H__

#include <scopegraph/Scope.h>
#include <sg_gui/MouseSignals.h>
#include <imageprocessing/ExplicitVolume.h>
#include "ImageView.h"

namespace sg_gui {

class VolumePointSelected {

public:

	VolumePointSelected(const util::point<float,3>& position) :
			_position(position) {}

	const util::point<float,3>& position() { return _position; }

private:

	util::point<float,3> _position;
};

class VolumeView :
		public sg::Scope<
				VolumeView,
				sg::Accepts<
						PointerDown,
						QuerySize
				>,
				sg::Provides<
						VolumePointSelected
				>,
				sg::ProvidesInner<
						SetImage
				>,
				sg::PassesUp<
						ContentChanged
				>
		> {

public:

	VolumeView();

	void setVolume(std::shared_ptr<ExplicitVolume<float>> volume);

	void onSignal(PointerDown& signal);

	void onSignal(QuerySize& signal);

private:

	void updateImage();

	std::shared_ptr<ImageView> _imageView;

	std::shared_ptr<ExplicitVolume<float>> _volume;

	int _index;

	util::point<float,3> _prevPointerDown;
};

} // namespace sg_gui

#endif // SG_GUI_VOLUME_VIEW_H__

