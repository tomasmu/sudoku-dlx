#pragma once

#include <vector>
#include "ToroidalLinkedList.h"

typedef int sudokuCell;
typedef std::vector<sudokuCell> sudokuRow;
typedef std::vector<sudokuRow> sudokuGrid;
typedef std::vector<sudokuGrid> sudokuList;

const int BLANK_CELL_VALUE = -1;
const char BLANK_CELL_CHAR = '.';

class Sudoku {
public:
	Sudoku(const char *sudoku);
	void solve(const int maxSolutions = 1);
	bool isSolved();
	void printSolution(const unsigned int count = 1);
	void printOriginal();
	int rowLength();
	int colLength();
	int boxLength();
	int cellCount();
	//sudokuCell at(const int index);
	sudokuCell at(const int row, const int col);
	std::string getOriginal();
	std::string getSolution(unsigned int index = 0);

private:
	sudokuGrid _grid;
	sudokuList _solutions;
	//todo: add ConstraintMatrix here
	const char* _original;
	
	int _rowLength;
	int _colLength;
	int _boxLength;
	int _cellCount;

	bool _isZeroBased;

	bool isZeroBased(const char* sudoku);
	bool isZeroBased(sudokuGrid sudoku);
	char cellToChar(sudokuCell digit);
	sudokuCell charToCell(char digit);
	sudokuGrid getSudoku(solution sol);
	void printGrid(sudokuGrid suodoku);
};
