#pragma once
#include "../base/NativeUIScrollInfo_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

class NativeUIWindow_Win32;

class NativeUIScrollInfo_Win32 : public NativeUIScrollInfo_Base {
	using Base = NativeUIScrollInfo_Base;
	using This = NativeUIScrollInfo_Win32;
public:
	NativeUIScrollInfo_Win32(NativeUIWindow_Win32* window, CreateDesc& desc);

	bool	isVertical()   const { return _win32_axis == SB_VERT; }
	bool	isHorizontal() const { return _win32_axis == SB_HORZ; }

	void	refresh(bool redraw = true);

	int		_win32_step()		const { return _step;	  }
	int		_win32_pos()		const { return _si.nPos;  }
	UINT	_win32_page()		const { return _si.nPage; }
	int		_win32_rangeMin()	const { return _si.nMin;  }
	int		_win32_rangeMax()	const { return _si.nMax;  }
	int		_win32_range()		const { return Math::max(0, _si.nMax - _si.nMin); }
	int		_win32_trackPos()	const { return _si.nTrackPos; } // mostly use getTrackPos(int&) instead

	int		_win32_axis;

protected:
	virtual void onScrollTo(int pos)	   override;
	virtual void onScrollToHome			() override;
	virtual void onScrollToEnd			() override;
	virtual void onScrollStepForward	() override;
	virtual void onScrollStepBackward	() override;
	virtual void onScrollPageForward	() override;
	virtual void onScrollPageBackward	() override;

	virtual void onSetPos			(int newPos)		override;
	virtual void onSetStep			(int newStep)		override;
	virtual void onSetRange			(int min, int max)	override;
	virtual void onSetPage			(int newPage)		override;
	virtual void onSetTrackPos		(int newTrackPos)	override;

	virtual void onGetPos			(int& o) override;
	virtual void onGetStep			(int& o) override;
	virtual void onGetPage			(int& o) override;
	virtual void onGetRange			(int& o) override;
	virtual void onGetTrackPos		(int& o) override;
	virtual void onGetMaxScrollPos	(int& o) override;

private:
	void _retrieve(int flag = SIF_ALL, bool redraw = true);
	void _refreshIfDirty();
	void _refreshScrollMaxPos();

	NativeUIWindow_Win32* _window = nullptr;
	SCROLLINFO _si;

	bool	_dirty	: 1;
	int		_step			= 0;
	int		_maxScrollPos	= 0;
};

}

#endif // SGE_OS_WINDOWS