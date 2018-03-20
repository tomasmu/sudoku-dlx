#pragma once
#include <vector>

typedef int sudokuCell;
typedef std::vector<sudokuCell> sudokuRow;
typedef std::vector<sudokuRow> sudokuGrid;	//1D vector instead?
typedef std::vector<sudokuGrid> sudokuList;

const int BLANK_CELL_VALUE = -1;	//remove later
const char BLANK_CELL_CHAR = '.';	//remove later
const int MAX_SOLUTIONS = 3000;		//remove later

class Sudoku {
public:
	Sudoku(const char *sudoku);
	void Solve(const int maxSolutions);
	void Print();
	int rowLength();
	int colLength();
	int boxLength();
	int cellCount();
	sudokuCell at(const int index);
	sudokuCell at(const int row, const int col);

private:
	sudokuGrid _grid;
	sudokuList _solutions;
	
	int _cellCount;
	int _dimension;
	
	bool _isZeroBased;
	
	const int BLANK_CELL_VALUE = -1;
	const char BLANK_CELL_CHAR = '.';
};
