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
}

void TestAllSudokus(fs::path const& path) {
	fs::create_directories("./Results");
	for (auto const& entry : fs::directory_iterator{ path }) {
		if (entry.is_directory()) TestAllSudokus(path / entry);
		else {
			SudokuSolver solver = SudokuSolver();
			std::cout << "--- " << entry << " -----------------------------\n";
			std::cout << (solver.Solve(entry, false) ? "\nSolved = True" : "\nSolved = False") << "\n\n";
			solver.SaveInFile("./Results/Result" + entry.path().filename().string() + ".txt");
		}
	}
}

int main() {
	TestOneSudoku("..\\Examples\\25x25_medium.txt");
	TestAllSudokus("..\\Examples");
}