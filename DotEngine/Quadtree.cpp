#include "Quadtree.h"
#include "glm/glm.hpp"

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

void Node::Insert(Dot* p_dotToInsert)
{
	if (m_layer == MAX_LAYER)
	{
		// cannot split any further
		m_data.push_back(p_dotToInsert);
		return;
	}
	else
	{
		// check if dot is on the middle that X_HALF and Y_HALF will penetrate
		float dotX = p_dotToInsert->position.x;
		float dotY = p_dotToInsert->position.y;
		float dotRadius = p_dotToInsert->Radius;
		float X_HALF_MINUS_RADIUS = X_HALF - dotRadius;
		float X_HALF_PLUS_RADIUS = X_HALF + dotRadius;
		float Y_HALF_MINUS_RADIUS = Y_HALF - dotRadius;
		float Y_HALF_PLUS_RADIUS = Y_HALF + dotRadius;

		// boundary check
		if ((dotX > X_HALF_MINUS_RADIUS && dotX < X_HALF_PLUS_RADIUS) || (dotY > Y_HALF_MINUS_RADIUS && dotY < Y_HALF_PLUS_RADIUS))
		{
			m_data.push_back(p_dotToInsert);
			return;
		}

		// can split
		Direction quad = DetermineQuadrant(p_dotToInsert);

		if (m_children[quad] == nullptr)
		{
			// not splitted
			m_children[quad] = Split(quad);
		}

		m_children[quad]->Insert(p_dotToInsert);
	}
}

bool Node::IsDotWithinBoundary(Dot* p_dot)
{
	float dotX = p_dot->position.x;
	float dotY = p_dot->position.y;

	if (dotX > bound_X_UpLeft && dotX < bound_X_DownRight && dotY > bound_Y_UpLeft && dotY < bound_Y_DownRight)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Direction Node::DetermineQuadrant(Dot* p_dot)
{
	float dotX = p_dot->position.x;
	float dotY = p_dot->position.y;

	if (dotX < X_HALF)
	{
		if (dotY < Y_HALF)
		{
			return Direction::UpLeft;
		}
		else
		{
			return Direction::DownLeft;
		}
	}
	else
	{
		if (dotY < Y_HALF)
		{
			return Direction::UpRight;
		}
		else
		{
			return Direction::DownRight;
		}
	}
}

Node* Node::Split(Direction p_quad)
{
	switch (p_quad)
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

void Node::BoundaryInsert(Dot* p_dotToInsert)
{
	// this dot is on boundary of children, insert it to this node
	m_data.push_back(p_dotToInsert);
}

Node::~Node()
{
	for (int i = 0; i < MAX_CHILDREN; i++)
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

void Node::_checkCollision(std::vector<Node*>& p_nodeList, std::unordered_set<int>& p_collidedDotIndex)
{
	if (m_parent != nullptr)
	{
		// add parent's data
		m_parent->_addDataToChild(m_data);
	}

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

Quadtree::Quadtree(float X_MAX, float Y_MAX, const std::vector<Dot*> *p_dots)
{
	m_rootNode = new Node(1, 0, X_MAX, 0, X_MAX); // should be (1, 0, screen_X_max, 0, screen_Y_max)
	m_dots = p_dots;
}

Quadtree::~Quadtree()
{
	delete m_rootNode;
	m_rootNode = nullptr;
}

void Quadtree::Populate()
{
	for (auto dot : *m_dots)
	{
		m_rootNode->Insert(dot);
	}
}

void Quadtree::CheckCollision(std::unordered_set<int>& p_collidedDotIndex)
{
	std::vector<Node*> nodesToCheck;
	m_rootNode->_populateNodeList(nodesToCheck);

	// can be multithreaded later
	for (auto node_p : nodesToCheck)
	{
		node_p->_checkCollision(nodesToCheck, p_collidedDotIndex);
	}
}