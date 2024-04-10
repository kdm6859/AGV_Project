#ifndef __MINHEAP_H__
#define __MINHEAP_H__

#include "HeapNode.h"

#define MAX_ELEMENT 10000

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
//	void insert(int key);
//	HeapNode remove();
//	HeapNode find() { return node[1]; }
//
//	void display();
//};

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
class MinHeap
{
	HeapNode node[MAX_ELEMENT];
	int size;
public:
	MinHeap() :size(0) {}
	bool isEmpty() { return size == 0; }
	bool isFull() { /*std::cout << "full" << std::endl;*/ return size == MAX_ELEMENT - 1; }

	HeapNode& getParent(int i) { return node[i / 2]; }
	HeapNode& getLeft(int i) { return node[i * 2]; }
	HeapNode& getRight(int i) { return node[i * 2 + 1]; }

	void insert(Node* insert_node);
	HeapNode remove();
	HeapNode find() { return node[1]; }

	//void display();
};

Node* heap_minimum_find(LinkedList* linked_list);

#endif