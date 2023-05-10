#pragma once

#include "SudokuSolver.h"
#include <thread>

namespace swe {

	struct Position {
		int row;
		int col;

		Position(int row, int col) : row{ row }, col{ col } {}
	};

	template<typename SI>
	double to(std::chrono::duration<double, SI> const duration) {
		return duration.count();
	}

	template<typename Func>
	auto timed_run(Func const& p_func) {
		auto const start{ std::chrono::high_resolution_clock::now() };
		p_func();
		auto const stop{ std::chrono::high_resolution_clock::now() };

		return stop - start;
	}
};