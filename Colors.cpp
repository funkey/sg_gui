#include <cmath>
#include "Colors.h"

namespace sg_gui {

void
hsvToRgb(double h, double s, double v, unsigned char& r, unsigned char& g, unsigned char& b) {

	if(s < 0) s = 0;
	if(s > 1) s = 1;
	if(v < 0) v = 0;
	if(v > 1) v = 1;

	if(s == 0) {
		r = (unsigned char)255.0*v;
		g = (unsigned char)255.0*v;
		b = (unsigned char)255.0*v;
	}

	h = fmod(h, 1.0); // want h to be in 0..1

	unsigned int i = h*6;
	double f = (h*6) - i;
	double p = v*(1.0f - s); 
	double q = v*(1.0f - s*f);
	double t = v*(1.0f - s*(1.0f-f));
	switch(i%6) {
	case 0:
		r = (unsigned char)255.0*v;
		g = (unsigned char)255.0*t;
		b = (unsigned char)255.0*p;
		return;
	case 1:
		r = (unsigned char)255.0*q;
		g = (unsigned char)255.0*v;
		b = (unsigned char)255.0*p;
		return;
	case 2:
		r = (unsigned char)255.0*p;
		g = (unsigned char)255.0*v;
		b = (unsigned char)255.0*t;
		return;
	case 3:
		r = (unsigned char)255.0*p;
		g = (unsigned char)255.0*q;
		b = (unsigned char)255.0*v;
		return;
	case 4:
		r = (unsigned char)255.0*t;
		g = (unsigned char)255.0*p;
		b = (unsigned char)255.0*v;
		return;
	case 5:
		r = (unsigned char)255.0*v;
		g = (unsigned char)255.0*p;
		b = (unsigned char)255.0*q;
		return;
	}
}

void
idToRgb(unsigned int id, unsigned char& r, unsigned char& g, unsigned char& b) {

	float x = fmod(0.4671057256451202*id, 1.0);
	float y = fmod(0.6262286337141059*id, 1.0);
	float z = fmod(0.9424373277692188*id, 1.0);

	float h = x;
	float s = 0.25 + y*0.75;
	float v = (id == 0 ? 0.0 : 0.5 + z*0.5);
	hsvToRgb(h, s, v, r, g, b);
}

} // namespace sg_gui
