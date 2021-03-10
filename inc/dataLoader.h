#pragma once

#include<fstream>
#include<iostream>
#include<string>
#include<vector>

#define SHOW_LOAD_FILE 0

namespace dzh {
	/** ds to save cnf file
		head
		 |
		clause - word - word - ...
		 |
		clause - word - word - ...
		 |
		 ...
	*/

	static const int dim = 6;

	typedef struct wordNode
	{
		int num = 0;
		wordNode* next = nullptr;
	}wordNode;

	typedef struct clauseNode
	{
		int wordNum = 0;	// 头结点是clauseNum
		clauseNode* down = nullptr;
		wordNode* right = nullptr;
	}clauseNode;

	typedef struct word {
		int truthValue;		// 1 if true, 0 if false
		int frequency;		// positive + negative
		int frequency_bak;	// back up for frequency
		int polarity;		// positive - negative
		int decesionLevel;
		clauseNode* antecedent;	// antecedent clause

	} word;

	class cnfData {
	public:
		cnfData() = default;
		~cnfData() = default;
		clauseNode* cnfParse(std::string path);
		static void displayDS(clauseNode* head);
		static clauseNode* distroyClause(clauseNode*& head, clauseNode* clause);
		static bool hasMember(clauseNode* clause, int num);
		static void addSingleClause(clauseNode*& head, int num);
		static wordNode* distroyWord(clauseNode*& clause, int num);
		static word* clone(word* word_, int variableNum);
		static clauseNode* clone(clauseNode* head);
		void saveResultToFile(std::string path, int variableNum, int flag, word* word_, float time);

	private:
		clauseNode* createClause(std::ifstream& fileReader);
		bool isSingleClause(clauseNode* clause);
		static clauseNode* findFatherClause(clauseNode* head, clauseNode* clause);
		static void freeClause(wordNode* clause);
	};
}