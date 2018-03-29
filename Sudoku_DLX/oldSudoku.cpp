#include "stdafx.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

#include "Sudoku.h"

//memory leak analysis
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW
#endif

void solveFromFile(const char *filename) {
	std::cout << "solving from file: " << filename << std::endl;
	auto startTime = std::chrono::high_resolution_clock::now();
	std::string answer, puzzle;
	std::ofstream outFile("..\\sudokus-result.txt");
	std::ifstream inFile;
	inFile.open(filename);
	if (inFile.is_open()) {
		while (std::getline(inFile, answer)) {
			std::stringstream lineStream(answer);
			getline(lineStream, answer, ';');
			lineStream >> puzzle;

			Sudoku sudoku(puzzle.c_str());
			sudoku.solve();
			//sudoku.printSolution();

			std::string solved = sudoku.getSolution();
			//std::string original = sudoku.getOriginal();
			outFile << solved << ";" << puzzle << std::endl;
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
	//_crtBreakAlloc = 48763;	//memory_leak_test
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

			Sudoku sudoku = Sudoku(argument);
			
			sudoku.solve(0);
			std::cout << "original: " << sudoku.getOriginal() << std::endl;
			std::cout << "solution: " << sudoku.getSolution(0) << std::endl;
			sudoku.printOriginal();
			sudoku.printSolution(0);

			auto endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> programDuration = endTime - startTime;
			printf("%f ms\n", 1000 * programDuration.count());
		}
	}

	//example sudokus
	const char* two_sudoku_0clues = "................";
	const char* two_sudoku_1missing = "132424134132324.";
	const char* three_sudoku_17clues_worstBacktrackCase = "..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9";
	const char* three_sudoku_empty = ".................................................................................";
	const char* three_sudoku_4clues_2solutions = "8356179426124957387..2386511..763825367582194258149367571926483486351279923874516";
	const char* four_sudoku_8solutions = ".5..6..E.F.C....B.0.24...1.35.E..E...D.15.........9F..C...2..418.02.F3E..97...4.6C4D......1.E2.....5....B8..7DF.F.....D.3.A....90....E.C.B.....F.2DC..73....8.....F3.8......27BA.8...2A..3FD.19.DAB..C...E..19.........28.C...D..9.E8.5...BAF..6..C.E.B.7..9..2.";
	const char* four_sudoku_77clues = "5...3....BF.4..8...4..D6..E....3..1....2.........6..5.....21.....F.3.1...7.2.6...C4.A7.B...5E....2....4.6D..378.8....C...4...9D1..........46B.72D..5..A3C.......4........37.1..6..A8...D.....F.4.8...51..2.......A6.....7..3F.......4.C..5....3...........1.....";
	const char* four_sudoku_55clues = "...9.....3.....2....F..C0....A.8.4.5.....9.............A..D....F..8............0.....5..........A.F............C.....D9..4....7.....0..E.........5.4.....7.B1D9....3.....1..5.4.....A..F........F.0.....8.A....E.....14.....2.5.8.......C.0..........973......1.";
	const char* four_sudoku_54clues_ambiguous = ".........2.....1....E..BF....9.7.3.4.....8.............9..C....E..7............F.....4..........9.E............B.....C8..3....6.....F..D.........4.3.....6.A0C8....2.....0..4.3.....9..E........E.F.....7.9....D.....03.....1.4.7.......B.F..........862......0.";

	bool canHasBenchmark = true;
	if (canHasBenchmark) {
		std::cout << "benchmarking.." << std::endl;
		const char* sudokuToSolve = three_sudoku_17clues_worstBacktrackCase;

		int numberOfRuns = 5;
		int solvesPerIteration = 1000;
#ifdef _DEBUG
		//a lot slower than Release
		numberOfRuns = 3;
		solvesPerIteration = 4;
#endif //_DEBUG

		for (int run = 0; run < numberOfRuns; run++) {
			auto startTime = std::chrono::high_resolution_clock::now();
			for (int iteration = 0; iteration < solvesPerIteration; iteration++) {
				Sudoku sudoku(sudokuToSolve);
				sudoku.solve();
			}

			auto endTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> programDuration = endTime - startTime;
			printf("%f ms\n", 1000 * programDuration.count());
		}
	}

	//temporary, to keep program open
	//bool noArgumentsSupplied = argc == 1;
	//if (noArgumentsSupplied || true) {
	//	system("pause");	//windows
	//	//system("read");		//mac/linux
	//}

	//auto memory_leak_test = new std::string();
	_CrtDumpMemoryLeaks();

	return 0;
}
