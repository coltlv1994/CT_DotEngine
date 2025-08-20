#include "Game.h"
#include "DotRenderer.h"
#include "Dot.h"
#include <cstdlib>
#include "glm/glm.hpp"
#include <unordered_set>
#include "Quadtree.h"


std::vector<Dot*> dots;
std::unordered_set<int> dotsToReset;

const int DotAmount = 1000;

Game::Game(DotRenderer* aRenderer)
{
	renderer = aRenderer;

	for (size_t i = 0; i < DotAmount; i++)
	{
		int diry = std::rand() % 2;
		int dirx = std::rand() % 2;

		dirx = -1 ? dirx > 1 : dirx;
		diry = -1 ? diry > 1 : diry;

		Dot* d = new Dot({ std::rand() % SCREEN_WIDTH, std::rand() % SCREEN_HEIGHT }, 3, i);

		OnScreenDots.push_back(d);
	}

	//To debug collision
	OnScreenDots[0]->overriden = true;
	OnScreenDots[0]->Radius = 10;
	//To debug collision
}

void Game::Update(float aDeltaTime)
{
	dotsToReset.clear();

	Quadtree qt = Quadtree(SCREEN_WIDTH, SCREEN_HEIGHT, &OnScreenDots);
	qt.Populate();
	qt.CheckCollision(dotsToReset);

	// for (int dotIndex_1 = 0; dotIndex_1 < DotAmount; dotIndex_1++)
	// {
	// 	if (dotsToReset.contains(dotIndex_1))
	// 	{
	// 		// no need to further check this dot
	// 		continue;
	// 	}

	// 	Dot* d1 = OnScreenDots[dotIndex_1];

	// 	for (int dotIndex_2 = dotIndex_1 + 1; dotIndex_2 < DotAmount; dotIndex_2++)
	// 	{
	// 		if (dotsToReset.contains(dotIndex_2))
	// 		{
	// 			// no need to further check this dot
	// 			continue;
	// 		}

	// 		Dot* d2 = OnScreenDots[dotIndex_2];

	// 		float dist = glm::distance(d1->position, d2->position);
	// 		float minDist = d1->Radius + d2->Radius;

	// 		if (dist < minDist)
	// 		{
	// 			glm::vec2 normal = glm::normalize(d2->position - d1->position);

	// 			d1->velocity = glm::reflect(d1->velocity, normal);
	// 			d2->velocity = glm::reflect(d2->velocity, -normal);

	// 			float overlap1 = 1.5f * ((minDist + 1) - dist);
	// 			float overlap2 = 1.5f * (minDist - dist);
	// 			d1->position -= normal * overlap1;
	// 			d2->position += normal * overlap2;
	// 			d1->TakeDamage(1);
	// 			d1->Radius++;
	// 			d2->TakeDamage(1);
	// 			d2->Radius++;
	// 		}

	// 		if (d2->health <= 0)
	// 		{
	// 			dotsToReset.insert(dotIndex_2);
	// 		}

	// 		if (d1->health <= 0)
	// 		{
	// 			dotsToReset.insert(dotIndex_1);
	// 			break; // d2 pointer should move to next dot
	// 		}
	// 	}
	// }

	for (auto dotIndex : dotsToReset)
	{
		int diry = std::rand() % 2;
		int dirx = std::rand() % 2;

		dirx = -1 ? dirx > 1 : dirx;
		diry = -1 ? diry > 1 : diry;

		OnScreenDots[dotIndex]->ResetDot({ std::rand() % SCREEN_WIDTH, std::rand() % SCREEN_HEIGHT }, 3);
	}

	for (auto dot : OnScreenDots)
	{
		dot->Render(renderer, aDeltaTime);
	}
}

void Game::CleanUp()
{

}
