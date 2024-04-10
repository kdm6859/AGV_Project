#ifndef __MINHEAP_H__
#define __MINHEAP_H__

#include "Node.h"

#define MAX_ELEMENT 5000//(여기부터)닫힌 노드 빠지는거 생각하면 배열크기가 작아도 될듯? openlist->힙으로 바꾸는 방향으로 가보자
#define MAX_ELEMENT_INT 50



////MinHeap_ver1
//class MinHeap
//{
//	HeapNode node[MAX_ELEMENT];
//	int size;
//public:
//	MinHeap() :size(0) {}
//	bool isEmpty() { return size == 0; }
//	bool isFull() { return size == MAX_ELEMENT - 1; }
//
//	HeapNode& getParent(int i) { return node[i / 2]; }
//	HeapNode& getLeft(int i) { return node[i * 2]; }
//	HeapNode& getRight(int i) { return node[i * 2 + 1]; }
//
//	void insert(cv::Point point, float F_distance, float G_distance, float H_distance, HeapNode* p_node);
//	HeapNode remove();
//	HeapNode find() { return node[1]; }
//
//	void display();
//};

////MinHeap_ver2
//class MinHeap
//{
//	HeapNode* node[MAX_ELEMENT];
//	int size;
//public:
//	MinHeap() :size(0) {}
//	bool isEmpty() { return size == 0; }
//	bool isFull() { return size == MAX_ELEMENT - 1; }
//
//	(HeapNode*)& getParent(int i) { return node[i / 2]; }
//	(HeapNode*)& getLeft(int i) { return node[i * 2]; }
//	(HeapNode*)& getRight(int i) { return node[i * 2 + 1]; }
//
//	void insert(HeapNode* insert_node);
//	HeapNode* remove();
//	HeapNode* find() { return node[1]; }
//
//	void display();
//};

////MinHeap_ver3
//class MinHeap
//{
//	HeapNode node[MAX_ELEMENT];
//	int size;
//public:
//	MinHeap() :size(0) {}
//	bool isEmpty() { return size == 0; }
//	bool isFull() { return size == MAX_ELEMENT - 1; }
//
//	HeapNode& getParent(int i) { return node[i / 2]; }
//	HeapNode& getLeft(int i) { return node[i * 2]; }
//	HeapNode& getRight(int i) { return node[i * 2 + 1]; }
//
//	void insert(cv::Point point, float F_distance, float G_distance, float H_distance, cv::Point p_point);
//	HeapNode remove();
//	HeapNode find() { return node[1]; }
//
//	void display();
//};

//MinHeap_ver4
class MinHeap_Node
{
	Node** node;
	int size;
public:
	MinHeap_Node() :size(0) { node = new Node * [MAX_ELEMENT]; }
	~MinHeap_Node() { clear(); }
	void clear() {
		if (size!=0) {
			for (int i = 1; i <= size; i++) {
				
				delete node[i];
			}
		}
		delete[] node;
		std::cout << "MinHeap_Node clear good" << std::endl;
	}
	bool isEmpty() { return size == 0; }
	bool isFull() { 
		bool tf = (size == MAX_ELEMENT - 1);
		if(tf)
			std::cout << "full" << std::endl; 
		return tf;
		//return size == MAX_ELEMENT - 1; 
	}
	Node*& getParent(int i) { return node[i / 2]; }
	Node*& getLeft(int i) { return node[i * 2]; }
	Node*& getRight(int i) { return node[i * 2 + 1]; }

	void insert(Node* insert_node);
	//void change(cv::Point point);
	Node* remove();
	Node* find_node(cv::Point point);
	Node* find() { return node[1]; }

	//void display();
};

class MinHeap_Node_intAry
{
	Node_intAry node[MAX_ELEMENT_INT];
	int size;
public:
	MinHeap_Node_intAry() :size(0) {}
	bool isEmpty() { return size == 0; }
	bool isFull() { return size == MAX_ELEMENT_INT - 1; }

	Node_intAry& getParent(int i) { return node[i / 2]; }
	Node_intAry& getLeft(int i) { return node[i * 2]; }
	Node_intAry& getRight(int i) { return node[i * 2 + 1]; }

	void insert(int key, int index);
	Node_intAry remove();
	Node_intAry find() { return node[1]; }

	void display();
};

int heap_minimum_find_index__Exception_zero(int* ary, int ary_size);
//Node* heap_minimum_find(LinkedList* linked_list);

#endif