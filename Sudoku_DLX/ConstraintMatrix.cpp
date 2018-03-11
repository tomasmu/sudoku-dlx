#include "stdafx.h"
#include "ConstraintMatrix.h"
#include "Sudoku.h"
#include <vector>

bool ConstraintMatrix::at(int row, int col) {
	return _matrix[row][col];
}

int ConstraintMatrix::rows() {
	return _matrix.size();
}

int ConstraintMatrix::cols() {
	return _matrix[0].size();
}

ConstraintMatrix::ConstraintMatrix(sudokuGrid sudoku) {
	//most of these variables are for pedagogic reasons
	//e.g.: rowLength, colLength, and digits will always be equal; but makes the code easier to understand
	const int rowLength = sudoku.size();
	const int colLength = sudoku[0].size();
	const int boxLength = (int)(sqrt(rowLength) + 0.5);
	const int gridSize = rowLength * colLength;
	const int digits = rowLength;

	const int sudokuConstraints = 4;
	const int cellOffset = 0 * gridSize;
	const int rowOffset = 1 * gridSize;
	const int colOffset = 2 * gridSize;
	const int boxOffset = 3 * gridSize;

	const int candidateCount = digits * gridSize;				//constraint rows
	const int constraintCount = sudokuConstraints * gridSize;	//constraint cols
	_matrix = std::vector<std::vector<bool>>(candidateCount, std::vector<bool>(constraintCount, NO_VALUE));

	//add constraints
	for (int row = 0; row < rowLength; row++) {
		for (int col = 0; col < colLength; col++) {
			for (int num = 0; num < digits; num++) {
				//add clues only if we have a blank, or when we have the correct clue
				if (sudoku[row][col] == BLANK_CELL_VALUE || sudoku[row][col] == num) {
					//all n^3 cell possibilities can be represented by row, col, num (RxCyNz, where x,y,z=1..n)
					//this can be converted to base b (number of digits), Rx*b^2 + Cy*b + Nz
					int candidateIndex = (row * digits * digits) + (col * digits) + num;

					//four different constraints, with one of them fixed we have n^2 possibilities
					//if we omit one term at a time we can map the values to n^2 unique values
					//RnCn[Nn]	cell (row-column possibilities), number fixed
					//Rn[Cn]Nn	row	(row-number possibilities), column fixed
					//[Rn]CnNn	column (column-number possibilities), row fixed
					//["Bn"]Nn	box	(box-number possibilities)
					const int box = (row / boxLength) * boxLength + (col / boxLength);	//box 0..(n-1)

					int cellIndex = cellOffset + row * digits + col;
					int rowIndex = rowOffset + row * digits + num;
					int colIndex = colOffset + col * digits + num;
					int boxIndex = boxOffset + box * digits + num;

					//four 1's (constraints) per row
					_matrix[candidateIndex][cellIndex] = HAS_VALUE;
					_matrix[candidateIndex][rowIndex] = HAS_VALUE;
					_matrix[candidateIndex][colIndex] = HAS_VALUE;
					_matrix[candidateIndex][boxIndex] = HAS_VALUE;
				}
			}
		}
	}
}
