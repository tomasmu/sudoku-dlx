#include "stdafx.h"
#include <vector>
#include <iostream>
#include <set>
#include <chrono>
#include <fstream>
#include <sstream>

#include "Node.h"
#include "Sudoku.h"
#include "ConstraintMatrix.h"
#include "ToroidalLinkedList.h"

//memory leak analysis
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW
#endif

//moved
sudokuGrid getSudokuFromSolution(std::vector<Node*> solution) {
	const unsigned int gridSize = solution.size();
	const int gridLength = (int)(sqrt(gridSize + 0.5));
	const int base = gridLength;
	sudokuGrid sudoku(gridLength, sudokuRow(gridLength, BLANK_CELL_VALUE));

	for (unsigned int i = 0; i < gridSize; i++) {
		int temp = solution[i]->rowId;

		int row = temp / (base * base);
		int col = temp / base % base;
		int num = temp % base;

		sudoku[row][col] = num;
	}

	return sudoku;
}

//moved
char formatCellIntToChar(int digit, bool isZeroBased) {
	if (digit == BLANK_CELL_VALUE)
		return BLANK_CELL_CHAR;

	if (isZeroBased == false)
		digit++;

	if (digit >= 0 && digit <= 9)
		return digit + '0';
	else if (digit >= 10 && digit <= 'Z' - 'A' + 10)
		return digit - 10 + 'A';

	return '?';
}

//moved
int formatCellCharToInt(char digit, bool isZeroBased) {
	if (digit == BLANK_CELL_CHAR)
		return BLANK_CELL_VALUE;

	int offset = isZeroBased == false ? -1 : 0;

	if (digit >= '0' && digit <= '9')
		return digit - '0' + offset;
	else if (digit >= 'A' && digit <= 'Z')
		return digit + 10 - 'A' + offset;

	return '?';
}

//moved
sudokuGrid getSudokuFromString(const char *sudokuString, bool isZeroBased) {
	int gridLength = (int)(sqrt(strlen(sudokuString)) + 0.5);
	int boxLength = (int)(sqrt(gridLength) + 0.5);
	if (boxLength * boxLength * boxLength * boxLength != strlen(sudokuString)) {
		std::cout << "WARNING! Input string is " << strlen(sudokuString) << " characters long, and not on the form n^4" << std::endl;
	}
	sudokuGrid sudoku(gridLength, sudokuRow(gridLength, BLANK_CELL_VALUE));

	for (int row = 0; row < gridLength; row++) {
		for (int col = 0; col < gridLength; col++) {
			int strIndex = row * gridLength + col;
			char charValue = sudokuString[strIndex];
			int cellValue = formatCellCharToInt(charValue, isZeroBased);
			sudoku[row][col] = cellValue;
		}
	}

	return sudoku;
}

//moved
std::string getSudokuAsString(sudokuGrid sudoku, bool isZeroBased) {
	const int length = sudoku.size();
	std::string result = "";
	for (int row = 0; row < length; row++) {
		for (int col = 0; col < length; col++) {
			char charValue = formatCellIntToChar(sudoku[row][col], isZeroBased);
			result += charValue;
		}
	}

	return result;
}

//moved
//void printSudoku(sudokuGrid sudoku, const bool isZeroBased) {
//	const int gridLength = sudoku.size();
//	const int boxLength = (int)(sqrt(gridLength) + 0.5);
//
//	const int drawGridWidth = 2 * boxLength * (boxLength + 1) + 1;
//	const std::string *gridLine = new std::string(drawGridWidth, '-');
//
//	for (int row = 0; row < gridLength; row++) {
//		if (row % boxLength == 0)
//			std::cout << gridLine->c_str() << std::endl;
//
//		for (int col = 0; col < gridLength; col++) {
//			if (col % boxLength == 0)
//				std::cout << "| ";
//
//			int cellValue = sudoku[row][col];
//			char charValue = formatCellIntToChar(cellValue, isZeroBased);
//			std::cout << charValue << " ";
//		}
//
//		std::cout << "|" << std::endl;
//	}
//
//	std::cout << gridLine->c_str() << std::endl;
//	delete gridLine;
//}

//todo
//void printAllSolutions(bool isZeroBased) {
//	std::cout << "solutions found: " << solutions.size() << std::endl;
//	for (unsigned int i = 0; i < solutions.size() && solutions.size() != 0; i++) {
//		std::cout << "printing solution [" << i << "]" << std::endl;
//		printSudoku(getSudokuFromSolution(solutions[i]), isZeroBased);
//	}
//}

//moved
bool isZeroBasedString(const char* sudokuString) {
	//is considered zero based only if it contains a zero
	return strpbrk(sudokuString, "0") != 0;
}

void solveFromFile(const char *filename) {
	auto startTime = std::chrono::high_resolution_clock::now();
	std::string answer, puzzle;
	std::ifstream inFile;
	inFile.open(filename);
	std::ofstream outFile("..\\sudokus-result.txt");
	if (inFile.is_open()) {
		while (std::getline(inFile, answer)) {
			std::stringstream lineStream(answer);
			getline(lineStream, answer, ';');
			lineStream >> puzzle;

			bool isZeroBased = isZeroBasedString(puzzle.c_str());
			sudokuGrid sudokuToSolve = getSudokuFromString(puzzle.c_str(), isZeroBased);

			//temp
			auto test = new ToroidalLinkedList(sudokuToSolve);
			test->solve(2);
			solutionList solutions = test->_solutions;

			if (solutions.size() > 0) {
				sudokuGrid solved = getSudokuFromSolution(solutions[solutions.size() - 1]);
				std::string solvedAsString = getSudokuAsString(solved, isZeroBased);
				std::string originalPuzzle = getSudokuAsString(sudokuToSolve, isZeroBased);
				outFile << solvedAsString << ";" << originalPuzzle << std::endl;
			}
			else {
				std::cout << "error: " << puzzle.c_str() << std::endl;
			}

			delete test;
		}
	}

	inFile.close();
	outFile.close();

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> programDuration = endTime - startTime;
	printf("%f ms\n", 1000 * programDuration.count());
}

