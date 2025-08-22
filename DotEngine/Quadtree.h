#pragma once
#include "Dot.h"
#include <unordered_set>
#include <vector>
#include <array>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

constexpr int MAX_CHILDREN = 4;
constexpr int MAX_LAYER = 4; // root node is layer 0

enum Direction : int {
	UpRight, // 0
	UpLeft, // 1
	DownRight, // 2
	DownLeft //3
};

class Node
{
public:
	Node(int p_layer, float X_UL, float X_DR, float Y_UL, float Y_DR);
	~Node();

	void InsertDots(std::vector<Dot*>& p_dots);
	Node* Split(int p_quad);

	// collsion detection related
	void _checkCollision(std::unordered_set<int>& p_collidedDotIndex);
	void _populateNodeList(std::vector<Node*>& p_nodeList);
	Direction _checkQuadrant(float posX, float posY);
	void _fillQuadrantList(std::vector<Dot*>& p_dots, std::array<std::vector<Dot*>, 4>& dividedDots);

private:
	int m_layer;
	Node* m_children[MAX_CHILDREN];
	std::vector<Dot*> m_data; // stores dots that sits on boundary, or this node cannot be splitted further
	float bound_X_UpLeft, bound_X_DownRight, bound_Y_UpLeft, bound_Y_DownRight;
	float X_HALF, Y_HALF; // for convenience of further calculation
};

class Quadtree
{
public:
	Quadtree(float X_MAX, float Y_MAX, std::vector<Dot*> *p_dots, unsigned int p_noOfThreads);
	~Quadtree();
	void Populate();
	void CheckCollision(std::unordered_set<int>& p_collidedDotIndex);
	void _threadSubtaskCheckCollision(std::vector<Node*>& p_nodeList, std::unordered_set<int>& p_resultList);

private:
	Node* m_rootNode = nullptr;
	std::vector<Dot*> *m_dots = nullptr;
	unsigned int m_noOfThreads;
};
