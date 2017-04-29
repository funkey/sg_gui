#ifndef SG_GUI_SEGMENT_SIGNALS_H__
#define SG_GUI_SEGMENT_SIGNALS_H__

namespace sg_gui {

class SegmentSignal : sg::Signal {

public:

	SegmentSignal(unsigned int id) : _id(id) {}

	unsigned int getId() { return _id; }

private:

	unsigned int _id;
};

class ShowSegment : public SegmentSignal { public: ShowSegment(unsigned int id) : SegmentSignal(id) {} };
class HideSegment : public SegmentSignal { public: HideSegment(unsigned int id) : SegmentSignal(id) {} };

} // namespace sg_gui

#endif // SG_GUI_SEGMENT_SIGNALS_H__

