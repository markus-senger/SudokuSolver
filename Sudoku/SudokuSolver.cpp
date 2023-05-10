#include "SudokuSolver.h"

#include <iomanip>

// --- private -------------------------------------------------------------

constexpr double PROGRESSBAR_SIZE = 40;

// helper methods
void SudokuSolver::Init() {
	emptyCells.clear();

	rows.assign(size, GetStartBits());
	cols.assign(size, GetStartBits());
	boxes.assign(size, GetStartBits());

	initNumOfEmptyCells = GetNumOfEmptyCells();

	FindEmptyCellsAndPossibleValuesforTheCell();
}

void SudokuSolver::FindEmptyCellsAndPossibleValuesforTheCell() {
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			int box = int(row / sqrt(size)) * sqrt(size) + int(col / sqrt(size));
			if (board[row * size + col] == 0) {
				emptyCells.emplace_back(std::make_tuple(row, col, box));
			}
			else {
				unsigned int value = 1u << (unsigned int)(board[row * size + col] - 1);
				rows[row] ^= value;
				cols[col] ^= value;
				boxes[box] ^= value;
			}
		}
	}
}

unsigned int SudokuSolver::GetStartBits() {
	if (size == 4) return 0xf;
	if (size == 9) return 0x1ff;
	if (size == 16) return 0xffff;
	if (size == 25) return 0x1ffffff;
}

int SudokuSolver::ConvertTo1D(int const row, int const col) {
	return row * size + col;
}

unsigned int SudokuSolver::GetPositionOfLowestBit(unsigned int binary) {
	unsigned int pos = 0;
	while ((binary & 1) == 0) {
		binary >>= 1;
		pos++;
	}
	return pos;
}

unsigned int SudokuSolver::GetTheLowestBit(unsigned int binary) {
	if (binary == 0)
		return 0;

	unsigned int pos = GetPositionOfLowestBit(binary);

	return (1 << pos);
}

int SudokuSolver::CountNumOfBitsSet(unsigned int binary) {
	int cnt = 0;
	while (binary != 0) {
		if (binary & 1)
			cnt++;
		binary >>= 1;
	}
	return cnt;
}

int SudokuSolver::CountNumOfBitsSet(const std::tuple<int, int, int>& row_col_box) {
	auto const& [row, col, box] { row_col_box };
	auto candidates = rows[row] & cols[col] & boxes[box];
	return CountNumOfBitsSet(candidates);
}

void SudokuSolver::MoveCellWithLowestPossibilitiesToFront(int idx) {
	auto first = emptyCells.begin() + idx;
	auto best = first;
	int bestCnt = CountNumOfBitsSet(*best);
	for (auto entry = first + 1; entry < emptyCells.end(); entry++) {
		int entryCnt = CountNumOfBitsSet(*entry);
		if (entryCnt < bestCnt) {
			bestCnt = entryCnt;
			best = entry;
			if (bestCnt == 1)
				break;
		}
	}
	swap(*first, *best);
}


// file handling and parsing methods
std::ifstream SudokuSolver::GetOpenFile(fs::path const& path) {
	std::ifstream file;
	file.open(path);
	return file;
}

bool SudokuSolver::ReadFromFile(fs::path const& path) {
	std::ifstream file = GetOpenFile(path);
	if (!file.is_open()) {
		std::cout << "File could not be opened\n";
		return false;
	}

	try {
		Parse(file);
		return true;
	}
	catch (const std::exception& e) {
		std::cout << "Error while parsing: " << e.what() << '\n';
	}
		
	file.close();
	return false;
}

void SudokuSolver::Parse(std::ifstream& file) {
	file >> size;

	board.resize(size * size);

	for (auto& entry : board) {
		file >> entry;
	}
}


