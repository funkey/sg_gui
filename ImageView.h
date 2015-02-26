#ifndef SG_GUI_IMAGE_VIEW_H__
#define SG_GUI_IMAGE_VIEW_H__

#include <scopegraph/Agent.h>
#include "Texture.h"
#include "GuiSignals.h"
#include "Image.h"

namespace sg_gui {

class SetImage : public GuiSignal {

public:

	typedef GuiSignal parent_type;

	SetImage(std::shared_ptr<Image> image) :
		_image(image) {}

	std::shared_ptr<Image> getImage() { return _image; }

private:

	std::shared_ptr<Image> _image;
};

class ImageView :
		public sg::Agent<
				ImageView,
				sg::Accepts<
						Draw,
						QuerySize,
						SetImage
				>,
				sg::Provides<
						ContentChanged
				>
		> {

public:

	ImageView() :
		_texture(0),
		_transparent(false),
		_red(1.0), _green(1.0), _blue(1.0),
		_needReload(true) {}

	~ImageView();

	void onSignal(Draw& signal);

	void onSignal(QuerySize& signal);

	void onSignal(SetImage& signal);

private:

	void loadTexture();

	std::shared_ptr<Image> _image;

	Texture* _texture;

	// if set, dark = transparent
	bool _transparent;

	// colorization of intensity images
	float _red, _green, _blue;

	bool _needReload;
};

} // namespace sg_gui

#endif // SG_GUI_IMAGE_VIEW_H__

