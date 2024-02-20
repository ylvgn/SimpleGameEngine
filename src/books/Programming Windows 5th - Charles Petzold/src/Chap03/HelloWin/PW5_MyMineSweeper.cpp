#if SGE_OS_WINDOWS

#include "PW5_MyMineSweeper.h"

namespace sge {

PW5_MyMineSweeper* PW5_MyMineSweeper::s_instance = nullptr;

void Cell::init(int x_, int y_) {
	x = x_;
	y = y_;

	hasMine = false;
	opened = false;
	mineCount = 0;
}

void Cell::onDraw() {
	auto& hdc = PW5_MyMineSweeper::instance()->hdc();

	int sx = x * kSize;
	int sy = y * kSize;

	if (opened) {
		::SelectObject(hdc, ::GetStockObject(WHITE_BRUSH));
	} else {
		::SelectObject(hdc, ::GetStockObject(GRAY_BRUSH));
	}

	Rectangle(hdc, sx, sy, sx + kSize, sy + kSize);

	if (!opened) return;

	if (hasMine) {
		GDI::textOut(hdc, sx + 3, sy + 3, "@");
	} else {
		if (mineCount > 0) {
			GDI::Fmt_textOut(hdc, sx + 3, sy + 3, "{}", mineCount);
		}
	}
}

void Grid::create(int w, int h) {
	_width  = w;
	_height = h;

	int n = w * h;
	_cells.resize(n);

	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			auto* center = getCell(x, y);
			center->init(x, y);
		}
	}

	for (int i = 0; i < w; i++) {
		int index = rand() % n;
		_cells[index].hasMine = true;
	}

	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			auto* center = getCell(x, y);

			_updateMineCount(center, getCell(x - 1, y - 1));
			_updateMineCount(center, getCell(x - 1, y ));
			_updateMineCount(center, getCell(x - 1, y + 1));

			_updateMineCount(center, getCell(x, y - 1));
			_updateMineCount(center, getCell(x, y + 1));

			_updateMineCount(center, getCell(x + 1, y - 1));
			_updateMineCount(center, getCell(x + 1, y ));
			_updateMineCount(center, getCell(x + 1, y + 1));
		}
	}
}

void Grid::_updateMineCount(Cell* center, Cell* another) {
	if (!another) return;
	if (another->hasMine)
		center->mineCount++;
}

Cell* Grid::getCell(int x, int y) {
	// 00-------> x (width)
	// +
	// +
	// +
	// y (height)
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return nullptr;
	return &_cells[y * _width + x];
}

void Grid::onDraw() {
	for (auto& c : _cells) {
		c.onDraw();
	}
}

void Grid::onClick(int x, int y) {
	auto* c = getCell(x, y);
	if (!c) return;
	
	if (c->hasMine) {
		::MessageBox(PW5_MyMineSweeper::instance()->hwnd(), L"Game Over", L"", MB_OK);
		create(_width, _height);
		return;
	}

	openCell(c);
}

void Grid::openCell(Cell* c) {
	if (!c)
		return;

	if (c->opened) return;
	c->opened = true;

	int x = c->x;
	int y = c->y;

	if (c->mineCount > 0)
		return;

	openCell(getCell(x - 1, y - 1));
	openCell(getCell(x - 1, y));
	openCell(getCell(x - 1, y + 1));

	openCell(getCell(x, y - 1));
	openCell(getCell(x, y + 1));

	openCell(getCell(x + 1, y - 1));
	openCell(getCell(x + 1, y));
	openCell(getCell(x + 1, y + 1));
}

void PW5_MyMineSweeper::onCreate(CreateDesc& desc) {
	desc.ownDC = true;

	s_instance = this;
	_grid.create(32, 16);

	Base::onCreate(desc);

	_hdc = GetDC(_hwnd);
}

void PW5_MyMineSweeper::onDraw() {
	_grid.onDraw();
}

void PW5_MyMineSweeper::onUIMouseEvent(UIMouseEvent& ev) {
	using Button = UIMouseEvent::Button;
	if (ev.pressedButtons == Button::Left && ev.isDown()) {
		int cx = static_cast<int>(ev.pos.x / Cell::kSize);
		int cy = static_cast<int>(ev.pos.y / Cell::kSize);
		_grid.onClick(cx, cy);
	}
	drawNeeded();
}

}

#endif // SGE_OS_WINDOWS