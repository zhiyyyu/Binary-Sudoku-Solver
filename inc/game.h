#pragma once

#include"sudoku.h"
#include"dataLoader.h"

namespace dzh {
	class Game {
	public:
		int* initGame();
	
	private:
		void lasVegas(int* filled, int n);
		bool isPossibleFill(int* filled);
		void writeFileHeader(int filledNum);

		std::string path = "../a.cnf";
	};
}