//utils
bool isFilePath(const char* filename) {
	std::ifstream tryOpenFile(filename);
	return (bool)tryOpenFile;
}

int main(int argc, char *argv[]) {
	int crtFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	crtFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	crtFlag |= _CRTDBG_ALLOC_MEM_DF;
	crtFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(crtFlag);
#ifdef _DEBUG
	//crt {160, 8 bytes}, {159, 8 bytes} - todo: not found, investigate later
	//crt {5011, 376 bytes, solutionGuesses?}, {5012, 16 bytes, solutionList?} - todo: investigate after restructuring
	//_crtBreakAlloc = 159;
	_crtBreakAlloc = 999999;
#endif // _DEBUG
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	std::cout << "hello world!" << std::endl;

	if (argc > 1) {
		//try open file argv[1] and read all the sudokus
		//if file isn't found, try parse it as a sudoku string
		const char *argument = argv[1];
		if (isFilePath(argument)) {
			solveFromFile(argument);
		}
		else {	//wasn't a file, probably sudoku? absolutely not /help or something
			std::cout << "argument: " << argument << std::endl;
			auto startTime = std::chrono::high_resolution_clock::now();

			bool isZeroBased = isZeroBasedString(argument);
			sudokuGrid sudokuToSolve = getSudokuFromString(argument, isZeroBased);

			auto endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> programDuration = endTime - startTime;
			printf("%f ms\n", 1000 * programDuration.count());
		}
	}

	//example sudokus
	/*
	sudokuGrid two_sudoku_0clues = getSudokuFromString("................", false);
	sudokuGrid two_sudoku_1missing = getSudokuFromString("132424134132324.", false);

	sudokuGrid three_sudoku_17clues_worstBacktrackCase = getSudokuFromString("..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9", false);
	sudokuGrid three_sudoku_empty = getSudokuFromString(".................................................................................", false);
	sudokuGrid three_sudoku_4clues_2solutions = getSudokuFromString("8356179426124957387..2386511..763825367582194258149367571926483486351279923874516", false);

	sudokuGrid four_sudoku_8solutions = getSudokuFromString(".5..6..E.F.C....B.0.24...1.35.E..E...D.15.........9F..C...2..418.02.F3E..97...4.6C4D......1.E2.....5....B8..7DF.F.....D.3.A....90....E.C.B.....F.2DC..73....8.....F3.8......27BA.8...2A..3FD.19.DAB..C...E..19.........28.C...D..9.E8.5...BAF..6..C.E.B.7..9..2.", true);
	sudokuGrid four_sudoku_77clues = getSudokuFromString("5...3....BF.4..8...4..D6..E....3..1....2.........6..5.....21.....F.3.1...7.2.6...C4.A7.B...5E....2....4.6D..378.8....C...4...9D1..........46B.72D..5..A3C.......4........37.1..6..A8...D.....F.4.8...51..2.......A6.....7..3F.......4.C..5....3...........1.....", true);
	sudokuGrid four_sudoku_55clues = getSudokuFromString("...9.....3.....2....F..C0....A.8.4.5.....9.............A..D....F..8............0.....5..........A.F............C.....D9..4....7.....0..E.........5.4.....7.B1D9....3.....1..5.4.....A..F........F.0.....8.A....E.....14.....2.5.8.......C.0..........973......1.", true);
	sudokuGrid four_sudoku_54clues_ambiguous = getSudokuFromString(".........2.....1....E..BF....9.7.3.4.....8.............9..C....E..7............F.....4..........9.E............B.....C8..3....6.....F..D.........4.3.....6.A0C8....2.....0..4.3.....9..E........E.F.....7.9....D.....03.....1.4.7.......B.F..........862......0.", true);

	bool canHasBenchmark = false;
	if (canHasBenchmark) {
		std::cout << "benchmarking.." << std::endl;

		//sudokuGrid sudokuToSolve = four_sudoku_77clues;
		//sudokuGrid sudokuToSolve = three_sudoku_4clues_2solutions;
		sudokuGrid sudokuToSolve = three_sudoku_17clues_worstBacktrackCase;

		bool isZeroBased = isZeroBasedSudoku(sudokuToSolve);
		printSudoku(sudokuToSolve, isZeroBased);

		for (int numberOfRuns = 0; numberOfRuns < 5; numberOfRuns++) {
			auto startTime = std::chrono::high_resolution_clock::now();

			for (int solvesPerIteration = 0; solvesPerIteration < 100; solvesPerIteration++) {
				solutionGuesses.clear();
				solutionList.clear();
				Node *list = createLinkedListFromSudoku(sudokuToSolve);
				search(list);
				deleteLinkedList(list);
			}

			auto endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> programDuration = endTime - startTime;
			printf("%f ms\n", 1000 * programDuration.count());
		}
	}
	*/

	//temporary, to keep program open
	//bool noArgumentsSupplied = argc == 1;
	//if (noArgumentsSupplied || true) {
	//	system("pause");	//windows
	//	//system("read");		//mac/linux
	//}

	_CrtDumpMemoryLeaks();

	return 0;
}
