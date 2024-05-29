#pragma once

namespace sge {

enum class NativeUIScrollInfoAxis {
	Horizontal,
	Vertical,
};

struct NativeUIScrollInfo_CreateDesc {
	using Axis = NativeUIScrollInfoAxis;

	NativeUIScrollInfo_CreateDesc(Axis axis_) noexcept
		: axis(axis_)
	{}

	Axis	axis;
	Vec2i	range { 0,0 };
	int		step = 0;
	u32		page = 0;
};

class NativeUIScrollInfo_Base : public NonCopyable {
public:
	using CreateDesc	= NativeUIScrollInfo_CreateDesc;
	using Axis			= NativeUIScrollInfoAxis;

	void getAxis			(Axis& o)	{ onGetAxis(o); }
	void getPos				(int& o)	{ onGetPos(o); }
	void getStep			(int& o)	{ onGetStep(o);	}
	void getPage			(int& o)	{ onGetPage(o);	}
	void getRange			(int& o)	{ onGetRange(o); }
	void getTrackPos		(int& o)	{ onGetTrackPos(o); }
	void getMaxScrollPos	(int& o)	{ onGetMaxScrollPos(o);	}

	void setPos(int pos)				{ onSetPos(pos); }
	void setStep(int step)				{ onSetStep(step); }
	void setPage(int page)				{ onSetPage(page); }
	void setRange(int min, int max)		{ onSetRange(min, max); }
	void setTrackPos(int newTrackPos)	{ onSetTrackPos(newTrackPos); }

	void scrollTo(int pos)				{ onScrollTo(pos); }
	void scrollToHome()					{ onScrollToHome();	}
	void scrollToEnd ()					{ onScrollToEnd(); }
	void scrollStepForward ()			{ onScrollStepForward(); }
	void scrollStepBackward()			{ onScrollStepBackward(); }
	void scrollPageForward ()			{ onScrollPageForward(); }
	void scrollPageBackward()			{ onScrollPageBackward(); }

protected:
	NativeUIScrollInfo_Base() noexcept = default; // block constructor

	virtual void onGetAxis			(Axis& o) {}
	virtual void onGetPos			(int& o)  {}
	virtual void onGetStep			(int& o)  {}
	virtual void onGetPage			(int& o)  {}
	virtual void onGetRange			(int& o)  {}
	virtual void onGetTrackPos		(int& o)  {}
	virtual void onGetMaxScrollPos	(int& o)  {}

	virtual void onSetPos			(int newPos)		{}
	virtual void onSetStep			(int step)			{}
	virtual void onSetPage			(int newPage)		{}
	virtual void onSetRange			(int min, int max)	{}
	virtual void onSetTrackPos		(int newTrackPos)	{}

	virtual void onScrollTo(int pos)	{}
	virtual void onScrollToHome		 () {}
	virtual void onScrollToEnd		 () {}
	virtual void onScrollStepForward () {}
	virtual void onScrollStepBackward() {}
	virtual void onScrollPageForward () {}
	virtual void onScrollPageBackward() {}
};

}