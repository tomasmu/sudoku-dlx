#include "stdafx.h"
#include <vector>
#include <iostream>
#include <set>
#include <chrono>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <memory>

using namespace std;

typedef std::vector<int> sudokurow;			//sudoku
typedef std::vector<sudokurow> sudokugrid;	//sudoku
const int BLANK_CELL_VALUE = -1;
const char BLANK_CELL_CHAR = '.';

//todo: perhaps move/rename/remove
const bool HAS_VALUE = 1;	//constraint matrix
const bool NO_VALUE = 0;	//constraint matrix

const int MAX_SOLUTIONS = 3000;

//generic node that acts as columnRoot, columnHeader, and dataNodes
//used for doubly linked list that is circular in two dimensions (up/down and left/right)
//could perhaps be split into struct Root, Header, Data; inheriting from struct Node

const int ROW_HEADER = -1;
const int COL_HEADER = -1;

struct Node {
public:				//used by; and why
	Node *left;		//root, header, data
	Node *right;	//root, header, data
	Node *up;		//header, data
	Node *down;		//header, data

	int count = 0;		//header, data; count linked nodes in column

	Node *columnHeader;	//data; point to head
	int rowId;	//data; for getting solution
	int colId;	//header; when creating linked list

	Node() {
		this->up = this;
		this->down = this;
		this->left = this;
		this->right = this;

		this->rowId = ROW_HEADER;
		this->colId = COL_HEADER;
	}

	Node(int row, int col) : Node() {
		this->rowId = row;
		this->colId = col;
	}

	void AddHorizontalNode(Node &newNode) {		//root, data
		newNode.right = this;			//1 new.right  = first
		newNode.left = this->left;		//2 new.left   = last (first.left)
		this->left->right = &newNode;	//3 last.right = new
		this->left = &newNode;			//4 first.left = new
	}

	void AddVerticalDataNode(Node &newNode) {	//header
		newNode.down = this;			//1 new.down  = first
		newNode.up = this->up;			//2 new.up    = last (first.up)
		this->up->down = &newNode;		//3 last.down = new
		this->up = &newNode;			//4 first.up  = new

		newNode.columnHeader = this;
		this->count++;
	}

	void coverColumnAndRows() {		//header
		this->coverHorizontalNode();		//cover column header
		for (Node *data = this->down; data != this; data = data->down)	//and all row elements from intersecting columns
			data->coverRow();
	}
	void uncoverColumnAndRows() {	//header
		for (Node *data = this->up; data != this; data = data->up)
			data->uncoverRow();
		this->uncoverHorizontalNode();
	}

private:
	void coverHorizontalNode() {	//(header)
		this->right->left = this->left;
		this->left->right = this->right;
	}
	void uncoverHorizontalNode() {
		this->right->left = this;
		this->left->right = this;
	}

	void coverVerticalNode() {	//(data)
		this->down->up = this->up;
		this->up->down = this->down;

		columnHeader->count--;
	}
	void uncoverVerticalNode() {
		this->down->up = this;
		this->up->down = this;

		columnHeader->count++;
	}

	void coverRow() {	//(header)
		for (Node *data = this->right; data != this; data = data->right)
			data->coverVerticalNode();
	}
	void uncoverRow() {
		for (Node *data = this->left; data != this; data = data->left)
			data->uncoverVerticalNode();
	}
};

vector<Node*> solutionGuesses;			//global :(
vector<vector<Node*>> solutionList;		//variables :(

Node *findMinColumn(Node *columnRoot) {
	Node *min = columnRoot->right;
	for (Node *colHead = min->right; colHead != columnRoot; colHead = colHead->right)
		if (colHead->count < min->count)
			min = colHead;

	return min;
}

void search(Node *columnRoot)
{
	//column list is empty, we have found a solution
	if (columnRoot->right == columnRoot) {
		solutionList.push_back(solutionGuesses);
		//printSolution();
		return;
	}

	Node *colHead = findMinColumn(columnRoot);

	if (colHead->count == 0)	//unsolvable, backtrack
		return;

	//cover column
	colHead->coverColumnAndRows();

	//loop vertical nodes
	for (Node *dataRow = colHead->down; dataRow != colHead; dataRow = dataRow->down) {
		//assume row is part of the solution, add to list
		solutionGuesses.push_back(dataRow);

		//cover columns with intersecting rows
		for (Node *dataCol = dataRow->right; dataCol != dataRow; dataCol = dataCol->right)
			dataCol->columnHeader->coverColumnAndRows();

		//with conditional recursion, instead of aborting, we backtrack and leave original list intact
		//then we can safely delete all nodes from memory
		if (solutionList.size() < MAX_SOLUTIONS)
			search(columnRoot);
		
		//backtrack
		for (Node *dataCol = dataRow->left; dataCol != dataRow; dataCol = dataCol->left)
			dataCol->columnHeader->uncoverColumnAndRows();

		solutionGuesses.pop_back();
	}

	colHead->uncoverColumnAndRows();
}

