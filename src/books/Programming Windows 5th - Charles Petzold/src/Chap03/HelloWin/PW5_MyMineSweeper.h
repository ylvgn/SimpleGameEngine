#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class Cell {
public:
	void init(int x_, int y_);
	void onDraw();

	static const int kSize = 22;

	int		x, y;

	bool	hasMine = false;
	bool	opened  = false;
	int		mineCount = 0;
};

class Grid {
public:
	void create(int w, int h);

	Cell* getCell(int x, int y);

	void onDraw();
	void onClick(int x, int y);
	void openCell(Cell* c);

private:
	void _updateMineCount(Cell* center, Cell* another);

	int _width;
	int _height;

	Vector<Cell> _cells;
};

class PW5_MyMineSweeper : public PW5_NativeUIWindow {
	using This = PW5_MyMineSweeper;
	using Base = PW5_NativeUIWindow;
public:
	static This*	instance() { return s_instance; }
	const HDC&		hdc() const { return _hdc; }
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
private:
	static This* s_instance;
	HDC _hdc = nullptr;

	Grid _grid;
};

}

#endif // SGE_OS_WINDOWS