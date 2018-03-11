#pragma once

#include "Sudoku.h"

const bool HAS_VALUE = 1;
const bool NO_VALUE = 0;

class ConstraintMatrix {
public:
	ConstraintMatrix(sudokuGrid sudoku);
	bool at(int row, int col);
	int rows();
	int cols();

private:
	std::vector<std::vector<bool>> _matrix;
};
