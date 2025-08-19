#pragma once
#include "glm/glm.hpp"

class DotRenderer;

class Dot
{
public:

	Dot(glm::vec2 aPosition, float aRadius, int p_dotIndex);
	void Render(DotRenderer* aRenderer, float dt);
	void TakeDamage(int someDamage);
	void ResetDot(glm::vec2 aPosition, float aRadius);

	glm::vec2 position;
	glm::vec2 startPos;
	glm::vec2 velocity;

	float totalTime = 0;
	float Radius = 0;

	int health;

	int dotIndex;

	bool overriden = false;
};

