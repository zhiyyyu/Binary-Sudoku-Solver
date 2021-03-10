#include"dataLoader.h"

namespace dzh {
	clauseNode* cnfData::cnfParse(std::string path) {
		std::ifstream fileReader(path);
		if (!fileReader) {
			std::cout << "file open failed!" << std::endl;
			exit(0);
		}
		char firstChar;
		std::string line;
		fileReader >> firstChar;
		while (firstChar == 'c') {
			std::getline(fileReader, line);
			// std::cout << firstChar << " " << line << std::endl;
			fileReader >> firstChar;
		}
		clauseNode* ret = new clauseNode();
		clauseNode* head = ret;
		std::string format;
		int variableNum;
		int clauseNum;
		int var;
		if (firstChar == 'p') {
			fileReader >> format >> variableNum >> clauseNum;
			ret->wordNum = variableNum;
			for (int i = 0; i < clauseNum; i++) {
				head->down = createClause(fileReader);
				head = head->down;
			}
			head->down = nullptr;
		}
#if SHOW_LOAD_FILE
			//std::vector<int> data;
			//while (!fileReader.eof()) {
			//	int dataInt;
			//	fileReader >> dataInt;
			//	if (fileReader.peek() == EOF) break;
			//	if (!dataInt) {		// 写入wordNode
			//		data.push_back(dataInt); 
			//	}
			//	else{				// 写入clauseNode
			//		data.push_back(0);
			//	}
			//}
			//std::cout << "p " << format << " " << variableNum << " " << clauseNum << std::endl;
			//for (auto& num : data) {
			//	if (num == 0) {
			//		std::cout << "0" << std::endl;
			//	}
			//	else {
			//		std::cout << num << " ";
			//	}
			//}
#endif
		fileReader.close();
		return ret;
	}

	clauseNode* cnfData::createClause(std::ifstream& fileReader) {
		clauseNode* clause = new clauseNode();
		wordNode* front = new wordNode();
		int var = 0;
		fileReader >> var;
		front->num = var;
		clause->right = front;
		clause->wordNum++;
		fileReader >> var;
		while( var != 0 ) {
			wordNode* rear = new wordNode();
			rear->num = var;
			front->next = rear;
			front = rear;
			clause->wordNum++;
			fileReader >> var;
		}
		front->next = nullptr;
		return clause;
	}

	void cnfData::saveResultToFile(std::string path, int variableNum, int flag, word* word_, float time) {
		std::ofstream fileWriter(path);
		if (!fileWriter) {
			std::cout << "file open failed!" << std::endl;
			exit(0);
		}
		fileWriter << "s " << flag << std::endl << "v ";
		if (flag == 1) {
			for (int i = 0; i < variableNum; i++) {
				fileWriter << ((word_[i].truthValue == 1) ? i + 1 : -i - 1) << " ";
			}
		}
		fileWriter << std::endl << "t " << time << std::endl;
		fileWriter.close();
	}

	void cnfData::displayDS(clauseNode* head) {
		clauseNode* p = head->down;
		std::cout << std::endl;
		std::cout << "clause num: " << head->wordNum << std::endl;
		while (p != nullptr) {
			std::cout << "word num : " << p->wordNum << " ";
			wordNode* clause = p->right;
			if (clause == nullptr) std::cout << " ";
			while (clause != nullptr) {
				std::cout << clause->num << " ";
				clause = clause->next;
			}
			std::cout << std::endl;
			p = p->down;
		}
		std::cout << std::endl;
		return;
	}

	bool cnfData::isSingleClause(clauseNode* clause) {
		return clause->right->next == nullptr;
	}

	//TODO 记得释放空间
	clauseNode* cnfData::distroyClause(clauseNode*& head, clauseNode* clause) {
		clauseNode* father = findFatherClause(head, clause);
		if (!father) {
			return father;
		}
		head->wordNum--;
		father->down = clause->down;
		freeClause(clause->right);
		free(clause);
		return father;
	}

	bool cnfData::hasMember(clauseNode* clause, int num) {
		if (!clause) return false;
		wordNode* word = clause->right;
		while (word) {
			if (word->num == num) {
				return true;
			}
		}
		return false;
	}

	wordNode* cnfData::distroyWord(clauseNode*& clause, int num) {
		wordNode* p = clause->right;
		wordNode* q = nullptr;
		// 头文字的情况
		if (p->num == num) {
			clause->right = p->next;
			clause->wordNum--;
			free(p);
			return clause->right;
		}
		else {
			// 中间文字的情况
			while (p->next) {
				if (p->next->num == num) {
					q = p->next;
					p->next = q->next;
					free(q);
					clause->wordNum--;
					return p->next;
				}
				p = p->next;
			}
		}
		// 尾文字的情况
		if (p->num == num) {
			q->next = nullptr;
			free(p);
			clause->wordNum--;
		}
		return nullptr;
	}

	word* cnfData::clone(word* word_, int variableNum)
	{
		word* cpy = (word*)malloc(sizeof(word)*variableNum);
		if (!cpy) return nullptr;
		for (int i = 0; i < variableNum; i++) {
			cpy[i].frequency = word_[i].frequency;
			cpy[i].polarity = word_[i].polarity;
			cpy[i].truthValue = word_[i].truthValue;
		}
		return cpy;
	}

	clauseNode* cnfData::clone(clauseNode* head)
	{
		clauseNode* p = head;
		clauseNode* ret = (clauseNode*)malloc(sizeof(clauseNode));		// new head
		clauseNode* retp = ret;					// clause iterator
		if (!ret) return nullptr;
		ret->wordNum = p->wordNum;
		ret->right = nullptr;
		retp = ret;
		while (p->down) {
			retp->down = (clauseNode*)malloc(sizeof(clauseNode));
			if (!retp) return nullptr;
			retp = retp->down;
			p = p->down;
			if (!retp) return nullptr;
			retp->wordNum = p->wordNum;

			wordNode* q = p->right;
			wordNode* retq = (wordNode*)malloc(sizeof(wordNode));
			retp->right = retq;
			retq->num = q->num;
			while (q->next) {
				q = q->next;
				retq->next = (wordNode*)malloc(sizeof(wordNode));
				retq = retq->next;
				if (!retp) return nullptr;
				retq->num = q->num;
			}
			retq->next = nullptr;
		}
		retp->down = nullptr;
		return ret;
	}

	void cnfData::addSingleClause(clauseNode*& head, int num) {
		clauseNode* clause = new clauseNode;
		clause->wordNum = 1;
		clause->down = head;
		clause->right = new wordNode;
		clause->right->num = num;
		clause->right->next = nullptr;
		head = clause;
		return;
	}

	clauseNode* cnfData::findFatherClause(clauseNode* head, clauseNode* clause) {
		clauseNode* p = head->down;
		if (p == clause) return head;
		while (p) {
			if (p->down == clause) {
				return p;

			}
			p = p->down;
		}
		return nullptr;
	}

	void cnfData::freeClause(wordNode* clause) {
		if (clause == nullptr) return;
		if (clause->next) {
			freeClause(clause->next);
			free(clause);
		}
		return;
	}
}