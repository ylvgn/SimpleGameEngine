#pragma once

namespace sge {

struct NativeUIScrollInfo_CreateDesc {
	enum class Axis {
		Horizontal,
		Vertical,
	};

	NativeUIScrollInfo_CreateDesc(Axis axis_)
		: axis(axis_)
	{}

	Axis				axis;
	RangeMinMaxValuei	range { 0,0,0 };
	int					step = 0;
	u32					page = 0;
};

class NativeUIScrollInfo_Base : public NonCopyable {
public:
	using CreateDesc = NativeUIScrollInfo_CreateDesc;

	void create(CreateDesc& desc)	{ onCreate(desc); }
	void scrollTo(int pos)			{ onScrollTo(pos); }

protected:
	virtual void onCreate(CreateDesc& desc) {}
	virtual void onScrollTo(int pos) {}
};

}