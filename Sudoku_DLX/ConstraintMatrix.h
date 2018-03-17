#pragma once
#include "Sudoku.h"

typedef bool matrixCell;
typedef std::vector<matrixCell> matrixRow;
typedef std::vector<matrixRow> matrixGrid;

const matrixCell HAS_VALUE = true;	//remove later
const matrixCell NO_VALUE = false;	//remove later

class ConstraintMatrix {
public:
	ConstraintMatrix(sudokuGrid sudoku);
	matrixCell at(int row, int col);
	int rows();
	int cols();

private:
	matrixGrid _matrix;

	//void initMatrix(Sudoku sudoku) {
	void initMatrix(sudokuGrid sudoku);

	int _rows;
	int _cols;

	const matrixCell HAS_VALUE = true;
	const matrixCell NO_VALUE = false;
};
