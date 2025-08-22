#include "Quadtree.h"
#include "glm/glm.hpp"
#include <thread>

// m[downright][upleft]
static const int insertQuadrant[4][4] = { {0x01, 0x03, 0x10, 0x10},
									 {0x10, 0x02, 0x10, 0x10},
									 {0x05, 0x0F, 0x04, 0x0C},
									 {0x10, 0x0A, 0x10, 0x08} };

Node::Node(int p_layer, float X_UL, float X_DR, float Y_UL, float Y_DR, Node* p_parent)
{
	m_layer = p_layer;
	bound_X_DownRight = X_DR;
	bound_X_UpLeft = X_UL;
	bound_Y_DownRight = Y_DR;
	bound_Y_UpLeft = Y_UL;

	X_HALF = (X_DR + X_UL) / 2.0;
	Y_HALF = (Y_DR + Y_UL) / 2.0;

	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		m_children[i] = nullptr;
	}

	m_parent = p_parent;
}

void Node::InsertDots(std::vector<Dot*>& p_dots)
{
	if (m_layer == MAX_LAYER)
	{
		// cannot split any further
		m_data.insert(m_data.end(), p_dots.begin(), p_dots.end());
	}
	else
	{
		// this could be splitted further
		std::array<std::vector<Dot*>, MAX_CHILDREN> dividedDots; // default initialized

		_fillQuadrantList(p_dots, dividedDots);

		for (int i = 0; i < MAX_CHILDREN; i++)
		{
			if (dividedDots[i].size() != 0)
			{
				// this quad needs to be filled
				if (m_children[i] == nullptr)
				{
					// not splitted
					m_children[i] = Split(i);
				}

				m_children[i]->InsertDots(dividedDots[i]);
			}
		}
	}
}

Direction Node::_checkQuadrant(float posX, float posY)
{
	if (posX > X_HALF)
	{
		if (posY > Y_HALF)
		{
			return Direction::DownRight;
		}
		else
		{
			return Direction::UpRight;
		}
	}
	else
	{
		if (posY > Y_HALF)
		{
			return Direction::DownLeft;
		}
		else
		{
			return Direction::UpLeft;
		}
	}
}

Node* Node::Split(int quad)
{
	switch (quad)
	{
	case Direction::DownLeft:
		return new Node(m_layer + 1, bound_X_UpLeft, X_HALF, Y_HALF, bound_Y_DownRight, this);
	case Direction::DownRight:
		return new Node(m_layer + 1, X_HALF, bound_X_DownRight, Y_HALF, bound_Y_DownRight, this);
	case Direction::UpLeft:
		return new Node(m_layer + 1, bound_X_UpLeft, X_HALF, bound_Y_UpLeft, Y_HALF, this);
	case Direction::UpRight:
		return new Node(m_layer + 1, X_HALF, bound_X_DownRight, bound_Y_UpLeft, Y_HALF, this);
	default:
		return nullptr;
	}
}

Node::~Node()
{
	for (int i = 0; i != MAX_CHILDREN; i++)
	{
		if (m_children[i] != nullptr)
		{
			delete m_children[i];
			m_children[i] = nullptr;
		}
	}
}


void Node::_populateNodeList(std::vector<Node*>& p_nodeList)
{
	if (m_layer == MAX_LAYER)
	{
		// reached the last layer of tree
		p_nodeList.push_back(this);
		return;
	}

	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		if (m_children[i] != nullptr)
		{
			m_children[i]->_populateNodeList(p_nodeList);
		}
	}
}

void Node::_checkCollision(std::unordered_set<int>& p_collidedDotIndex)
{
	size_t dataSize = m_data.size();
	for (int i = 0; i < dataSize; i++)
	{
		Dot* d1 = m_data[i];
		if (p_collidedDotIndex.contains(d1->dotIndex))
		{
			// no need to further check this dot
			continue;
		}

		for (int j = i + 1; j < dataSize; j++)
		{
			Dot* d2 = m_data[j];

			if (d1 != d2)
			{
				// check collision
				float dist = glm::distance(d1->position, d2->position);
				float minDist = d1->Radius + d2->Radius;

				if (dist < minDist)
				{
					glm::vec2 normal = glm::normalize(d2->position - d1->position);

					d1->velocity = glm::reflect(d1->velocity, normal);
					d2->velocity = glm::reflect(d2->velocity, -normal);

					float overlap1 = 1.5f * ((minDist + 1) - dist);
					float overlap2 = 1.5f * (minDist - dist);
					d1->position -= normal * overlap1;
					d2->position += normal * overlap2;
					d1->TakeDamage(1);
					d1->Radius++;
					d2->TakeDamage(1);
					d2->Radius++;
				}

				if (d2->health <= 0)
				{
					p_collidedDotIndex.insert(d2->dotIndex);
				}

				if (d1->health <= 0)
				{
					p_collidedDotIndex.insert(d1->dotIndex);
					break; // d2 pointer should move to next dot
				}
			}
		}
	}
}

