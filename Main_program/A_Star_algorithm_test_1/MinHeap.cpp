#include "MinHeap.h"

//void MinHeap::insert(int key) {
//	if (isFull())
//		return;
//	int i = ++size;
//	while (i != 1 && key < getParent(i).getKey()) {
//		node[i] = getParent(i);
//		i /= 2;
//	}
//	node[i].setKey(key);
//}
//
//HeapNode MinHeap::remove() {
//	if (isEmpty())
//		return NULL;
//	HeapNode root = node[1];
//	HeapNode last = node[size--];
//	int parent = 1;
//	int child = 2;
//	while (child <= size) {
//		if (child < size && getLeft(parent).getKey() > getRight(parent).getKey())
//			child++;
//		if (last.getKey() <= node[child].getKey())
//			break;
//		node[parent] = node[child];
//		parent = child;
//		child *= 2;
//	}
//	node[parent] = last;
//	return root;
//}
//
//void MinHeap::display() {
//	for (int i = 1, level = 1; i <= size; i++) {
//		if (i == level) {
//			std::cout << std::endl;
//			level *= 2;
//		}
//		node[i].display();
//		std::cout << " ";
//	}
//	std::cout << "\n-------------------------" << std::endl;
//}

////MinHeap_ver1
//void MinHeap::insert(cv::Point point, float F_distance, float G_distance, float H_distance, HeapNode* p_node) {
//	if (isFull())
//		return;
//	int i = ++size;
//	while (i != 1 && F_distance < getParent(i).getKey()) {
//		node[i] = getParent(i);
//		i /= 2;
//	}
//	node[i].setKey(point, F_distance, G_distance, H_distance, p_node);
//}

////insert_ver2
//void MinHeap::insert(HeapNode* insert_node) {
//	if (isFull())
//		return;
//	int i = ++size;
//	while (i != 1 && insert_node->getKey() < getParent(i)->getKey()) {
//		node[i] = getParent(i);
//		i /= 2;
//	}
//	node[i] = insert_node;
//}

//MinHeap_ver3
void MinHeap::insert(cv::Point point, float F_distance, float G_distance, float H_distance, cv::Point p_point) {
	if (isFull())
		return;

	/*for (int i = size; i > 0; i--) {
		if(node[i].getPoint())
	}*/

	int i = ++size;
	

	while (i != 1 && F_distance < getParent(i).getKey()) {
		node[i] = getParent(i);
		i /= 2;
	}
	node[i].setKey(point, F_distance, G_distance, H_distance, p_point);
}

//remove_ver1
HeapNode MinHeap::remove() {
	if (isEmpty())
		return NULL;
	HeapNode root = node[1];
	HeapNode last = node[size--];
	int parent = 1;
	int child = 2;
	while (child <= size) {
		if (child < size && getLeft(parent).getKey() > getRight(parent).getKey())
			child++;
		if (last.getKey() <= node[child].getKey())
			break;
		node[parent] = node[child];
		parent = child;
		child *= 2;
	}
	node[parent] = last;
	return root;
}

////remove_ver2
//HeapNode* MinHeap::remove() {
//	if (isEmpty())
//		return NULL;
//	HeapNode* root = node[1];
//	HeapNode* last = node[size--];
//	int parent = 1;
//	int child = 2;
//	while (child <= size) {
//		if (child < size && getLeft(parent)->getKey() > getRight(parent)->getKey())
//			child++;
//		if (last->getKey() <= node[child]->getKey())
//			break;
//		node[parent] = node[child];
//		parent = child;
//		child *= 2;
//	}
//	node[parent] = last;
//	return root;
//}

//display_ver1
void MinHeap::display() {
	for (int i = 1, level = 1; i <= size; i++) {
		if (i == level) {
			std::cout << std::endl;
			level *= 2;
		}
		node[i].display();
		std::cout << " ";
	}
	std::cout << "\n-------------------------" << std::endl;
}

////display_ver2
//void MinHeap::display() {
//	for (int i = 1, level = 1; i <= size; i++) {
//		if (i == level) {
//			std::cout << std::endl;
//			level *= 2;
//		}
//		node[i]->display();
//		std::cout << " ";
//	}
//	std::cout << "\n-------------------------" << std::endl;
//}