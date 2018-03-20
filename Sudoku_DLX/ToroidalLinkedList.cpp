#include "stdafx.h"

#include "ConstraintMatrix.h"
#include "ToroidalLinkedList.h"
#include "Sudoku.h"

ToroidalLinkedList::ToroidalLinkedList(sudokuGrid sudoku) {
}

void ToroidalLinkedList::initList(sudokuGrid sudoku) {
	ConstraintMatrix constraintMatrix(sudoku);
	const int rowSize = constraintMatrix.rows();
	const int colSize = constraintMatrix.cols();

	//create column root
	_columnRoot = new Node(ROW_HEADER, COL_HEADER);

	//create column header row
	for (int col = 0; col < colSize; col++) {
		Node *colHead = new Node(ROW_HEADER, col);
		_columnRoot->AddHorizontalNode(*colHead);
	}

	//create data node rows
	for (int row = 0; row < rowSize; row++) {
		//pretend first existing dataNode on each row is a row header, so we can append nodes horizontally
		Node *dataHead = nullptr;
		//create data nodes rowwise
		for (Node *colHead = _columnRoot->right; colHead != _columnRoot; colHead = colHead->right) {
			//create data node if constraintMatrix says so
			if (constraintMatrix.at(row, colHead->colId) == HAS_VALUE) {
				Node *data = new Node(row, colHead->colId);
				if (dataHead == nullptr)
					dataHead = data;
				else
					dataHead->AddHorizontalNode(*data);

				colHead->AddVerticalDataNode(*data);
			}
		}
	}
}

void ToroidalLinkedList::search() {
	//column list is empty, we have found a solution
	if (_columnRoot->right == _columnRoot) {
		_solutions.push_back(_guesses);
		return;
	}

	Node *colHead = getMinColumn();
	if (colHead->count == 0)	//unsolvable, backtrack
		return;

	//cover column
	colHead->coverColumnAndRows();

	//loop vertical nodes
	for (Node *dataRow = colHead->down; dataRow != colHead; dataRow = dataRow->down) {
		//guess that row is part of the solution, add to list
		_guesses.push_back(dataRow);

		//cover columns with intersecting rows
		for (Node *dataCol = dataRow->right; dataCol != dataRow; dataCol = dataCol->right) //crt{5011, 376 bytes}
			dataCol->columnHeader->coverColumnAndRows();

		//with conditional recursion instead of aborting, we can backtrack and leave original list intact
		//then we can safely delete all nodes from memory afterwards
		if (_solutions.size() < _maxSolutions)
			search();

		//backtrack the current guess
		for (Node *dataCol = dataRow->left; dataCol != dataRow; dataCol = dataCol->left)
			dataCol->columnHeader->uncoverColumnAndRows();

		_guesses.pop_back();
	}

	colHead->uncoverColumnAndRows();
}

void ToroidalLinkedList::solve(const unsigned int maxSolutions = 1) {
	_maxSolutions = maxSolutions == 0
					? std::numeric_limits<const unsigned int>::max()
					: maxSolutions;
	_solutions.clear();
	_guesses.clear();
	search();
}

Node* ToroidalLinkedList::getMinColumn() {
	Node *min = _columnRoot->right;
	for (Node *colHead = min->right; colHead != _columnRoot; colHead = colHead->right)
		if (colHead->count < min->count)
			min = colHead;

	return min;
}

ToroidalLinkedList::~ToroidalLinkedList() {
	Node *deleteHead, *deleteData;
	for (Node *colHead = _columnRoot->right; colHead != _columnRoot; colHead = colHead->right, delete deleteHead) {
		deleteHead = colHead;
		for (Node *data = colHead->down; data != colHead; data = data->down, delete deleteData) {
			deleteData = data;
		}
	}

	delete _columnRoot;
}
