#include"ui.h"

namespace dzh {
	void UI::initGraph(int* filled, int filledNum) {
		// init graph
		initgraph(width, height);
		setbkcolor(YELLOW);
		cleardevice();
		setlinecolor(BLACK);
		setfillcolor(GREEN);
		int tl_x, tl_y, br_x, br_y;
		// add rect blocks
		for (int i = 1; i <= dim; i++) {		// row i
			for (int j = 1; j <= dim; j++) {	// col j
				tl_x = rectWidth * j;
				tl_y = rectWidth * i;
				br_x = rectWidth * (j + 1);
				br_y = rectWidth * (i + 1);
				fillrectangle(tl_x, tl_y, br_x, br_y);
			}
		}
		// init filled blocks
		int x = 0, y = 0;
		for (int i = 1; i <= dim * dim; i++) {
			if (filled[i - 1] == 0) {
				continue;
			}
			getPointsInGraph(i, x, y);
			getRectPoints(x, y, tl_x, tl_y, br_x, br_y);
			if (filled[i - 1]  == 1) {
				setfillcolor(WHITE);
				fillrectangle(tl_x, tl_y, br_x, br_y);
			}
			else if(filled[i - 1] == -1){
				setfillcolor(BLACK);
				fillrectangle(tl_x, tl_y, br_x, br_y);
			}
		}	
		// solve block
		tl_x = rectWidth * 3.0 / 2;
		br_x = width - rectWidth * 3.0 / 2;
		tl_y = height - rectWidth * 3.0 / 4;
		br_y = height - rectWidth * 1.0 / 4;
		setfillcolor(WHITE);
		fillrectangle(tl_x, tl_y, br_x, br_y);
		setbkcolor(WHITE);
		settextcolor(BLACK);
		settextstyle(rectWidth * 3.0 / 8, rectWidth * 3.0 / 8, L"楷体");
		outtextxy((tl_x + br_x) / 2.0 - 5.0 / 2 * rectWidth * 3.0 / 8, (tl_y + br_y) / 2.0 - 1.0 / 2 * rectWidth * 3.0 / 8, L"solve");
		// hint
		setbkcolor(YELLOW);
		outtextxy(0, 0, L"L-WHITE");
		outtextxy(0, rectWidth/2.0, L"R-BLACK");
		
		return;
	}

	bool UI::toSolve(MOUSEMSG mouse) {
		int tl_x, tl_y, br_x, br_y;
		tl_x = rectWidth * 3.0 / 2;
		br_x = width - rectWidth * 3.0 / 2;
		tl_y = height - rectWidth * 3.0 / 4;
		br_y = height - rectWidth * 1.0 / 4;
		if (mouse.x >= tl_x && mouse.x <= br_x &&
			mouse.y >= tl_y && mouse.y <= br_y &&
			mouse.uMsg == WM_LBUTTONDOWN) {
			return true;
		}
		return false;
	}

	void UI::addRect(MOUSEMSG mouse, int* filled) {
		int tl_x, tl_y, br_x, br_y;
		if (mouse.x >= rectWidth && mouse.x <= width - rectWidth
			&& mouse.y >= rectWidth && mouse.y <= width - rectWidth) {
			int x = mouse.x / rectWidth;
			int y = mouse.y / rectWidth;
			int i = (y - 1) * dim + x;
			getRectPoints(mouse.x, mouse.y, tl_x, tl_y, br_x, br_y);
			if (mouse.uMsg == WM_RBUTTONDOWN && filled[i - 1] == 0) {
				setfillcolor(BLACK);
				fillrectangle(tl_x, tl_y, br_x, br_y);
				filled[i - 1] = -1;
			}
			else if (mouse.uMsg == WM_LBUTTONDOWN && filled[i - 1] == 0) {
				setfillcolor(WHITE);
				fillrectangle(tl_x, tl_y, br_x, br_y);
				filled[i - 1] = 1;
			}
		}
		return;
	}

	/// <summary>
	/// 返回坐标（i，j）所在的矩形的左上角点和右下角点
	/// </summary>
	/// <param name="i"></param>
	/// <param name="j"></param>
	/// <param name="tl_x"></param>
	/// <param name="tl_y"></param>
	/// <param name="br_x"></param>
	/// <param name="br_y"></param>
	void UI::getRectPoints(int i, int j, int& tl_x, int& tl_y, int& br_x, int& br_y) {
		tl_x = i / rectWidth * rectWidth;
		tl_y = j / rectWidth * rectWidth;
		br_x = (i / rectWidth + 1) * rectWidth;
		br_y = (j / rectWidth + 1) * rectWidth;
		return;
	}

	/// <summary>
	/// 把二维数组中的点转换成图像中的坐标
	/// </summary>
	/// <param name="fill"> 1 - dim*dim </param>
	void UI::getPointsInGraph(int fill, int& x, int& y){
		int r = fill / dim + (fill % dim == 0 ? 0 : 1);
		int c = fill % dim == 0 ? dim : fill % dim;
		y = r * rectWidth;
		x = c * rectWidth;
		return;
	}
}