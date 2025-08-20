#pragma once
#include "Dot.h"
#include <unordered_set>
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
	Node(int p_layer, float X_UL, float X_DR, float Y_UL, float Y_DR, Node* p_parent = nullptr);
	~Node();

	void Insert(Dot* p_dotToInsert);
	bool IsDotWithinBoundary(Dot* p_dot);
	Direction DetermineQuadrant(Dot* p_dot);
	Node* Split(Direction p_quad);
	void BoundaryInsert(Dot* p_dotToInsert); // this dot is on boundary of children, insert it to this node

	// collsion detection related
	void _checkCollision(std::unordered_set<int>& p_collidedDotIndex);
	void _populateNodeList(std::vector<Node*>& p_nodeList);
	void _addDataToChild(std::vector<Dot*>& p_childData);
	void _checkValid(std::unordered_set<Dot*>& p_invalidDots);
	void _reinsert(Dot* p_dotToInsert); // this should check if the dot fits boundary
	void _remove(Dot* p_dotToRemove);

private:
	int m_layer;
	bool m_hadChild = false;
	Node* m_parent = nullptr;
	Node* m_children[MAX_CHILDREN];
	std::vector<Dot*> m_data; // stores dots that sits on boundary, or this node cannot be splitted further
	float bound_X_UpLeft, bound_X_DownRight, bound_Y_UpLeft, bound_Y_DownRight;
	float X_HALF, Y_HALF; // for convenience of further calculation
};

class Quadtree
{
public:
	Quadtree(float X_MAX, float Y_MAX, const std::vector<Dot*> *p_dots, unsigned int p_noOfThreads);
	~Quadtree();
	void Populate();
	void CheckCollision(std::unordered_set<int>& p_collidedDotIndex);
	void CheckAndMoveInvalid();
	void Remove(Dot* p_dotToRemove);
	void Insert(Dot* p_dotToInsert);

private:
	Node* m_rootNode = nullptr;
	const std::vector<Dot*> *m_dots = nullptr;
	unsigned int m_noOfThreads;
};

void _threadSubtaskCheckCollision(std::vector<Node*>& p_nodeList, std::unordered_set<int>& p_resultList);
