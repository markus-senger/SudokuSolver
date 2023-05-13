#include "SudokuSolver.h"
#include "Utility.h"

#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std::chrono_literals;

void TestOneSudoku(fs::path const& path) {
	SudokuSolver solver = SudokuSolver();

	std::cout << "--- " << path << " -----------------------------\n";
	std::cout << (solver.Solve(path, true) ? "\nSolved = True" : "\nSolved = False") << "\n\n";

	solver.Print();

	if (!solver.CheckSolvedSudoku())
		std::cout << "ERROR: Sudoku not correct!\n\n";
	else
		std::cout << "Sudoku correct!\n\n";
}

void TestAllSudokus(fs::path const& path) {
	fs::create_directories("./Results");
	for (auto const& entry : fs::directory_iterator{ path }) {
		if (entry.is_directory()) TestAllSudokus(path / entry);
		else {
			SudokuSolver solver = SudokuSolver();
			std::cout << "--- " << entry << " -----------------------------\n";
			std::cout << (solver.Solve(entry, false) ? "\nSolved = True" : "\nSolved = False") << "\n\n";
			solver.SaveInFile("./Results/Result_" + entry.path().filename().string());
		
			if (!solver.CheckSolvedSudoku())
				std::cout << "ERROR: Sudoku not correct!\n\n";
			else
				std::cout << "Sudoku correct!\n\n";
		}
	}
}

int main() {
	TestOneSudoku("..\\Examples\\25x25_medium.txt");
	TestAllSudokus("..\\Examples");
}