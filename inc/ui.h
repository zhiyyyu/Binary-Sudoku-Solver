#pragma once

#include<easyx.h>
#include<graphics.h>
#include<iostream>
#include"solve.h"

namespace dzh {
	class UI {
	public:
		UI() = default;
		~UI() = default;
		static void initGraph(int* filled, int filledNum);
		static void addRect(MOUSEMSG mouse, int* filled);
		static bool toSolve(MOUSEMSG mouse);
		static void getPointsInGraph(int fill, int& x, int& y);
		static void getRectPoints(int i, int j, int& tl_x, int& tl_y, int& br_x, int& br_y);

	private:
		static const int height = 480;
		static const int width = 480;
		static const int rectWidth = height / (dim + 2);
	};
}