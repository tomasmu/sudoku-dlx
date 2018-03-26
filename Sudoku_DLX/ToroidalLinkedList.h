#pragma once

#include <vector>
#include "Node.h"

typedef std::vector<Node*> solution;
typedef std::vector<solution> solutionList;

class ToroidalLinkedList {
public:
	ToroidalLinkedList(std::vector<std::vector<int>> input);
	~ToroidalLinkedList();
	void solve(const unsigned int maxSolutions = 1);
	
	solutionList _solutions;	//todo
private:
	Node *_columnRoot;
	solution _guesses;
	unsigned int _maxSolutions;

	void search();
	Node* getMinColumn();
};
