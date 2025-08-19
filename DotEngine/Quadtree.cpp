#include "Quadtree.h"
#include "glm/glm.hpp"

Node::Node(int p_layer, float X_UL, float X_DR, float Y_UL, float Y_DR)
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
}

void Node::Insert(Dot* p_dotToInsert)
{
	// determine which quarter should this dot belong
	
	if (m_layer == MAX_LAYER)
	{
		// cannot split any further
		m_data.insert(p_dotToInsert);
		return;
	}
	else
	{
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
	if (p_dot->position.x < X_HALF)
	{
		if (p_dot->position.y < Y_HALF)
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
		if (p_dot->position.y < Y_HALF)
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
		return new Node(m_layer + 1, bound_X_UpLeft, X_HALF, Y_HALF, bound_Y_DownRight);
	case Direction::DownRight:
		return new Node(m_layer + 1, X_HALF, bound_X_DownRight, Y_HALF, bound_Y_DownRight);
	case Direction::UpLeft:
		return new Node(m_layer + 1, bound_X_UpLeft, X_HALF, bound_Y_UpLeft, Y_HALF);
	case Direction::UpRight:
		return new Node(m_layer + 1, X_HALF, bound_X_DownRight, bound_Y_UpLeft, Y_HALF);
	default:
		return nullptr;
	}
}

Quadtree::Quadtree(float X_MAX, float Y_MAX)
{
	m_rootNode = new Node(1, 0, X_MAX, 0, X_MAX); // should be (1, 0, screen_X_max, 0, screen_Y_max)
}

void Quadtree::Populate(const std::vector<Dot*>& p_dots)
{
	for (auto dot : p_dots)
	{
		m_rootNode->Insert(dot);
	}
}