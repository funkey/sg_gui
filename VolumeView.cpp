#include "VolumeView.h"
#include <util/plane.hpp>
#include <util/geometry.hpp>

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
VolumeView::onSignal(PointerDown& signal) {

	if (signal.processed)
		return;

	if (signal.modifiers != sg_gui::NoModifier)
		return;

	if (signal.button == sg_gui::buttons::WheelUp)
		_index = std::min((int)_volume->depth() - 1, _index + 1);
	else if (signal.button == sg_gui::buttons::WheelDown)
		_index = std::max(0, _index - 1);
	else if (signal.button == sg_gui::buttons::Left) {

		// find the intersection of the click ray with the current image

		// z-position of the current image plane
		float imageZ = _volume->getBoundingBox().min().z() + _volume->getResolutionZ()*_index;

		util::plane<float,3> imagePlane(
				util::point<float,3>(0, 0, imageZ),
				util::point<float,3>(0, 0, 1));

		float t;
		if (!util::intersect(imagePlane, signal.ray, t))
			return;

		util::point<float,3> intersection = signal.ray.at(t);

		if (!_volume->getBoundingBox().contains(intersection))
			return;

		util::point<float,3> prev = _prevPointerDown;
		_prevPointerDown = intersection;

		unsigned int x, y, z;
		auto image = _imageView->getImage();
		image->getDiscreteCoordinates(
				intersection.x(),
				intersection.y(),
				intersection.z(),
				x, y, z);
		std::cout << "value at " << intersection << " is " << (*image)(x, y, z) << std::endl;

		if (util::length(prev - intersection) >= 1.0)
			return;

		send<VolumePointSelected>(intersection);

	} else
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