vector<vector<bool>> createConstraintMatrix(sudokugrid sudoku) {
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
	vector<vector<bool>> constraintMatrix(candidateCount, vector<bool>(constraintCount, NO_VALUE));

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
					constraintMatrix[candidateIndex][cellIndex] = HAS_VALUE;
					constraintMatrix[candidateIndex][rowIndex] = HAS_VALUE;
					constraintMatrix[candidateIndex][colIndex] = HAS_VALUE;
					constraintMatrix[candidateIndex][boxIndex] = HAS_VALUE;
				}
			}
		}
	}

	return constraintMatrix;
}

Node *createLinkedListFromSudoku(sudokugrid sudoku) {
	vector<vector<bool>> constraintMatrix = createConstraintMatrix(sudoku);

	const int rowSize = constraintMatrix.size();
	const int colSize = constraintMatrix[0].size();

	//root for column headers that will be returned
	Node *columnRoot = new Node(ROW_HEADER, COL_HEADER);

	//create column header row
	for (int col = 0; col < colSize; col++) {
		Node *colHead = new Node(ROW_HEADER, col);
		columnRoot->AddHorizontalNode(*colHead);
	}

	//create dataNode rows
	for (int row = 0; row < rowSize; row++) {
		//pretend first existing dataNode on each row is a row header, so we can append nodes horizontally
		Node *dataHead = nullptr;
		//create dataNodes rowwise
		for (Node *colHead = columnRoot->right; colHead != columnRoot; colHead = colHead->right) {
			//create data node if constraintMatrix says so
			if (constraintMatrix[row][colHead->colId] == HAS_VALUE) {
				Node *data = new Node(row, colHead->colId);
				if (dataHead == nullptr)
					dataHead = data;
				else
					dataHead->AddHorizontalNode(*data);

				colHead->AddVerticalDataNode(*data);
			}
		}
	}

	return columnRoot;
}

//avoid memory leaks :)
void deleteLinkedList(Node *list) {
	Node *deleteHead, *deleteData;
	for (Node *colHead = list->right; colHead != list; colHead = colHead->right, delete deleteHead) {
		deleteHead = colHead;
		for (Node *data = colHead->down; data != colHead; data = data->down, delete deleteData) {
			deleteData = data;
		}
	}

	delete list;
}

sudokugrid getSudokuFromSolution(vector<Node*> solution) {
	const unsigned int gridSize = solution.size();
	const int gridLength = (int)(sqrt(gridSize + 0.5));
	const int base = gridLength;
	sudokugrid sudoku(gridLength, sudokurow(gridLength, BLANK_CELL_VALUE));

	for (unsigned int i = 0; i < gridSize; i++) {
		int temp = solution[i]->rowId;

		int row = temp / (base * base);
		int col = temp / base % base;
		int num = temp % base;

		sudoku[row][col] = num;
	}

	return sudoku;
}

char formatCellIntToChar(int digit, bool isZeroBased) {
	if (digit == BLANK_CELL_VALUE)
		return BLANK_CELL_CHAR;

	if (isZeroBased == false)
		digit++;

	if (digit >= 0 && digit <= 9)
		return digit + '0';
	else if (digit >= 10 && digit <= 'Z' + 10)
		return digit - 10 + 'A';

	return '?';
}
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

