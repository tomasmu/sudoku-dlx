#pragma once

#include <vector>
#include "ToroidalLinkedList.h"

typedef int sudokuCell;
typedef std::vector<sudokuCell> sudokuRow;
typedef std::vector<sudokuRow> sudokuGrid;
typedef std::vector<sudokuGrid> sudokuList;

const int BLANK_CELL_VALUE = -1;	//remove later
const char BLANK_CELL_CHAR = '.';	//remove later

class Sudoku {
public:
	Sudoku(const char *sudoku);
	void solve(const int maxSolutions = 1);
	void print(const int count = 1);
	int rowLength();
	int colLength();
	int boxLength();
	int cellCount();
	sudokuCell at(const int index);
	sudokuCell at(const int row, const int col);
	bool isSolved();

private:
	sudokuGrid _grid;
	sudokuList _solutions;
	const char* _puzzle;
	
	int _rowLength;
	int _colLength;
	int _boxLength;
	int _cellCount;

	bool _isZeroBased;

	bool isZeroBased(const char* sudoku);
	bool isZeroBased(sudokuGrid sudoku);
	char cellToChar(sudokuCell digit);
	sudokuCell charToCell(char digit);
	std::string toString();
	sudokuGrid getSudoku(solution sol);
	void printGrid(sudokuGrid suodoku);

	const int BLANK_CELL_VALUE = -1;
	const char BLANK_CELL_CHAR = '.';
};
