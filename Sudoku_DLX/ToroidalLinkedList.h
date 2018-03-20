#pragma once

#include "Node.h"

typedef std::vector<Node*> solution;
typedef std::vector<solution> solutionList;

class ToroidalLinkedList {
public:
	ToroidalLinkedList(sudokuGrid sudoku);	//todo: (Sudoku sudoku)
	~ToroidalLinkedList();
	void solve(const unsigned int maxSolutions);
	Node* getMinColumn();
	
private:
	Node *_columnRoot;
	solution _guesses;
	solutionList _solutions;
	unsigned int _maxSolutions;

	void initList(sudokuGrid sudoku);
	void search();
};
