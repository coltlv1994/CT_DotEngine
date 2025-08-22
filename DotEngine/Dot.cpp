#include "Dot.h"
#include "DotRenderer.h"
#include "Game.h"
#include <random>
#include <glm/gtc/constants.hpp>
#include <cstdlib>
#include <algorithm>

const float DotVelocity = 50.0f;

Dot::Dot(glm::vec2 aPosition, int aRadius, int p_dotIndex)
{
	position = aPosition;
	startPos = aPosition;
	Radius = aRadius;

	static std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
	std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

	velocity = glm::vec2(dist(rng), dist(rng));

	float angle = dist(rng) * glm::pi<float>() / 100.0f;
	velocity = glm::vec2(cos(angle), sin(angle));

	health = 3;

	dotIndex = p_dotIndex;
}

void Dot::RenderPixelBuffer(float p_deltaTime, uint32_t* p_pixelBuffer)
{
	totalTime += p_deltaTime;

	position += velocity * DotVelocity * p_deltaTime;

	float redColor = (glm::cos((totalTime + startPos.x) * 0.1f) * 0.5f + 0.5f) * 255.0f;

	float greenColor = (glm::cos((totalTime + startPos.y) * 0.9f) * 0.5f + 0.5f) * 255.0f;

	float blueColor = (glm::cos(totalTime * 0.4f) * 0.5f + 0.5f) * 255.0f;

	uint32_t color = ConvertColor(redColor, greenColor, blueColor);

	int posX = (int)(position.x + 0.5f);
	int posY = (int)(position.y + 0.5f);

	for (int y = 0; y <= Radius; y++)
	{
		int lineYTop = posY - y;
		int lineYBottom = posY + y;

		if (lineYTop > SCREEN_HEIGHT - 1 || lineYBottom < 0)
		{
			// out-of-screen dot
			continue;
		}

		int x = static_cast<int>(std::sqrt(Radius * Radius - y * y));

		int startX = posX - x;
		int endX = posX + x;

		if (startX > SCREEN_WIDTH - 1 || endX < 0)
		{
			continue;
		}

		if (startX < 0)
		{
			startX = 0;
		}

		if (endX > SCREEN_WIDTH - 1)
		{
			endX = SCREEN_WIDTH - 1;
		}

		if (lineYBottom < SCREEN_HEIGHT)
		{
			std::fill(&(p_pixelBuffer[lineYBottom * SCREEN_WIDTH + startX]), &(p_pixelBuffer[lineYBottom * SCREEN_WIDTH + endX + 1]), color);
		}

		if (lineYTop >= 0)
		{
			std::fill(&(p_pixelBuffer[lineYTop * SCREEN_WIDTH + startX]), &(p_pixelBuffer[lineYTop * SCREEN_WIDTH + endX + 1]), color);
		}
	}

	if (position.x - Radius < 0.0f)
	{
		position.x = Radius;
		velocity.x *= -1;
	}
	else if (position.x + Radius > SCREEN_WIDTH)
	{
		position.x = SCREEN_WIDTH - Radius;
		velocity.x *= -1;
	}

	if (position.y - Radius < 0.0f)
	{
		position.y = Radius;
		velocity.y *= -1;
	}
	else if (position.y + Radius > SCREEN_HEIGHT)
	{
		position.y = SCREEN_HEIGHT - Radius;
		velocity.y *= -1;
	}
}

uint32_t Dot::ConvertColor(float p_red, float p_green, float p_blue)
{
	return
		((static_cast<uint32_t>(p_red) << 24) |
			(static_cast<uint32_t>(p_green) << 16) |
			(static_cast<uint32_t>(p_blue) << 8) |
			0x000000FF);
}

void Dot::TakeDamage(int someDamage)
{
	health -= someDamage;
}

void Dot::ResetDot(glm::vec2 aPosition, int aRadius)
{
	position = aPosition;
	startPos = aPosition;
	Radius = aRadius;

	static std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
	std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

	velocity = glm::vec2(dist(rng), dist(rng));

	float angle = dist(rng) * glm::pi<float>() / 100.0f;
	velocity = glm::vec2(cos(angle), sin(angle));

	health = 3;
}



