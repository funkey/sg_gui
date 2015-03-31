#ifndef SG_GUI_VOLUME_VIEW_H__
#define SG_GUI_VOLUME_VIEW_H__

#include <scopegraph/Scope.h>
#include <sg_gui/MouseSignals.h>
#include <imageprocessing/ExplicitVolume.h>
#include "ImageView.h"

namespace sg_gui {

class VolumeView :
		public sg::Scope<
				VolumeView,
				sg::Accepts<
						MouseDown
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

	void onSignal(MouseDown& signal);

private:

	void updateImage();

	std::shared_ptr<ImageView> _imageView;

	std::shared_ptr<ExplicitVolume<float>> _volume;

	int _index;
};

} // namespace sg_gui

#endif // SG_GUI_VOLUME_VIEW_H__

