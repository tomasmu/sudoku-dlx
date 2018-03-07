#include "stdafx.h"

class Node {
public:
	Node * left;
	Node* right;
	Node* up;
	Node* down;

	int count;

	Node* columnHeader;
	int rowId;
	int colId;

	void coverColumnsAndRows();

	void addDataNode();
private:
	void coverHorizontalNode();
	void uncoverHorizontalNode();

	void coverVerticalNode();
	void uncoverVerticalNode();

	void coverRow();
	void uncoverRow();
};