void Node::_addDataToChild(std::vector<Dot*>& p_childData)
{
	for (auto dot : m_data)
	{
		p_childData.push_back(dot);
	}
}

void Node::_fillQuadrantList(std::vector<Dot*>& p_dots, std::array<std::vector<Dot*>, 4>& dividedDots)
{
	for (auto dot_p : p_dots)
	{
		// check if dot is on the middle that X_HALF and Y_HALF will penetrate
		float dotX = dot_p->position.x;
		float dotY = dot_p->position.y;
		float dotRadius = dot_p->Radius;

		float dotXUpLeft = dotX - dotRadius;
		float dotYUpLeft = dotY - dotRadius;
		float dotXDownRight = dotX + dotRadius;
		float dotYDownRight = dotY + dotRadius;

		Direction upleftQuadrant = _checkQuadrant(dotXUpLeft, dotYUpLeft);
		Direction downrightQuadrant = _checkQuadrant(dotXDownRight, dotYDownRight);

		int action = insertQuadrant[downrightQuadrant][upleftQuadrant];

		switch (action)
		{
			// very ugly but action is actually bitmap
		case 15:
			dividedDots[Direction::DownLeft].push_back(dot_p);
			dividedDots[Direction::DownRight].push_back(dot_p);
		case 3:
			dividedDots[Direction::UpLeft].push_back(dot_p);
		case 1:
			dividedDots[Direction::UpRight].push_back(dot_p);
			break;

		case 10:
			dividedDots[Direction::DownLeft].push_back(dot_p);
		case 2:
			dividedDots[Direction::UpLeft].push_back(dot_p);
			break;

		case 12:
			dividedDots[Direction::DownLeft].push_back(dot_p);
		case 4:
			dividedDots[Direction::DownRight].push_back(dot_p);
			break;

		case 5:
			dividedDots[Direction::UpRight].push_back(dot_p);
		case 8:
			dividedDots[Direction::DownRight].push_back(dot_p);
			break;

		default: continue;
		}
	}
}

Quadtree::Quadtree(float X_MAX, float Y_MAX, std::vector<Dot*>* p_dots, unsigned int p_noOfThreads)
{
	m_rootNode = new Node(0, 0, X_MAX, 0, X_MAX); // should be (0, 0, screen_X_max, 0, screen_Y_max)
	m_dots = p_dots;
	m_noOfThreads = p_noOfThreads;
}

Quadtree::~Quadtree()
{
	delete m_rootNode;
	m_rootNode = nullptr;
}

void Quadtree::Populate()
{
	m_rootNode->InsertDots(*m_dots);
}

void Quadtree::CheckCollision(std::unordered_set<int>& p_collidedDotIndex)
{
	std::vector<Node*> nodesToCheck;
	m_rootNode->_populateNodeList(nodesToCheck);

	// divide task for threads
	std::vector<std::vector<Node*>> threadTaskPool;
	std::vector<std::unordered_set<int>> threadResultPool;
	std::vector<std::thread> threads;

	int threadIndex = 0;

	for (threadIndex = 0; threadIndex < m_noOfThreads; threadIndex++)
	{
		threadTaskPool.push_back(std::vector<Node*>());
		threadResultPool.push_back(std::unordered_set<int>());
	}

	threadIndex = 0;

	for (auto node_p : nodesToCheck)
	{
		(threadTaskPool[threadIndex]).push_back(node_p);
		threadIndex += 1;
		threadIndex = threadIndex % m_noOfThreads;
	}

	for (threadIndex = 0; threadIndex < m_noOfThreads; threadIndex++)
	{
		threads.push_back(std::thread(&Quadtree::_threadSubtaskCheckCollision, this, std::ref(threadTaskPool[threadIndex]), std::ref(threadResultPool[threadIndex])));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}

	for (auto threadResult : threadResultPool)
	{
		for (auto dotIndex : threadResult)
		{
			p_collidedDotIndex.insert(dotIndex);
		}
	}
}

void Quadtree::_threadSubtaskCheckCollision(std::vector<Node*>& p_nodeList, std::unordered_set<int>& p_resultList)
{
	for (auto node_p : p_nodeList)
	{
		node_p->_checkCollision(p_resultList);
	}
}