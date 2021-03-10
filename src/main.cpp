#include"main.h"

using namespace dzh;

#if GAME
int main() {
	/*
	1. 生成唯一解数独
	2. 增加已经填过空的位置的约束
	3. 填入新的空 -> 2
	*/
	Game* game = new Game();
	// TODO 增加启动界面
	int* filled = game->initGame();
	UI* ui = new UI();
	int filledNum = dim * dim / 3;
	ui->initGraph(filled, filledNum);
	MOUSEMSG mouse;
	while (1) {
		mouse = GetMouseMsg();
		if (ui->toSolve(mouse)) {
			SAT* solver = new SAT();
			word* word_ = solver->solve("../a.cnf", solver);
			int x, y, tl_x, tl_y, br_x, br_y;
			for (int i = 0; i < dim * dim; i++) {
				ui->getPointsInGraph(i + 1, x, y);
				ui->getRectPoints(x, y, tl_x, tl_y, br_x, br_y);
				if (word_[i].truthValue == 1) {
					setfillcolor(WHITE);
					fillrectangle(tl_x, tl_y, br_x, br_y);
				}
				else if (word_[i].truthValue == 0) {
					setfillcolor(BLACK);
					fillrectangle(tl_x, tl_y, br_x, br_y);
				}
			}
			break;
		}
		ui->addRect(mouse, filled);
	}
	getchar();
	closegraph();
	return 0;
}
#endif
#if SAT_PROB
int main() {
	cnfData* loader = new cnfData();
	SAT* solver = new SAT();
#if SAT_CASE
	// std::string path = "../satisfied_case/S/7cnf20_90000_90000_7.shuffled-20.cnf";
	// std::string path = "../satisfied_case/M/ec-mod2c-rand3bip-sat-250-2.shuffled-as.sat05-2534.cnf";
	std::string path = "../satisfied_case/L/e-par32-3.shuffled-3176.cnf";
	clauseNode* head = loader->cnfParse(path);
#endif
#if UNSAT_CASE
	std::string path = "../unsatisfied_case/u-problem7-50.cnf";
	clauseNode* head = loader->cnfParse(path);
#endif
#if TEST_CASE
	std::string path = "../test/9（unsatisfied）.cnf";
	clauseNode* head = loader->cnfParse(path);
#endif
#if GEN_CASE
	// std::string path = "../功能测试/unsat-5cnf-30.cnf";
	std::string path = "../性能测试/ais10.cnf";
	clauseNode* head = loader->cnfParse(path);
#endif

	clauseNode* cpy = cnfData::clone(head);
	//change word to literal
	solver->variableNum = head->wordNum;
	word* word_ = (word*)malloc(sizeof(word) * head->wordNum);
	solver->init(head, word_);
#if USE_CDCL
	clock_t start = clock();
	status status_ = solver->CDCL(head, word_);
	clock_t end = clock();
#endif
#if USE_DPLL
	clock_t start = clock();
	status status_ = solver->DPLL(head, word_);
	clock_t end = clock();
#endif
	if (status_ == success) {
		std::string outPath = path.substr(0, path.length() - 3) + "res";
		loader->saveResultToFile(outPath, solver->variableNum, 1, word_, (float)(end - start) * 1000 / CLOCKS_PER_SEC);
		bool isCorrect = SAT::verifyResult(head, word_);
		/*solver->displayResult(word_, solver->variableNum);*/
		std::cout << "final status: SAT\n" <<
			"verify result: " << isCorrect << std::endl <<
			"cost time: " << (float)(end - start) * 1000 / CLOCKS_PER_SEC << " ms." << std::endl;
	}
	else if (status_ == normal) {
		std::string outPath = path.substr(0, path.length() - 3) + "res";
		loader->saveResultToFile(outPath, solver->variableNum, 0, word_, (float)(end - start) * 1000 / CLOCKS_PER_SEC);
		std::cout << "final status: UNSAT" << std::endl <<
			"cost time: " << (float)(end - start) * 1000 / CLOCKS_PER_SEC << " ms." << std::endl;
	}
	free(cpy);
	delete solver;
	delete loader;
	return 0;
}
#endif