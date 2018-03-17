#include "stdafx.h"
#include "Node.h"

Node::Node() {
	this->up = this;
	this->down = this;
	this->left = this;
	this->right = this;

	this->rowId = ROW_HEADER;
	this->colId = COL_HEADER;
}

Node::Node(int row, int col) : Node() {
	this->rowId = row;
	this->colId = col;
}

void Node::AddHorizontalNode(Node &newNode) {
	newNode.right = this;
	newNode.left = this->left;
	this->left->right = &newNode;
	this->left = &newNode;
}

void Node::AddVerticalDataNode(Node &newNode) {
	newNode.down = this;
	newNode.up = this->up;
	this->up->down = &newNode;
	this->up = &newNode;

	newNode.columnHeader = this;
	this->count++;
}

void Node::coverColumnAndRows() {
	this->coverHorizontalNode();
	for (Node *data = this->down; data != this; data = data->down)
		data->coverRow();
}

void Node::uncoverColumnAndRows() {
	for (Node *data = this->up; data != this; data = data->up)
		data->uncoverRow();
	this->uncoverHorizontalNode();
}

void Node::coverHorizontalNode() {
	this->right->left = this->left;
	this->left->right = this->right;
}

void Node::uncoverHorizontalNode() {
	this->right->left = this;
	this->left->right = this;
}

void Node::coverVerticalNode() {
	this->down->up = this->up;
	this->up->down = this->down;

	columnHeader->count--;
}

void Node::uncoverVerticalNode() {
	this->down->up = this;
	this->up->down = this;

	columnHeader->count++;
}

void Node::coverRow() {
	for (Node *data = this->right; data != this; data = data->right)
		data->coverVerticalNode();
}

void Node::uncoverRow() {
	for (Node *data = this->left; data != this; data = data->left)
		data->uncoverVerticalNode();
}
