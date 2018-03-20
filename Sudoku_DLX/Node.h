#pragma once

//generic node that acts as columnRoot, columnHeader, and dataNodes
//used for doubly linked list that is circular in two dimensions (up/down and left/right)
//could perhaps be split into struct Root, Header, Data; inheriting from some struct Node

const int ROW_HEADER = -1;	//remove later
const int COL_HEADER = -1;	//remove later

struct Node {
public:
	Node *left;
	Node *right;
	Node *up;
	Node *down;

	int count = 0;

	Node *columnHeader;
	int rowId;
	int colId;

	Node();
	Node(int row, int col);

	void AddHorizontalNode(Node &newNode);
	void AddVerticalDataNode(Node &newNode);
	void coverColumnAndRows();
	void uncoverColumnAndRows();

private:
	void coverHorizontalNode();
	void uncoverHorizontalNode();
	void coverVerticalNode();
	void uncoverVerticalNode();
	void coverRow();
	void uncoverRow();
};
