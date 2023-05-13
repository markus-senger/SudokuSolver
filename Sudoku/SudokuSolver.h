#pragma once

#include "Utility.h"

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <tuple>
#include <filesystem>
#include <bitset>
#include <iostream>

namespace fs = std::filesystem;

class SudokuSolver {

private:

	int size;
	bool result;
	std::vector<int> board;
	std::vector<std::tuple<int, int, int>> emptyCells;
	std::vector<unsigned int> rows, cols, boxes;
	int initNumOfEmptyCells;
	bool fin;

	// helper methods
	void Init();
	void FindEmptyCellsAndPossibleValuesforTheCell();
	unsigned int GetStartBits();
	int ConvertTo1D(int const row, int const col);
	unsigned int GetPositionOfLowestBit(unsigned int binary);
	unsigned int GetTheLowestBit(unsigned int binary);
	int CountNumOfBitsSet(unsigned int binary);
	int CountNumOfBitsSet(const std::tuple<int, int, int>& row_col_box);
	void MoveCellWithLowestPossibilitiesToFront(int idx);

	// file handling and parsing methods
	std::ifstream GetOpenFile(fs::path const& path);
	bool ReadFromFile(fs::path const& path);
	void Parse(std::ifstream& file);

	// recursive solver
	bool SolveRecursive(int idx);

	// sudoku progress methods
	void ShowProgressBar();
	int GetNumOfEmptyCells();

	// check sudoku methods
	bool IsValid(int num, swe::Position pos);
	std::tuple<int, int> GetSquareStartPos(swe::Position pos);
	bool IsValidRowAndCol(int num, swe::Position pos);
	bool IsValidSquares(int num, swe::Position pos);
	

public:

	// check sudoku methods
	bool CheckSolvedSudoku();

	// prepre for solving
	bool Solve(fs::path const& path, bool showProgressBar = false);

	// output
	void SaveInFile(fs::path const& path = "./output.txt");
	void Print(std::ostream& out = std::cout);
};