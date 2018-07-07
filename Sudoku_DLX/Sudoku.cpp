#include "stdafx.h"

#include <iostream>
#include <vector>
#include <set>
#include "Sudoku.h"

Sudoku::Sudoku(const char *sudoku) {
	_original = sudoku;

	_boxLength = (int)sqrt(sqrt(strlen(sudoku)));
	_rowLength = _boxLength * _boxLength;
	_colLength = _rowLength;

	if (_rowLength * _rowLength != strlen(sudoku)) {
		std::cout << "WARNING! Input string is " << strlen(sudoku) << " characters long, and not on the form n^4" << std::endl;
	}

	_grid = sudokuGrid(_rowLength, sudokuRow(_colLength, BLANK_CELL_VALUE));
	_isZeroBased = isZeroBased(sudoku);

	for (int row = 0; row < _rowLength; row++) {
		for (int col = 0; col < _colLength; col++) {
			int strIndex = row * _rowLength + col;
			//char charValue = sudoku[strIndex];
			//sudokuCell cellValue = charToCell(charValue);
			_grid[row][col] = charToCell(sudoku[strIndex]);
		}
	}
}

Sudoku::Sudoku(sudokuGrid grid) {
	_grid = grid;
}

sudokuGrid Sudoku::getSudoku(solution sol) {
	const unsigned int cellCount = sol.size();
	const int dimension = (int)(sqrt(cellCount));
	const int base = dimension;
	sudokuGrid sudoku(dimension, sudokuRow(dimension, BLANK_CELL_VALUE));

	for (unsigned int i = 0; i < cellCount; i++) {
		const int temp = sol[i]->rowId;

		const int row = temp / (base * base);
		const int col = temp / base % base;
		const int num = temp % base;

		sudoku[row][col] = num;
	}

	return sudoku;
}

void Sudoku::solve(const int maxSolutions) {
	//get all solutions
	ToroidalLinkedList list(_grid);
	list.solve(maxSolutions);

	//convert solutions and save to list
	_solutions.clear();
	for (unsigned int i = 0; i < list._solutions.size(); i++) {
		sudokuGrid sudoku = getSudoku(list._solutions[i]);
		_solutions.push_back(sudoku);
	}
}

void Sudoku::printGrid(sudokuGrid sudoku) {
	const int gridWidth = 2 * _boxLength * (_boxLength + 1) + 1;
	std::string gridLine(gridWidth, '-');

	for (int row = 0; row < _rowLength; row++) {
		if (row % _boxLength == 0)
			std::cout << gridLine.c_str() << std::endl;
	
		for (int col = 0; col < _colLength; col++) {
			if (col % _boxLength == 0)
				std::cout << "| ";
	
			//int cellValue = sudoku[row][col];
			//char charValue = cellToChar(cellValue);
			std::cout << cellToChar(sudoku[row][col]) << " ";
		}
	
		std::cout << "|" << std::endl;
	}
	
	std::cout << gridLine.c_str() << std::endl;
}

void Sudoku::printSolution(const unsigned int count) {
	std::cout << "solutions found: " << _solutions.size() << std::endl;
	if (_solutions.size() <= 0)
		return;

	for (unsigned int i = 0; i < _solutions.size() && (i < count || count == 0); i++) {
		std::cout << "solution[" << i << "]" << std::endl;
		printGrid(_solutions[i]);
	}
}

void Sudoku::printOriginal() {
	std::cout << "original sudoku: " << std::endl;
	printGrid(_grid);
}

std::string Sudoku::getOriginal() {
	std::string result = "";
	for (int row = 0; row < _rowLength; row++) {
		for (int col = 0; col < _colLength; col++) {
			//char charValue = cellToChar(_grid[row][col]);
			result += cellToChar(_grid[row][col]);
		}
	}

	return result;
}

std::string Sudoku::getSolution(unsigned int index) {
	std::string result = "";
	if (_solutions.size() == 0)
		return "no solutions";

	if (index >= _solutions.size()) {
		index = _solutions.size();
	}

	for (int row = 0; row < _rowLength; row++) {
		for (int col = 0; col < _colLength; col++) {
			//char charValue = cellToChar(_solutions[index][row][col]);
			result += cellToChar(_solutions[index][row][col]);
		}
	}

	return result;
}

sudokuGrid Sudoku::getFirstSolution() {
	return _solutions[0];
}

//sudokuCell Sudoku::at(const int index) {
//	return _grid[index / _rowLength][index % _rowLength];
//}

sudokuCell Sudoku::at(const int row, const int col) {
	return _grid[row][col];
}

char Sudoku::cellToChar(sudokuCell digit) {
	if (digit == BLANK_CELL_VALUE)
		return BLANK_CELL_CHAR;

	if (_isZeroBased == false)
		digit++;

	if (digit >= 0 && digit <= 9)
		return digit + '0';
	else if (digit >= 10 && digit <= 'Z' - 'A' + 10)
		return digit - 10 + 'A';

	return '?';
}

sudokuCell Sudoku::charToCell(char digit) {
	if (digit == BLANK_CELL_CHAR)
		return BLANK_CELL_VALUE;

	int offset = _isZeroBased == false ? -1 : 0;

	if (digit >= '0' && digit <= '9')
		return digit - '0' + offset;
	else if (digit >= 'A' && digit <= 'Z')
		return digit + 10 - 'A' + offset;

	return '?';
}

bool Sudoku::isZeroBased(const char* sudoku) {
	const char* firstZeroPointer = strpbrk(sudoku, "0");
	return firstZeroPointer != NULL;
}

bool Sudoku::isZeroBased(sudokuGrid sudoku) {
	for (unsigned int row = 0; row < sudoku.size(); row++)
		for (unsigned int col = 0; col < sudoku.size(); col++)
			if (sudoku[row][col] == 0)
				return true;

	return false;
}

bool Sudoku::isSolved() {
	//solutions.size = {0, 1, >=2} = {unsolved, unique, ambiguous}
	return _solutions.size() == 1;
	//std::set<sudokuCell> rowDigits, colDigits, boxDigits;
	//for (int row = 0; row < _rowLength; row++)
	//{
	//	rowDigits.clear();
	//	colDigits.clear();
	//	boxDigits.clear();
	//	for (int col = 0; col < _colLength; col++) {
	//		int box = (row / _rowLength) * _rowLength + (row % _boxLength);
	//		boxDigits.insert(sudoku[box][col]);
	//		rowDigits.insert(sudoku[row][col]);
	//		colDigits.insert(sudoku[col][row]);
	//	}
	//	
	//	if (rowDigits.size() != _rowLength)
	//		return false;
	//	if (colDigits.size() != _colLength)
	//		return false;
	//	if (boxDigits.size() != _rowLength)
	//		return false;
	//}

	//return true;
}

int Sudoku::rowLength() {
	return _rowLength;
}

int Sudoku::colLength() {
	return _colLength;
}

int Sudoku::boxLength() {
	return _boxLength;
}

int Sudoku::cellCount() {
	return _cellCount;
}
