#include "sudoku.h"

namespace dzh {
	/// <summary>
	/// 生成dim维的数独，写入到path路径文件中
	/// </summary>
	/// <param name="path"></param>
	/// <param name="dim"></param>
	/// <param name="filled"></param>
	/// <param name="size"></param>
	void Sudoku::generateConstraint(std::string path, int dim)
	{
		std::ofstream fileWriter(path, std::ios::out | std::ios::app);

		// constraint NO.1
		for (int i = 1; i <= dim; i++) {// dim
			for (int j = 1; j <= dim; j++) {
				if (j < dim - 1) {
					fileWriter << (i - 1) * dim + j << " " << (i - 1) * dim + j + 1 << " " << (i - 1) * dim + j + 2 << " " << 0 << "\n";
					fileWriter << -((i - 1) * dim + j) << " " << -((i - 1) * dim + j + 1) << " " << -((i - 1) * dim + j + 2) << " " << 0 << "\n";
				}
				if (i < dim - 1) {
					fileWriter << (i - 1) * dim + j << " " << i * dim + j << " " << (i + 1) * dim + j << " " << 0 << "\n";
					fileWriter << -((i - 1) * dim + j) << " " << -(i * dim + j) << " " << -((i + 1) * dim + j) << " " << 0 << "\n";
				}
			}
		}

		// constraint NO.2
		int tar = dim / 2 + 1;
		int* src = new int[dim];
		int* dst = new int[tar];
		for (int i = 1; i <= dim; i++) {		// 生成组合
			// row i
			for (int j = 1; j <= dim; j++) {
				src[j - 1] = (i - 1) * dim + j;
			}
			combination(src, dst, fileWriter, dim, tar, tar);
			for (int j = 1; j <= dim; j++) {
				src[j - 1] = -((i - 1) * dim + j);
			}
			combination(src, dst, fileWriter, dim, tar, tar);
			// col i
			for (int j = 1; j <= dim; j++) {
				src[j - 1] = (j - 1) * dim + i;
			}
			combination(src, dst, fileWriter, dim, tar, tar);
			for (int j = 1; j <= dim; j++) {
				src[j - 1] = -((j - 1) * dim + i);
			}
			combination(src, dst, fileWriter, dim, tar, tar);
		}

		// constraint NO.3
		int start = dim * dim + 1;
		for (int i = 0; i < dim; i++) {
			src[i] = i + 1;
		}
		constraint3(src, dst, fileWriter, dim, 2, dim, start);

		delete[] src;
		delete[] dst;
		fileWriter.close();
		return;
	}

	void Sudoku::randomGuess(int dim, int* filled, int& size) {
		srand((unsigned)time(NULL));
		int i = rand() % dim;	//row
		int j = rand() % dim;	//col
		while (filled[i * dim + j] != 0) {
			i = rand() % dim;	//row
			j = rand() % dim;	//col
		}
		filled[i * dim + j] = 1;
		size++;
		return;
	}

	/// <summary>
	/// 生成src的m组合，用dst储存中间状态，写入filewriter
	/// </summary>
	/// <param name="src"></param>
	/// <param name="dst"></param>
	/// <param name="fileWriter"></param>
	/// <param name="n"> src中元素的个数 </param>
	/// <param name="m"> dst中元素的个数 </param>
	void Sudoku::combination(int* src, int* dst, std::ofstream& fileWriter, int n, int m, int size) {
		for (int i = m; i <= n; i++) {
			dst[m - 1] = src[i - 1];
			if(m == 1){
				for (int j = 0; j < size; j++) {
					auto elem = dst[j];
					fileWriter << elem << " ";
				}
				fileWriter << "0\n";
			}
			else {
				combination(src, dst, fileWriter, i - 1, m - 1, size);
			}
		}
	}

	void Sudoku::combination(int n, int m, int& cnt) {
		for (int i = m; i <= n; i++) {
			if (m == 1) {
				cnt++;
			}
			else {
				combination(i - 1, m - 1, cnt);
			}
		}
	}

	void Sudoku::constraint3(int* src, int* dst, std::ofstream& fileWriter, int n, int m, int dim, int& start) {
		for (int i = m; i <= n; i++) {
			dst[m - 1] = src[i - 1];
			if (m == 1) {
				/*std::cout << dst[0] << " " << dst[1] << std::endl;*/
				Tseytin(fileWriter, dim, dst[0], dst[1], start, ROW_CASE);		// dst[0]行 和 dst[1]行 约束
				start += (3 * dim);
				Tseytin(fileWriter, dim, dst[0], dst[1], start, COL_CASE);		// dst[0]列 和 dst[1]列 约束
				start += (3 * dim);
			}
			else {
				constraint3(src, dst, fileWriter, i - 1, m - 1, dim, start);
			}
		}
	}

	/// <summary>
	/// 生成第i行/列与第j行/列不重复的范式
	/// </summary>
	/// <param name="fileWriter"></param>
	/// <param name="dim"></param>
	/// <param name="i"> 0 - dim-1 </param>
	/// <param name="j"> 0 - dim-1 </param>
	/// <param name="start"></param>
	/// <param name="flag"></param>
	void Sudoku::Tseytin(std::ofstream& fileWriter, int dim, int i, int j, int start, int flag) {
		int fijk_s = start;			// start是可以开始用的那个
		int fijkt_s = start + dim;
		int ik, jk, fijkt;
		// part1: fijkt
		for (int k = 1; k <= dim; k++) {
			if (flag == ROW_CASE) {		// col NO.k
				ik = (i - 1) * dim + k;
				jk = (j - 1) * dim + k;
			}
			else if(flag == COL_CASE){	// row NO.k
				ik = i + (k - 1) * dim;
				jk = j + (k - 1) * dim;
			}
			for (int t = 1; t >= 0; t--) {
				fijkt = fijkt_s + (k - 1) * 2 + (t + 1) % 2;
				fileWriter << ik << " " << -fijkt << " " << "0\n";
				fileWriter << jk << " " << -fijkt << " " << "0\n";
				fileWriter << -ik << " " << -jk << " " << fijkt << " " << "0\n";
			}
		}
		// part2: fijk
		int fijk0, fijk1, fijk;
		for (int k = 1; k <= dim; k++) {
			fijk0 = fijkt_s + (k - 1) * 2 + 1;
			fijk1 = fijkt_s + (k - 1) * 2;
			fijk = fijk_s + k - 1;
			fileWriter << -fijk0 << " " << fijk << " " << "0\n";
			fileWriter << -fijk1 << " " << fijk << " " << "0\n";
			fileWriter << fijk0 << " " << fijk1 << " " << -fijk << " " << "0\n";
		}
		// part3: fij
		for (int k = 0; k < dim; k++) {
			fijk = fijk_s + k;
			fileWriter << -fijk << " ";
		}
		fileWriter << "0\n";
		return;
	}

	int Sudoku::isSameResult(word* solution1, word* solution2, int variableNum) {
		for (int i = 0; i < variableNum; i++) {
			if (solution1[i].truthValue != solution2[i].truthValue) {
				return i;
			}
		}
		return -1;
	}
}