#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <opencv2/opencv.hpp>

//class Node
//{
//	Node* link;
//	cv::Point point;
//	float F_distance;
//	float G_distance;
//	float H_distance;
//	Node* parent_node;
//public:
//	Node(float F_distance =0, float G_distance = 0, float H_distance = 0):
//		F_distance(F_distance), G_distance(G_distance), H_distance(H_distance), link(NULL), parent_node(NULL){}
//	Node* getLink() { return link; }
//	void setLink(Node* next) { link = next; }
//	void display() { std::cout << F_distance << std::endl; }
//	bool hasData(int val) { return F_distance == val; }
//
//	void insertNext(Node* n) {
//		if (n != NULL) {
//			n->link = link;
//			link = n;
//		}
//	}
//
//	Node* removeNext() {
//		Node* removed = link;
//		if (removed != NULL)
//			link = removed->link;
//		return removed;
//	}
//};

class Node
{
	Node* link;
	cv::Point point;
	float F_distance;
	float G_distance;
	float H_distance;
	Node* parent_node;
public:
	Node(cv::Point point = cv::Point(-1, -1), float F_distance = 0, float G_distance = 0, float H_distance = 0, Node* parent_node = NULL) :
		point(point), F_distance(F_distance), G_distance(G_distance), H_distance(H_distance), link(NULL), parent_node(parent_node) {}
	void setNode(float F_distance, float G_distance, float H_distance, Node* parent_node) {
		this->F_distance = F_distance; this->G_distance = G_distance; this->H_distance = H_distance; this->parent_node = parent_node;
	}
	Node* getLink() { return link; }
	Node* get_parent_node() { return parent_node; }
	void setLink(Node* next) { link = next; }
	void display() { std::cout << F_distance << point << std::endl; }
	bool hasData(int val) { return F_distance == val; }
	bool hasPoint(cv::Point point) { return this->point == point; }
	float get_F_distance() { return F_distance; }
	float get_G_distance() { return G_distance; }
	cv::Point getPoint() { return point; }

	void insertNext(Node* n) {
		if (n != NULL) {
			n->link = link;
			link = n;
		}
	}

	Node* removeNext() {
		Node* removed = link;
		if (removed != NULL)
			link = removed->link;
		return removed;
	}
};

#endif