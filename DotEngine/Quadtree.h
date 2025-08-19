#pragma once
#include "Dot.h"
#include <set>
#include <vector>

constexpr int MAX_CHILDREN = 4;
constexpr int MAX_LAYER = 3; // root node is layer 1

enum Direction : int {
	UpRight,
	UpLeft,
	DownRight,
	DownLeft
};

class Node
{
public:
	Node(int p_layer, float X_UL, float X_DR, float Y_UL, float Y_DR);
	void Insert(Dot* p_dotToInsert);
	bool IsDotWithinBoundary(Dot* p_dot);
	Direction DetermineQuadrant(Dot* p_dot);
	Node* Split(Direction p_quad);

private:
	int m_layer; // starts from 1 (top-est)
	bool m_hadChild = false;
	Node* m_parent = nullptr;
	Node* m_children[MAX_CHILDREN];
	std::set<Dot*> m_data; // stores dots that sits on boundary, or this node cannot be splitted further
	float bound_X_UpLeft, bound_X_DownRight, bound_Y_UpLeft, bound_Y_DownRight;
	float X_HALF, Y_HALF; // for convenience of further calculation
};

class Quadtree
{
public:
	Quadtree(float X_MAX, float Y_MAX);
	void Populate(const std::vector<Dot*> &p_dots);

private:
	Node* m_rootNode = nullptr;
};

