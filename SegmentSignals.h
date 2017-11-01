#ifndef SG_GUI_SEGMENT_SIGNALS_H__
#define SG_GUI_SEGMENT_SIGNALS_H__

namespace sg_gui {

class SegmentSignal : sg::Signal {

public:

	SegmentSignal(uint64_t id) : _id(id) {}

	uint64_t getId() { return _id; }

private:

	uint64_t _id;
};

class ShowSegment : public SegmentSignal { public: ShowSegment(uint64_t id) : SegmentSignal(id) {} };
class HideSegment : public SegmentSignal { public: HideSegment(uint64_t id) : SegmentSignal(id) {} };

} // namespace sg_gui

#endif // SG_GUI_SEGMENT_SIGNALS_H__

