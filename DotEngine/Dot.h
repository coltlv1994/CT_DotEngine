#pragma once
#include "glm/glm.hpp"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

class DotRenderer;

class Dot
{
public:

	Dot(glm::vec2 aPosition, int aRadius, int p_dotIndex);
	void TakeDamage(int someDamage);
	void ResetDot(glm::vec2 aPosition, int aRadius);
	void RenderPixelBuffer(float p_deltaTime, uint32_t* p_pixelBuffer);
	uint32_t ConvertColor(float p_red, float p_green, float p_blue);

	glm::vec2 position;
	glm::vec2 startPos;
	glm::vec2 velocity;

	float totalTime = 0;
	int Radius = 0;

	int health;

	int dotIndex;
};

