#include "stdafx.h"
#include "ConstraintMatrix.h"

ConstraintMatrix::ConstraintMatrix(sudokuGrid sudoku) {
	//most of these variables are for pedagogic reasons
	//e.g.: rowLength, colLength, and digits will always be equal; but makes the code easier to understand
	const int rowLength = sudoku.size();
	const int colLength = sudoku[0].size();
	const int boxLength = (int)(sqrt(rowLength));
	const int cellCount = rowLength * colLength;
	const int digits = rowLength;

	const int sudokuConstraints = 4;
	const int cellOffset = 0 * cellCount;
	const int rowOffset = 1 * cellCount;
	const int colOffset = 2 * cellCount;
	const int boxOffset = 3 * cellCount;

	const int candidateRowCount = digits * cellCount;
	const int constraintColCount = sudokuConstraints * cellCount;
	_matrix = matrixGrid(candidateRowCount, matrixRow(constraintColCount, NO_VALUE));

	//add constraints
	for (int row = 0; row < rowLength; row++) {
		for (int col = 0; col < colLength; col++) {
			for (int num = 0; num < digits; num++) {
				//add clues only if we have a blank, or when we have the correct clue
				if (sudoku[row][col] == BLANK_CELL_VALUE || sudoku[row][col] == num) {
					//map each constraint to a unique value (offset + 0..cellCount-1)
					int cellIndex = cellOffset + row * digits + col;
					int rowIndex = rowOffset + row * digits + num;
					int colIndex = colOffset + col * digits + num;
					int box = (row / boxLength) * boxLength + (col / boxLength);
					int boxIndex = boxOffset + box * digits + num;

					//four constraints per row
					int candidateIndex = (row * digits * digits) + (col * digits) + num;
					_matrix[candidateIndex][cellIndex] = HAS_VALUE;
					_matrix[candidateIndex][rowIndex] = HAS_VALUE;
					_matrix[candidateIndex][colIndex] = HAS_VALUE;
					_matrix[candidateIndex][boxIndex] = HAS_VALUE;
				}
			}
		}
	}
}

//ConstraintMatrix::ConstraintMatrix(Sudoku sudoku) {
//	const int sudokuConstraints = 4;
//	const int cellOffset = 0 * sudoku.cellCount();
//	const int rowOffset = 1 * sudoku.cellCount();
//	const int colOffset = 2 * sudoku.cellCount();
//	const int boxOffset = 3 * sudoku.cellCount();
//
//	const int digits = sudoku.rowLength();
//	const int candidateRowCount = digits * sudoku.cellCount();
//	const int constraintColCount = sudokuConstraints * sudoku.cellCount();
//	_matrix = matrixGrid(candidateRowCount, matrixRow(constraintColCount, NO_VALUE));
//
//	//add constraints
//	for (int row = 0; row < sudoku.rowLength(); row++) {
//		for (int col = 0; col < sudoku.colLength(); col++) {
//			for (int num = 0; num < digits; num++) {
//				//add clues only if we have a blank, or when we have the correct clue
//				//todo: isEmptyCell(row, col)
//				if (sudoku.at(row, col) == BLANK_CELL_VALUE || sudoku.at(row, col) == num) {
//					//map each constraint to a unique value (offset + 0..cellCount-1)
//					int cellIndex = cellOffset + row * digits + col;
//					int rowIndex = rowOffset + row * digits + num;
//					int colIndex = colOffset + col * digits + num;
//					int box = (row / sudoku.boxLength()) * sudoku.boxLength() + (col / sudoku.boxLength());
//					int boxIndex = boxOffset + box * digits + num;
//
//					//four constraints per row
//					int candidateIndex = (row * digits * digits) + (col * digits) + num;
//					_matrix[candidateIndex][cellIndex] = HAS_VALUE;
//					_matrix[candidateIndex][rowIndex] = HAS_VALUE;
//					_matrix[candidateIndex][colIndex] = HAS_VALUE;
//					_matrix[candidateIndex][boxIndex] = HAS_VALUE;
//				}
//			}
//		}
//	}
//}

int ConstraintMatrix::rows() {
	return _matrix.size();
}

int ConstraintMatrix::cols() {
	return _matrix[0].size();
}

matrixCell ConstraintMatrix::at(const int row, const int col) {
	return _matrix[row][col];
}
