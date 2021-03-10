#include"game.h"

namespace dzh {
	int* Game::initGame() {
		cnfData* loader = new cnfData();
		SAT* solver = new SAT();
		// generate data
		int filledNum = dim * dim / 3;
		int* filled = (int*)malloc(sizeof(int) * dim * dim);
		if (!filled) return nullptr;
		memset(filled, 0, sizeof(int) * dim * dim);
		lasVegas(filled, filledNum / 2);

		delete solver;
		delete loader;
		return filled;
	}

	void Game::writeFileHeader(int filledNum) {
		Sudoku* sudoku = new Sudoku();
		int cnt = 0;		// 约束二的个数
		sudoku->combination(dim, dim / 2 + 1, cnt);
		int clauseNum = (dim - 2) * 2 * 2 * dim + cnt * 2 * dim * 2 + (9 * dim + 1) * dim * (dim - 1) + filledNum;
		int variableNum = dim * dim + 3 * dim * dim * (dim - 1);
		std::ofstream fileWriter(path, std::ios::out);
		fileWriter << "p cnf " << variableNum << " " << clauseNum << std::endl;
		delete sudoku;
		return;
	}

	void Game::lasVegas(int* filled, int n) {
		srand((unsigned int)(time(NULL)));
		while (n > 0) {
			int i1 = rand() % dim + 1;
			int j1 = rand() % dim + 1;
			int p1 = rand() % 2;
			int var1 = (i1 - 1) * dim + (j1 - 1);
			int i2 = rand() % dim + 1;
			int j2 = rand() % dim + 1;
			int p2 = rand() % 2;
			int var2 = (i2 - 1) * dim + (j2 - 1);
			if (filled[var1] == 0 && filled[var2] == 0) {
				filled[var1] = p1 == 1 ? 1 : -1;
				filled[var2] = p2 == 1 ? 1 : -1;
				n--;
				if (!isPossibleFill(filled)) {
					filled[var1] = 0;
					filled[var2] = 0;
					n++;
				}
			}
		}
		return;
	}

	bool Game::isPossibleFill(int* filled) {
		cnfData* loader = new cnfData();
		SAT* solver = new SAT();
		Sudoku* sudoku = new Sudoku();
		int filledNum = dim * dim / 3;			// TODO 统一一下几个地方的filledNum
		writeFileHeader(filledNum);
		std::ofstream fileWriter(path, std::ios::out | std::ios::app);
		for (int i = 0; i < dim * dim; i++) {
			if (filled[i] == 1) {
				fileWriter << i + 1 << " 0\n";
			}
			else if (filled[i] == -1) {
				fileWriter << -(i + 1) << " 0\n";
			}
		}
		fileWriter.close();
		sudoku->generateConstraint(path, dim);
		
		// load data
		clauseNode* head = loader->cnfParse(path);
		solver->variableNum = head->wordNum;
		word* word_ = (word*)malloc(sizeof(word) * head->wordNum);
		solver->init(head, word_);
		// solve and timeit
		clock_t start = clock();
		status status_ = solver->DPLL(head, word_);
		clock_t end = clock();
		delete sudoku;
		delete solver;
		delete loader;
		if (status_ == success) return true;
		else return false;
	}
}