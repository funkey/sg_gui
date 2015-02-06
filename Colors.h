#ifndef SG_GUI_COLORS_H__
#define SG_GUI_COLORS_H__

namespace sg_gui {

/**
 * Get the RGB values for hsv.
 */
void hsvToRgb(double h, double s, double v, unsigned char& r, unsigned char& g, unsigned char& b);

/**
 * Get a (more or less random) color for an id.
 */
void idToRgb(unsigned int id, unsigned char& r, unsigned char& g, unsigned char& b);

} // namespace sg_gui

#endif // SG_GUI_COLORS_H__

