#include"solve.h"

namespace dzh{
	void SAT::init(clauseNode* head, word*& word_) {
		for (int i = 0; i < variableNum; i++) {
			word_[i].truthValue = -1;
			word_[i].frequency = 0;
			word_[i].frequency_bak = 0;
			word_[i].polarity = 0;
			word_[i].decesionLevel = -1;
			word_[i].antecedent = nullptr;
		}
		clauseNode* p = head->down;
		while (p) {
			wordNode* q = p->right;
			while (q) {
				int var = abs(q->num) - 1;
				word_[var].frequency++;
				word_[var].frequency_bak++;
				word_[var].polarity += q->num > 0 ? 1 : -1;
				q = q->next;
			}
			p = p->down;
		}
		return;
	}
	word* SAT::solve(std::string path, SAT*& solver) {

		cnfData* loader = new cnfData();
		clauseNode* head = loader->cnfParse(path);
		solver->variableNum = head->wordNum;
		word* word_ = (word*)malloc(sizeof(word) * head->wordNum);
		solver->init(head, word_);
		clock_t start = clock();
		status status_ = solver->DPLL(head, word_);
		clock_t end = clock();
		delete loader;
		return word_;
	}

	/*
	1. 找到单位子句
	2. 删除所有含有单位子句文字的字句
	3. 删除所有含有单位子句负文字的文字
	4. 迭代1-3至没有单位子句
	5. 启发式搜索，赋值一个文字
	*/
	status SAT::DPLL(clauseNode* head, word*& word_) {
		status status_ = normal;
		status_ = unitPropagation(head, word_);		// 单元子句传播
		// std::cout << "up in dpll status:" << status_ << std::endl;
		if (status_ == satisfied) {
			return success;
		}
		else if (status_ == unsatisfied) {
			return normal;
		}
		//TODO 增加启发式
		int var = -1;
		bool isFind = false;
		clauseNode* p = head->down;
		while (p && !isFind) {
			wordNode* q = p->right;
			while (q && !isFind) {
				int num = abs(q->num) - 1;
				if (word_[num].frequency > 0) {	/*TODO 找到频率最高的文字 注意要-1*/
					var = num;
					isFind = true;
				}
				q = q->next;
			}
			p = p->down;
		}
		// std::cout << "choose var: " << var << std::endl;

		for (int i = 1; i >= 0; i--) {
			word* cpy = cnfData::clone(word_, variableNum);
			// 复制head
			clauseNode* headCpy = cnfData::clone(head);		//TODO
			if (cpy[var].polarity > 0) {
				cpy[var].truthValue = i;
			}
			else {
				cpy[var].truthValue = (i + 1) % 2;
			}
			cpy[var].frequency = -1;
			status_ = applyAssignment(headCpy, cpy[var].truthValue ? var+1 : -var-1);
			// std::cout << "aa in dpll status:" << status_ << std::endl;
			if (status_ == satisfied) {
				//displayResult(cpy, head->wordNum);
				return success;
			}
			else if (status_ == unsatisfied) {
				// TODO free cpy and headCpy
				free(cpy);
				while (headCpy->down) {
					cnfData::distroyClause(headCpy, headCpy->down);
				}
				free(headCpy);
				continue;
			}
			status_ = DPLL(headCpy, cpy);
			if (status_ == success) {
				word_ = cnfData::clone(cpy, variableNum);
				free(cpy);
				while (headCpy && headCpy->down) {
					cnfData::distroyClause(headCpy, headCpy->down);
				}
				free(headCpy);
				return success;
			}
			// TODO free cpy and headCpy
			free(cpy);
			while (headCpy && headCpy->down) {
				cnfData::distroyClause(headCpy, headCpy->down);
			}
			free(headCpy);

		}
		return normal;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="head"></param>
	/// <param name="word_"></param>
	/// <returns>
	///		satisfied	可满足
	///		unsatisfied	不满足，回溯
	///		normal		暂时没找到可满足解
	/// </returns>
	status SAT::unitPropagation(clauseNode*& head, word* word_) {
		bool hasUnit = false;
		clauseNode* p = nullptr;
		if (head->down == nullptr) {		// 空字句集
			return satisfied;
		}
		status status_;
		do {
			hasUnit = false;
			p = head->down;

			while (p && !hasUnit) {
				if (p->wordNum == 1) {		// 找到了单子句
					hasUnit = true;
					word_[abs(p->right->num) - 1].truthValue = p->right->num > 0 ? 1 : 0;
					word_[abs(p->right->num) - 1].frequency = -1;
					status_ = applyAssignment(head, p->right->num);
					// std::cout << "aa in up status:" << status_ << std::endl;
					if (status_ == satisfied || status_ == unsatisfied) {
						return status_;
					}
					break;
				}
				else if(p->wordNum == 0){	// 找到了空子句
					return unsatisfied;
				}
				p = p->down;
			}

		} while (hasUnit);
		return normal;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="head"></param>
	/// <param name="num"></param>
	/// <returns>
	///		satisfied	可满足
	///		unsatisfied	不满足，回溯
	///		normal		暂时没找到可满足解
	/// </returns>
	status SAT::applyAssignment(clauseNode* head, int num) {
		clauseNode* p = head;
		while (p) {
			wordNode* q = p->right;
			while (q) {
				if (q->num == num) {
					p = cnfData::distroyClause(head, p);
					// std::cout << "distroy clause successfully" << std::endl;
					if (head->down == nullptr) {
						return satisfied;
					}
					break;
				}
				else if (q->num == -num) {
					q = cnfData::distroyWord(p, -num);
					// std::cout << "distroy word successfully: " << -num << std::endl;
					if (p->right == nullptr) {
						return unsatisfied;
					}
					continue;
				}
				q = q->next;
			}
			p = p->down;
		}
		return normal;
	}

	bool SAT::verifyResult(clauseNode* head, word* word_) {
		bool isCorrect = true;
		clauseNode* p = head->down;
		while (p && isCorrect) {
			isCorrect = false;
			wordNode* q = p->right;
			while (q) {
				if (q->num > 0 && word_[q->num - 1].truthValue == 1 ||
					q->num < 0 && word_[-q->num - 1].truthValue == 0) {
					isCorrect = true; 
					break;
				}
				q = q->next;
			}
			if (isCorrect == false) {
				return false;
			}
			p = p->down;
		}
		return isCorrect;
	}

	void SAT::displayResult(word* word_, int variableNum)
	{
		for (int i = 0; i < variableNum; i++) {
			std::cout << "num: " << i <<
				"\ntruthValue: " << word_[i].truthValue <<
				//"\nfrequency: " << word_[i].frequency <<
				//"\npolarity: " << word_[i].polarity <<
				std::endl;
		}
	}

	int SAT::isSameResult(word* solution1, word* solution2, int variableNum) {
		for (int i = 0; i < variableNum; i++) {
			if (solution1[i].truthValue != solution2[i].truthValue) {
				return i;
			}
		}
		return -1;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="head"></param>
	/// <param name="word_"></param>
	/// <returns></returns>
	status SAT::CDCL(clauseNode* head, word*& word_) {
		int level = 0;
		// 看看有没有已经存在的冲突
		status status_ = unitPropagation(head, word_, level);
		if (status_ == unsatisfied) {
			return normal;
		}
		while (assignedNum != variableNum) {
			// 随机选一个文字猜，祖先为nullptr
			int var = pickVariable(head, word_);
			level++;
			applyAssignment(word_, var, level, nullptr);
			// 回溯
			while (true) {
				status_ = unitPropagation(head, word_, level);
				// 出现冲突
				if (status_ == unsatisfied) {
					// 冲突发生在第一层
					if (level == 0) {
						return normal;
					}
					// 找到最早发生冲突的层
					level = clauseLearningAndBacktracking(head, word_, level);
				}
				else {
					break;
				}
			}
		}
		return success;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="head"></param>
	/// <param name="word_"></param>
	/// <param name="decisionLevel"></param>
	/// <returns></returns>
	status SAT::unitPropagation(clauseNode*& head, word* word_, int decisionLevel) {
		bool isGet = false;
		int falseNum = 0;
		int unsetNum = 0;
		int lastUnset = 0;
		int var = -1;
		bool isSatisfied = false;
		do {
			isGet = false;
			clauseNode* p = head->down;
			while (p && !isGet) {
				falseNum = 0;
				unsetNum = 0;
				isSatisfied = false;
				wordNode* q = p->right;
				while (q) {
					var = abs(q->num) - 1;
					// 统计未赋值的文字个数
					if (word_[var].truthValue == -1) {
						unsetNum++;
						lastUnset = q->num;
					}
					// 赋值之后值为负
					else if (word_[var].truthValue == 0 && q->num > 0 ||
						word_[var].truthValue == 1 && q->num < 0) {
						falseNum++;
					}
					else {
						isSatisfied = true;
						break;
					}
					q = q->next;
				}
				if (isSatisfied) {
					p = p->down;
					continue;
				}
				// 只有一个未赋值，找到单子句，记录子句p
				if (unsetNum == 1) {
					applyAssignment(word_, lastUnset, decisionLevel, p);
					isGet = true;
					break;
				}
				// 所有文字都为假，即子句不成立
				else if (falseNum == p->wordNum) {
					antecedent = p;
					return unsatisfied;
				}
				p = p->down;
			}
		} while (isGet);
		antecedent = nullptr;
		return normal;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="word_"></param>
	/// <param name="num"> q->num </param>
	/// <param name="level"></param>
	/// <param name="antecedent"></param>
	void SAT::applyAssignment(word* word_, int num, int level, clauseNode* antecedent) {
		int var = abs(num) - 1;
		word_[var].truthValue = num > 0 ? 1 : 0;
		word_[var].decesionLevel = level;
		word_[var].antecedent = antecedent;
		word_[var].frequency = -1;
		assignedNum++;
		return;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="word_"></param>
	/// <param name="num"></param>
	void SAT::resetAssignment(word* word_, int num) {
		word_[num].truthValue = -1;
		word_[num].frequency = word_[num].frequency_bak;
		word_[num].decesionLevel = -1;
		word_[num].antecedent = nullptr;
		assignedNum--;
		return;
	}

	/// <summary>
	/// 先找到冲突字句，找到回溯层和祖先，然后reset掉回溯层后的所有层的文字。
	/// </summary>
	/// <param name="head"></param>
	/// <param name="word_"></param>
	/// <param name="level"></param>
	/// <returns></returns>
	int SAT::clauseLearningAndBacktracking(clauseNode*& head, word* word_, int level) {
		clauseNode* clauseLearned = antecedent;
		int conflictLevel = level;		// 发生冲突的层
		int cnt = 0;					// 在同一层冲突的文字
		int resolver = 0;
		while (true) {
			cnt = 0;
			// 遍历需要学习的字句，对于所有处于冲突层的文字计数
			wordNode* p = clauseLearned->right;
			while (p) {
				int var = abs(p->num) - 1;
				if (word_[var].decesionLevel == conflictLevel) {
					cnt++;
					// 同时记录祖先不为nullptr的文字
					if (word_[var].antecedent != nullptr) {
						resolver = var;
					}
				}
				p = p->next;
			}
			// 如果该层只有一个文字冲突？
			if (cnt == 1) {
				break;
			}
			// 求出学习到的子句
			clauseLearned = resolve(clauseLearned, word_, resolver);	//在后面free了
			if (!clauseLearned) {
				return -1;
			}
		}
		// 将学习到的子句加入到子句集中，并更新word_
		clauseNode* p = head->down;
		head->down = clauseLearned;
		clauseLearned->down = p;
		wordNode* q = clauseLearned->right;
		while (q) {
			int var = abs(q->num) - 1;
			word_[var].polarity += q->num > 0 ? 1 : -1;
			if (word_[var].frequency != -1) {
				word_[var].frequency++;
			}
			word_[var].frequency_bak++;
			q = q->next;
		}
		head->wordNum++;
		// 找到回溯层，也就是学习到的子句中层数最小的那个
		int backtrackedLevel = 0;
		q = clauseLearned->right;
		while (q) {
			int level = word_[abs(q->num) - 1].decesionLevel;
			if (level != conflictLevel && level > backtrackedLevel) {
				backtrackedLevel = level;
			}
			q = q->next;
		}
		// 还原回溯层后面的所有赋值
		p = head->down;
		for (int i = 0; i < variableNum; i++) {
			if (word_[i].decesionLevel > backtrackedLevel) {
				resetAssignment(word_, i);
			}
		}
		// free clauseLearned

		return backtrackedLevel;
	}

	/// <summary>
	/// 从 引发冲突的句子 和 BCP引发赋值的句子 中学习
	/// </summary>
	/// <param name="clause"> 引发冲突的子句 </param>
	/// <param name="word_"> </param>
	/// <param name="resolver"> 子句中最新赋值的文字，var格式 </param>
	/// <returns></returns>
	clauseNode* SAT::resolve(clauseNode* clause, word* word_, int resolver) {
		// 找到resolver的祖先所在的子句
		clauseNode* antecedent = word_[resolver].antecedent;
		clauseNode* ret = (clauseNode*)malloc(sizeof(clauseNode));		// TODO free ret
		if (!ret) return nullptr;
		ret->right = (wordNode*)malloc(sizeof(wordNode));
		ret->wordNum = 0;
		ret->down = nullptr;
		// 去除掉clause和antecedent中间的resolver
		wordNode* p = antecedent->right;
		wordNode* q = ret->right;
		if (!q) return nullptr;
		q->num = 0;
		if (q && p->num - 1 != resolver && -p->num-1 != resolver) {	
			q->num = p->num;
			ret->wordNum++;
		}
		while (p) {
			if (q && p->num - 1 != resolver && -p->num - 1 != resolver) {
				if (q->num != 0) {
					q->next = (wordNode*)malloc(sizeof(wordNode));
					q = q->next;
				}
				if (!q) return nullptr;
				q->num = p->num;
				ret->wordNum++;
			}
			p = p->next;
		}
		p = clause->right;
		while (p) {
			if (q && p->num - 1 != resolver && -p->num - 1 != resolver) {
				q->next = (wordNode*)malloc(sizeof(wordNode));
				q = q->next;
				if (!q) return nullptr;
				q->num = p->num;
				ret->wordNum++;
			}
			p = p->next;
		}
		if (!q) return nullptr;
		q->next = nullptr;
		// ret去重
		wordNode* prev = ret->right;
		std::unordered_map<int, int> umap;
		q = ret->right;
		while (q) {
			if (umap.find(q->num) == umap.end()) {
				umap[q->num] = 1;
			}
			else {
				ret->wordNum--;
				prev->next = q->next;
				wordNode* tmp = q;
				q = q->next;
				free(tmp);
				continue;
			}
			prev = q;
			q = q->next;
		}
		return ret;
	}

	/// <summary>
	/// 搜索下一个变元
	/// </summary>
	/// <param name="head">  </param>
	/// <param name="word_">  </param>
	/// <returns> q->num </returns>
	int SAT::pickVariable(clauseNode* head, word* word_) {
		clauseNode* p = head->down;
		while (p) {
			wordNode* q = p->right;
			while (q) {
				int num = abs(q->num) - 1;
				if (word_[num].frequency > 0) {	/*TODO 找到频率最高的文字 注意要-1*/
					return q->num;
				}
				q = q->next;
			}
			p = p->down;
		}
		return 0;
	}
}