// recursive solver
bool SudokuSolver::SolveRecursive(int idx) {
	MoveCellWithLowestPossibilitiesToFront(idx);

	auto const& [row, col, box] { emptyCells[idx] };
	auto possibleValues = rows[row] & cols[col] & boxes[box];

	while (possibleValues) {
		unsigned int value = GetTheLowestBit(possibleValues);

		// remove bit
		rows[row] ^= value;
		cols[col] ^= value;
		boxes[box] ^= value;

		if (idx == emptyCells.size() - 1 || SolveRecursive(idx + 1)) {
			board[row * size + col] = GetPositionOfLowestBit(value) + 1;
			return true;
		}

		// reset
		rows[row] |= value;
		cols[col] |= value;
		boxes[box] |= value;

		// remove bit from possible values
		possibleValues = possibleValues & (possibleValues - 1);
	}
	return false;
}


// sudoku progress methods
void SudokuSolver::ShowProgressBar()
{
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		int numOfEmptyCells = GetNumOfEmptyCells();
		double percentage = double(initNumOfEmptyCells - numOfEmptyCells) / initNumOfEmptyCells;
		std::cout << "Loading [";

		int pos = PROGRESSBAR_SIZE * percentage;
		for (int i = 0; i < PROGRESSBAR_SIZE; ++i)
		{
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] [" << initNumOfEmptyCells - numOfEmptyCells << " / " << initNumOfEmptyCells << "] -> " << int(percentage * 100.0) << " %    \r";
		std::cout.flush();
	} while (GetNumOfEmptyCells() != 0);
}

int SudokuSolver::GetNumOfEmptyCells() {
	int cnt = 0;
	for (auto entry : board) {
		if (entry == 0)
			cnt++;
	}
	return cnt;
}


// check sudoku methods
std::tuple<int, int> SudokuSolver::GetSquareStartPos(swe::Position pos) {
	return std::make_tuple(pos.row - (pos.row % static_cast<int>(sqrt(size))), pos.col - (pos.col % static_cast<int>(sqrt(size))));
}

bool SudokuSolver::IsValidRowAndCol(int num, swe::Position pos) {
	for (int i = 0; i < size; i++) {
		if (board[ConvertTo1D(i, pos.col)] == num || board[ConvertTo1D(pos.row, i)] == num)
			return false;
	}
	return true;
}

bool SudokuSolver::IsValidSquares(int num, swe::Position pos) {
	auto const& [squareStartRowPos, squareStartColPos] { GetSquareStartPos(pos) };

	for (int row = squareStartRowPos; row < squareStartRowPos + static_cast<int>(sqrt(size)); row++) {
		for (int col = squareStartColPos; col < squareStartColPos + static_cast<int>(sqrt(size)); col++) {
			if (board[ConvertTo1D(row, col)] == num)
				return false;
		}
	}
	return true;
}


// --- public -------------------------------------------------------------

// check sudoku methods
bool SudokuSolver::IsValid(int num, swe::Position pos) {
	return IsValidRowAndCol(num, pos) && IsValidSquares(num, pos);
}


// prepre for solving
bool SudokuSolver::Solve(fs::path const& path, bool showProgressBar) {
	if (ReadFromFile(path)) {

		std::thread progressBarThread;
		if(showProgressBar)
			progressBarThread = std::thread([this] { ShowProgressBar(); });
		
		Init();

		auto duration = swe::timed_run([this] {
			result = SolveRecursive(0);
		});

		if (showProgressBar)
			progressBarThread.join();
		
		std::cout << "\nneeded time: " << swe::to<std::milli>(duration) << " ms\n";
		
		return result;
	}
	return false;
}


// output
void SudokuSolver::SaveInFile(fs::path const& path) {
	std::ofstream file{ path };
	Print(file);
}

void SudokuSolver::Print(std::ostream& out) {
	if (board.empty() || board.size() != size * size)
		out << "print not valid\n";
	else {
		for (int row = 0; row < size; row++) {
			for (int col = 0; col < size; col++) {
				out << std::setw(2) << board[ConvertTo1D(row, col)] << ' ';
				if ((col + 1) % static_cast<int>(sqrt(size)) == 0) 
					out << "  ";
			}
			out << '\n';
			if ((row + 1) % static_cast<int>(sqrt(size)) == 0) out << '\n';
		}
	}
}