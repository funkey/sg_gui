#include "VolumeView.h"

namespace sg_gui {

VolumeView::VolumeView() :
		_imageView(std::make_shared<ImageView>()),
		_index(0) {

	add(_imageView);
}

void
VolumeView::setVolume(std::shared_ptr<ExplicitVolume<float>> volume) {

	_volume = volume;

	updateImage();
}

void
VolumeView::onSignal(MouseDown& signal) {

	if (signal.processed)
		return;

	if (signal.button == sg_gui::buttons::WheelUp)
		_index = std::min((int)_volume->depth() - 1, _index + 1);
	else if (signal.button == sg_gui::buttons::WheelDown)
		_index = std::max(0, _index - 1);
	else
		return;

	updateImage();
}

void
VolumeView::onSignal(QuerySize& signal) {

	if (!_volume)
		return;

	signal.setSize(_volume->getBoundingBox());
}

void
VolumeView::updateImage() {

	auto image = std::make_shared<Image>(_volume->width(), _volume->height());

	*image = _volume->slice(_index);

	sendInner<SetImage>(image);
}

} // namespace sg_gui
