#pragma once
#include <vector>

typedef std::vector<int> sudokuRow;
typedef std::vector<sudokuRow> sudokuGrid;

const int BLANK_CELL_VALUE = -1;
const char BLANK_CELL_CHAR = '.';

const int MAX_SOLUTIONS = 3000;

