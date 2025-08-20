#include "Game.h"
#include "DotRenderer.h"
#include "Dot.h"
#include <cstdlib>
#include "glm/glm.hpp"
#include <unordered_set>
#include "Quadtree.h"
#include <thread>

#define USE_QUAD_TREE

std::vector<Dot*> dots;
std::unordered_set<int> dotsToReset;

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

		m_onScreenDots.push_back(d);
	}

	//To debug collision
	m_onScreenDots[0]->overriden = true;
	m_onScreenDots[0]->Radius = 10;
	//To debug collision

	unsigned int noOfHardwareThread = std::thread::hardware_concurrency();

	if (m_noOfThreads > noOfHardwareThread)
	{
		// reduce number of threads to hardware supported number
		m_noOfThreads = noOfHardwareThread;
	}

	// populate quadtree
	m_qt = new Quadtree(SCREEN_WIDTH, SCREEN_HEIGHT, &m_onScreenDots, m_noOfThreads);
	m_qt->Populate();
}

void Game::Update(float aDeltaTime)
{
	dotsToReset.clear();
	// check if all dots are still  in valid tree branch/boundary
	m_qt->CheckAndMoveInvalid();

	// dots that need to reset will be removed temporarily from the quadtree
	m_qt->CheckCollision(dotsToReset);

	for (auto dotIndex : dotsToReset)
	{
		// remove from tree first
		m_qt->Remove(m_onScreenDots[dotIndex]);
		int diry = std::rand() % 2;
		int dirx = std::rand() % 2;

		dirx = -1 ? dirx > 1 : dirx;
		diry = -1 ? diry > 1 : diry;

		m_onScreenDots[dotIndex]->ResetDot({ std::rand() % SCREEN_WIDTH, std::rand() % SCREEN_HEIGHT }, 3);

		// update the tree with reseted dots
		m_qt->Insert(m_onScreenDots[dotIndex]);
	}

	for (auto dot : m_onScreenDots)
	{
		dot->Render(renderer, aDeltaTime);
	}
}

void Game::CleanUp()
{
	
}

Game::~Game()
{
	delete m_qt;
}
