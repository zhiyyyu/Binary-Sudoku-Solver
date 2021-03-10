#pragma once

#include"dataLoader.h"
#include<ctime>
#include<unordered_map>

//#define HAS_SAME_WORD 1
//#define HAS_OPPO_WORD 0

namespace dzh {

	typedef enum {
		satisfied,
		unsatisfied,
		success,
		normal,
	} status;

	class SAT {
	public:
		SAT() = default;
		~SAT() = default;
		void init(clauseNode* head, word*& word_);
		status DPLL(clauseNode* head, word*& word_);
		status CDCL(clauseNode* head, word*& word_);
		static void displayResult(word* word_, int variableNum);
		static bool verifyResult(clauseNode* head, word* word_);
		static word* solve(std::string path, SAT*& solver);

		int variableNum = 0;
		int clauseNum = 0;
		//int level;			// 决策深度
		int assignedNum;	// 已经赋值了的变元个数
		clauseNode* antecedent = nullptr;

	private:
		
		static int isSameResult(word* solution1, word* solution2, int variableNum);
		int clauseLearningAndBacktracking(clauseNode*& head, word* word_, int level);
		static status unitPropagation(clauseNode*& head, word* word_);
		status unitPropagation(clauseNode*& head, word* word_, int decisionLevel);
		static status applyAssignment(clauseNode* head, int num);
		void applyAssignment(word* word_, int num, int level, clauseNode* antecedent);
		void resetAssignment(word* word_, int num);
		int pickVariable(clauseNode* head, word* word_);
		static clauseNode* resolve(clauseNode* clause, word* word_, int resolver);
	};
}