sudokugrid getSudokuFromString(const char *sudokuString, bool isZeroBased) {
	int gridLength = (int)(sqrt(strlen(sudokuString)) + 0.5);
	int boxLength = (int)(sqrt(gridLength) + 0.5);
	if (boxLength * boxLength * boxLength * boxLength != strlen(sudokuString)) {
		cout << "WARNING! Input string is " << strlen(sudokuString) << " characters long, and not on the form n^4" << endl;
	}
	sudokugrid sudoku(gridLength, sudokurow(gridLength, BLANK_CELL_VALUE));

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

string getSudokuAsString(sudokugrid sudoku, bool isZeroBased) {
	const int length = sudoku.size();
	string result = "";
	for (int row = 0; row < length; row++) {
		for (int col = 0; col < length; col++) {
			char charValue = formatCellIntToChar(sudoku[row][col], isZeroBased);
			result += charValue;
		}
	}

	return result;
}

void printSudoku(sudokugrid sudoku, const bool isZeroBased) {
	//cout << "printing sudoku with iszerobased: " << (isZeroBased ? "true" : "false") << endl;
	const int gridLength = sudoku.size();
	const int boxLength = (int)(sqrt(gridLength) + 0.5);

	const int drawGridWidth = 2 * boxLength * (boxLength + 1) + 1;
	const string *gridLine = new string(drawGridWidth, '-');

	for (int row = 0; row < gridLength; row++) {
		if (row % boxLength == 0)
			cout << gridLine->c_str() << endl;

		for (int col = 0; col < gridLength; col++) {
			if (col % boxLength == 0)
				cout << "| ";

			int cellValue = sudoku[row][col];
			char charValue = formatCellIntToChar(cellValue, isZeroBased);
			cout << charValue << " ";
		}

		cout << "|" << endl;
	}

	cout << gridLine->c_str() << endl;
	delete gridLine;
}

void printAllSolutions(bool isZeroBased) {
	cout << "solutions found: " << solutionList.size() << endl;
	for (unsigned int i = 0; i < solutionList.size() && solutionList.size() != 0; i++) {
		cout << "printing solution [" << i << "]" << endl;
		printSudoku(getSudokuFromSolution(solutionList[i]), isZeroBased);
	}
}

bool isSolved(sudokugrid sudoku) {
	//the set size for rows, cols, boxes should all be equal to sudoku size
	return false;
}

bool isZeroBasedSudoku(sudokugrid sudoku) {
	//if we find a 0 it is zero based (if there's a 9 too then no solution is possible)
	//0-to-8 sudoku with 0 missing, then that 0 could be a 9, so we default to 1
	//1 and 9 cannot both be missing, that is ambiguous since they can swap freely, we can safely default to 1 with 9 gone
	//conclusion: if (0 in sudoku) true, else false
	for (unsigned int row = 0; row < sudoku.size(); row++)
		for (unsigned int col = 0; col < sudoku.size(); col++)
			if (sudoku[row][col] == 0)
				return true;

	return false;
}

bool isZeroBasedString(const char* sudokuString) {
	return strpbrk(sudokuString, "0") != 0;
}

void solveFromFile(const char *filename) {
	bool isZeroBased = isZeroBasedString(filename);
	auto startTime = std::chrono::high_resolution_clock::now();
	string answer, puzzle;
	ifstream sudokuFile;
	sudokuFile.open(filename);
	ofstream outFile("..\\sudokus-result.txt");
	if (sudokuFile.is_open()) {
		while (getline(sudokuFile, answer)) {
			stringstream lineStream(answer);
			getline(lineStream, answer, ';');
			lineStream >> puzzle;

			sudokugrid sudokuToSolve = getSudokuFromString(puzzle.c_str(), isZeroBased);
			Node *list = createLinkedListFromSudoku(sudokuToSolve);
			search(list);

			sudokugrid solved = getSudokuFromSolution(solutionList[solutionList.size() - 1]);
			string solvedAsString = getSudokuAsString(solved, isZeroBased);
			string originalPuzzle = getSudokuAsString(sudokuToSolve, isZeroBased);
			outFile << solvedAsString << ";" << originalPuzzle << endl;
		}
	}

	sudokuFile.close();
	outFile.close();

	auto endTime = chrono::high_resolution_clock::now();
	chrono::duration<double> programDuration = endTime - startTime;
	printf("%f ms\n", 1000 * programDuration.count());
}

//utils
bool isFilePath(const char* filename) {
	ifstream tryOpenFile(filename);
	return (bool)tryOpenFile;
}

int main(int argc, char *argv[]) {
	cout << "hello world!" << endl;

	if (argc > 1) {
		//try open file argv[1] and read all the sudokus
		//if file isn't found, try parse it as a sudoku string
		const char *argument = argv[1];
		if (isFilePath(argument)) {
			solveFromFile(argument);

			if (solutionList.size() > 0) {
				//bool isZeroBased = isZeroBasedSudoku(getSudokuFromSolution(solutionList[0]));
				//printAllSolutions(isZeroBased);
			}

			cout << "solutions found: " << solutionList.size() << endl;
		}
		else {	//wasn't a file, probably sudoku? absolutely not /help or something
			cout << "argument: " << argument << endl;
			auto startTime = chrono::high_resolution_clock::now();

			bool isZeroBased = isZeroBasedString(argument);
			sudokugrid sudokuToSolve = getSudokuFromString(argument, isZeroBased);
			Node *list = createLinkedListFromSudoku(sudokuToSolve);
			search(list);

			printAllSolutions(isZeroBased);
			deleteLinkedList(list);

			auto endTime = chrono::high_resolution_clock::now();
			chrono::duration<double> programDuration = endTime - startTime;
			printf("%f ms\n", 1000 * programDuration.count());
		}
	}

	//example sudokus
	sudokugrid two_sudoku_0clues = getSudokuFromString("................", false);
	sudokugrid two_sudoku_1missing = getSudokuFromString("132424134132324.", false);

	sudokugrid three_sudoku_17clues_worstBacktrackCase = getSudokuFromString("..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9", false);
	sudokugrid three_sudoku_empty = getSudokuFromString(".................................................................................", false);
	sudokugrid three_sudoku_4clues_2solutions = getSudokuFromString("8356179426124957387..2386511..763825367582194258149367571926483486351279923874516", false);

	sudokugrid four_sudoku_8solutions = getSudokuFromString(".5..6..E.F.C....B.0.24...1.35.E..E...D.15.........9F..C...2..418.02.F3E..97...4.6C4D......1.E2.....5....B8..7DF.F.....D.3.A....90....E.C.B.....F.2DC..73....8.....F3.8......27BA.8...2A..3FD.19.DAB..C...E..19.........28.C...D..9.E8.5...BAF..6..C.E.B.7..9..2.", true);
	sudokugrid four_sudoku_77clues = getSudokuFromString("5...3....BF.4..8...4..D6..E....3..1....2.........6..5.....21.....F.3.1...7.2.6...C4.A7.B...5E....2....4.6D..378.8....C...4...9D1..........46B.72D..5..A3C.......4........37.1..6..A8...D.....F.4.8...51..2.......A6.....7..3F.......4.C..5....3...........1.....", true);
	sudokugrid four_sudoku_55clues = getSudokuFromString("...9.....3.....2....F..C0....A.8.4.5.....9.............A..D....F..8............0.....5..........A.F............C.....D9..4....7.....0..E.........5.4.....7.B1D9....3.....1..5.4.....A..F........F.0.....8.A....E.....14.....2.5.8.......C.0..........973......1.", true);
	sudokugrid four_sudoku_54clues_ambiguous = getSudokuFromString(".........2.....1....E..BF....9.7.3.4.....8.............9..C....E..7............F.....4..........9.E............B.....C8..3....6.....F..D.........4.3.....6.A0C8....2.....0..4.3.....9..E........E.F.....7.9....D.....03.....1.4.7.......B.F..........862......0.", true);

	bool canHasBenchmark = false;
	if (canHasBenchmark) {
		cout << "benchmarking.." << endl;

		//sudokugrid sudokuToSolve = four_sudoku_77clues;
		//sudokugrid sudokuToSolve = three_sudoku_4clues_2solutions;
		sudokugrid sudokuToSolve = three_sudoku_17clues_worstBacktrackCase;

		bool isZeroBased = isZeroBasedSudoku(sudokuToSolve);
		printSudoku(sudokuToSolve, isZeroBased);

			for (int numberOfRuns = 0; numberOfRuns < 5; numberOfRuns++) {
			auto startTime = chrono::high_resolution_clock::now();

			for (int solvesPerIteration = 0; solvesPerIteration < 1000; solvesPerIteration++) {
				solutionGuesses.clear();
				solutionList.clear();
				Node *list = createLinkedListFromSudoku(sudokuToSolve);
				search(list);
				deleteLinkedList(list);
			}

			auto endTime = chrono::high_resolution_clock::now();
			chrono::duration<double> programDuration = endTime - startTime;
			printf("%f ms\n", 1000 * programDuration.count());
		}
	}

	//temporary, to keep program open
	bool noArgumentsSupplied = argc == 1;
	if (noArgumentsSupplied || true) {
		system("pause");	//windows
		//system("read");		//mac/linux
	}

	return 0;
}
