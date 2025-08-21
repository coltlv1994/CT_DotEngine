#include "Game.h"
#include "DotRenderer.h"
#include "Dot.h"
#include <cstdlib>
#include "glm/glm.hpp"
#include <unordered_set>
#include "Quadtree.h"
#include <thread>

const int DotAmount = 4000;

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

	unsigned int noOfHardwareThread = std::thread::hardware_concurrency();

	if (m_noOfThreads > noOfHardwareThread)
	{
		// reduce number of threads to hardware supported number
		m_noOfThreads = noOfHardwareThread;
	}
}

void Game::Update(float aDeltaTime)
{
	dotsToReset.clear();

	Quadtree qt = Quadtree(SCREEN_WIDTH, SCREEN_HEIGHT, &OnScreenDots, m_noOfThreads);
	qt.Populate();
	qt.CheckCollision(dotsToReset);

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
