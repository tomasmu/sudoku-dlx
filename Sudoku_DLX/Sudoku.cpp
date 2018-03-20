#include "stdafx.h"
#include "Sudoku.h"

Sudoku::Sudoku(const char *sudoku) {
	_cellCount = _grid.size();
	_dimension = (int)sqrt(_grid.size());
	//parse sudoku
}

void Sudoku::Solve(int maxSolutions = 1) {
}

void Sudoku::Print() {
}

sudokuCell Sudoku::at(const int index) {
	return _grid[index / _dimension][index % _dimension];
}

sudokuCell Sudoku::at(const int row, const int col) {
	return _grid[row][col];
}

int Sudoku::rowLength() {
	return _grid.size();
}

int Sudoku::colLength() {
	return _grid[0].size();
}

int Sudoku::boxLength() {
	return (int)(sqrt(rowLength()) + 0.5);
}
int Sudoku::cellCount() {
	return rowLength() * colLength();
 }