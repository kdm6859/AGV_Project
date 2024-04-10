#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "Node.h"

//class LinkedList
//{
//	Node org;
//public:
//	LinkedList(): org() {}
//	~LinkedList() { clear(); }
//	void clear() { while (!isEmpty()) delete remove(0); }
//	Node* getHead() { return org.getLink(); }
//	bool isEmpty() { return getHead() == NULL; }
//
//	//pos��° �׸��� ��ȯ��
//	Node* getEntry(int pos) {
//		Node* n = &org;
//		for (int i = -1; i < pos; i++, n = n->getLink())
//			if (n == NULL) break;
//		return n;
//	}
//
//	//����Ʈ�� � ��ġ�� �׸� ����
//	void insert(int pos, Node* n) {
//		Node* prev = getEntry(pos - 1);
//		if (prev != NULL)
//			prev->insertNext(n);
//	}
//
//	//����Ʈ�� � ��ġ�� �׸� ����
//	Node* remove(int pos) {
//		Node* prev = getEntry(pos - 1);
//		return prev->removeNext();
//	}
//
//	//Ž�� �Լ�
//	Node* find(int val) {
//		for (Node* p = getHead(); p != NULL; p = p->getLink())
//			if (p->hasData(val)) return p;
//		return NULL;
//	}
//
//	//����Ʈ�� � ��ġ�� �׸� ����
//	void replace(int pos, Node* n) {
//		Node* prev = getEntry(pos - 1);
//		if (prev != NULL) {
//			delete prev->removeNext();
//		}
//	}
//
//	//����Ʈ �׸� ������ ��ȯ
//	int size() {
//		int count = 0;
//		for (Node* p = getHead(); p != NULL; p = p->getLink())
//			count++;
//		return count;
//	}
//
//	//ȭ�鿡 ���� ���� ���
//	void display() {
//		std::cout << "[��ü �׸� �� = " << size() << "] : " << std::endl;
//		for (Node* p = getHead(); p != NULL; p = p->getLink())
//			p->display();
//		std::cout << std::endl;
//	}
//};

class LinkedList
{
	Node org;
public:
	LinkedList() : org() {}
	~LinkedList() { clear(); }
	void clear() { while (!isEmpty()) delete remove(0); std::cout << "LinkedList clear good" << std::endl; }
	Node* getHead() { return org.getLink(); }
	bool isEmpty() { return getHead() == NULL; }

	//pos��° �׸��� ��ȯ��
	Node* getEntry(int pos) {
		Node* n = &org;
		for (int i = -1; i < pos; i++, n = n->getLink())
			if (n == NULL) break;
		return n;
	}

	//����Ʈ�� � ��ġ�� �׸� ����
	void insert(int pos, Node* n) {
		Node* prev = getEntry(pos - 1);
		if (prev != NULL)
			prev->insertNext(n);
	}

	//����Ʈ�� � ��ġ�� �׸� ����
	Node* remove(int pos) {
		Node* prev = getEntry(pos - 1);
		return prev->removeNext();
	}

	//Ž�� �Լ�
	Node* find(cv::Point point) {
		for (Node* p = getHead(); p != NULL; p = p->getLink())
			if (p->hasPoint(point)) return p;
		return NULL;
	}

	//��ġ Ž�� �Լ�
	int find_pos(cv::Point point) {
		int pos = 0;
		for (Node* p = getHead(); p != NULL; p = p->getLink()) {
			if (p->hasPoint(point)) return pos;
			pos++;
		}
		return -1;
	}

	//����Ʈ�� � ��ġ�� �׸� ����
	void replace(int pos, Node* n) {
		Node* prev = getEntry(pos - 1);
		if (prev != NULL) {
			delete prev->removeNext();
		}
	}

	//����Ʈ �׸� ������ ��ȯ
	int size() {
		int count = 0;
		for (Node* p = getHead(); p != NULL; p = p->getLink())
			count++;
		return count;
	}

	//ȭ�鿡 ���� ���� ���
	void display() {
		std::cout << "[��ü �׸� �� = " << size() << "] : " << std::endl;
		for (Node* p = getHead(); p != NULL; p = p->getLink())
			p->display();
		std::cout << std::endl;
	}
};

#endif