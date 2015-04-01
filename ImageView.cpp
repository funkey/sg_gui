#include "Colors.h"
#include "ImageView.h"

namespace sg_gui {

ImageView::~ImageView() {

	if (_texture != 0)
		delete _texture;
}

void
ImageView::onSignal(Draw& /*signal*/) {

	// wait for image
	if (!_image)
		return;

	// wait for content
	if (_image->width() == 0 || _image->height() == 0)
		return;

	if (_needReload)
		loadTexture();

	glEnable(GL_TEXTURE_2D);

	_texture->bind();

	glColor3f(_red, _green, _blue);
	if (_transparent) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	}

	util::box<float>& bb = _image->getBoundingBox();
	float minX = bb.minX;
	float minY = bb.minY;
	float maxX = bb.maxX;
	float maxY = bb.maxY;
	float z    = bb.minZ;

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 1.0); glVertex3d(minX, maxY, z);
	glTexCoord2d(1.0, 1.0); glVertex3d(maxX, maxY, z);
	glTexCoord2d(1.0, 0.0); glVertex3d(maxX, minY, z);
	glTexCoord2d(0.0, 0.0); glVertex3d(minX, minY, z);
	glEnd();

	if (_transparent)
		glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	return;
}

void
ImageView::onSignal(QuerySize& signal) {

	if (!_image)
		return;

	util::rect<double> bb(
			0, 0,
			_image->width(),
			_image->height());

	signal.setSize(bb);
}

void
ImageView::onSignal(SetImage& signal) {

	_image = signal.getImage();

	send<ContentChanged>();
}

void
ImageView::loadTexture() {

	if (!_image || _image->width()*_image->height() == 0)
		return;

	// ensure that OpenGl operations are save
	OpenGl::Guard guard;

	if (!_texture) {

		_texture = new Texture(_image->width(), _image->height(), GL_RGBA);

	} else {

		if (_image->width()  != _texture->width() ||
		    _image->height() != _texture->height()) {

			_texture->resize(_image->width(), _image->height());
		}
	}

	float min = 0.0f;
	float max = 1.0f;
	_image->minmax(&min, &max);

	if (max > 1.0) {

		// consider this image as a color index image
		std::vector<boost::array<unsigned char, 4> > colorImage;
		colorImage.reserve(_image->size());

		boost::array<unsigned char, 4> pixel;
		pixel[3] = 255;

		for (typename Image::iterator i = _image->begin(); i != _image->end(); i++) {

			// fire
			//float h = ((*i) - min)/(max - min);//fmod(static_cast<float>(*i)*M_PI, 1.0);

			// intensities above one are handled as color indices
			if (*i >= 1.0) {

				idToRgb(*i, pixel[0], pixel[1], pixel[2]);

			// intensities below 1 are handled as grayscale
			} else {

				pixel[0] = (*i)*255.0;
				pixel[1] = (*i)*255.0;
				pixel[2] = (*i)*255.0;
			}
			colorImage.push_back(pixel);
		}

		_texture->loadData(&(*colorImage.begin()));

	} else {

		// non-intensity images are just loaded
		_texture->loadData(&(*_image->begin()));
	}
}

} // namespace sg_gui
