#ifndef SG_GUI_IMAGE_VIEW_H__
#define SG_GUI_IMAGE_VIEW_H__

#include <scopegraph/Agent.h>
#include <imageprocessing/Image.h>
#include "Texture.h"
#include "GuiSignals.h"
#include "ViewSignals.h"

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
						DrawOpaque,
						DrawTranslucent,
						QuerySize,
						SetImage,
						ChangeAlpha
				>,
				sg::Provides<
						ContentChanged
				>
		> {

public:

	ImageView() :
		_texture(0),
		_red(1.0), _green(1.0), _blue(1.0),
		_needReload(true),
		_alpha(1.0) {}

	~ImageView();

	void onSignal(DrawOpaque& signal);
	void onSignal(DrawTranslucent& signal);

	void onSignal(QuerySize& signal);

	void onSignal(SetImage& signal);

	void onSignal(ChangeAlpha& signal);

private:

	void loadTexture();

	void draw();

	std::shared_ptr<Image> _image;

	Texture* _texture;

	// colorization of intensity images
	float _red, _green, _blue;

	bool _needReload;

	float _alpha;
};

} // namespace sg_gui

#endif // SG_GUI_IMAGE_VIEW_H__

