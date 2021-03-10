#pragma once

#include<string>
#include<fstream>
#include<random>

#include"dataLoader.h"
#include"solve.h"

#define ROW_CASE 1
#define COL_CASE 0

namespace dzh {
	// col: 1 - dim
	// row: 1 - dim

	/// <summary>
	/// 整体思想：
	/// 1. 约束一：遍历
	/// 2. 约束二：生成组合
	/// 3. 约束三：Tseytin变换
	/// 4. 已确定的空作为单子句填入
	/// </summary>
	class Sudoku {
	public:
		/// <summary>
		/// 生成已有的空的约束
		/// </summary>
		/// <param name="path"></param>
		/// <param name="dim"></param>
		static void generateConstraint(std::string path, int dim);
		static void randomGuess(int dim, int* filled, int& size);
		static void combination(int n, int m, int& cnt);
		static int isSameResult(word* solution1, word* solution2, int variableNum);
	private:
		static void combination(int* src, int* dst, std::ofstream& fileWriter, int n, int m, int size);
		static void constraint3(int* src, int* dst, std::ofstream& fileWriter, int n, int m, int dim, int& start);
		static void Tseytin(std::ofstream& fileWriter, int dim, int i, int j, int start, int flag);
	};
}