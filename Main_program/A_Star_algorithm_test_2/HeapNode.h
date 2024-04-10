#ifndef __HEAPNODE_H__
#define __HEAPNODE_H__

#include <iostream>
#include <opencv2/opencv.hpp>
#include "LinkedList.h"
#include "Node.h"
//class HeapNode
//{
//	int key;
//public:
//	HeapNode(int k = 0) : key(k) {}
//	void setKey(int k) { key = k; }
//	int getKey() { return key; }
//	void display() { std::cout << key; }
//};

////HeapNode_ver1,2
//class HeapNode
//{
//	cv::Point point;
//	float F_distance;
//	float G_distance;
//	float H_distance;
//	HeapNode* parent_node;
//public:
//	/*HeapNode(cv::Point point = cv::Point(-1, -1), float f = 0, float g = 0, float h = 0) :
//		point(point), F_distance(f), G_distance(g), H_distance(h) { parent_node = NULL; }*/
//	//HeapNode() : HeapNode(cv::Point(-1,-1), 0, 0, 0){}
//	HeapNode(float f = 0, float g = 0, float h = 0, cv::Point point = cv::Point(-1, -1)) {
//		F_distance = f;
//		G_distance = g;
//		H_distance = h;
//		this->point = point;
//		parent_node = NULL;
//	}
//	void setKey(cv::Point point, float f, float g, float h, HeapNode* p_node) { 
//		this->point = point; F_distance = f; G_distance = g; H_distance = h; parent_node = p_node;
//	}
//	float getKey() { return F_distance; }
//	float get_G_distance() { return G_distance; }
//	cv::Point getPoint() { return point; }
//	HeapNode* get_parent_node() { return parent_node; }
//	void display() { std::cout << F_distance; }
//};

//HeapNode_ver3
class HeapNode
{
	Node* node;
public:
	/*HeapNode(cv::Point point = cv::Point(-1, -1), float f = 0, float g = 0, float h = 0) :
		point(point), F_distance(f), G_distance(g), H_distance(h) { parent_node = NULL; }*/
		//HeapNode() : HeapNode(cv::Point(-1,-1), 0, 0, 0){}
	HeapNode(Node* node = NULL) : node(node) {}
	void setKey_node(Node* node) {
		this->node = node;
	}
	Node* getKey_node() { return node; }
	//void display() { std::cout << F_distance; }
};

#endif