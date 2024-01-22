#pragma once
#include "../base/NativeUIScrollInfo_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

class NativeUIScrollInfo_Win32 : public NativeUIScrollInfo_Base {
	using Base = NativeUIScrollInfo_Base;
	using This = NativeUIScrollInfo_Win32;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onScrollTo(int pos) override;
public:
	void setRange(int min, int max);
	void setPage(UINT newPage);
	void setPos(int newPos);
	void setTrackPos(int newTrackPos);
	void setStep(int step) { _step = step; }

	void getRange(HWND hwnd, int& outMin, int& outMax);
	void getPage(HWND hwnd, UINT& out);
	void getPos(HWND hwnd, int& out);
	void getTrackPos(HWND hwnd, int& out);

	void reset(HWND hwnd, bool redraw = true);

	SCROLLINFO toSCROLLINFO(HWND hwnd);

	int		rangeMin()	const { return _si.nMin;  }
	int		rangeMax()	const { return _si.nMax;  }
	UINT	page()		const { return _si.nPage; }
	int		pos()		const { return _si.nPos;  }
//	int		trackPos()	const { return _si.nTrackPos; } // not provide such function, and use getTrackPos instead

	int		range()		const { return Math::max(0, _si.nMax - _si.nMin); }
	int		step()		const { return _step; }

private:
	void _retrieve(HWND hwnd, int flag = SIF_ALL, bool redraw = true);

	bool		_dirty : 1;
	int			_type;

	int			_step;

	SCROLLINFO	_si;
};

}

#endif // SGE_OS_WINDOWS