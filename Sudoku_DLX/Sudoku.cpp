#include "stdafx.h"
#include <vector>

typedef std::vector<int> SudokuGrid;

class Sudoku {
public:
	Sudoku(const char *sudoku) {
		_count = _grid.size();
		_size = (int)sqrt(_grid.size());
	}
	
	void Solve(int maxSolutions = 1);

	void Print();

	int operator[] (const int index) {
		return _grid[index];
	}

	int at(const int index) {
		return _grid[index];
	}

	int at(const int row, const int col) {
		return _grid[row * _size + col];
	}

private:
	SudokuGrid _grid;
	std::vector<SudokuGrid> solutions;

	int _count;
	int _size;

	bool _isZeroBased;
};
