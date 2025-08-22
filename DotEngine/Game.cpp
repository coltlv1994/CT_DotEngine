#include "Game.h"
#include "DotRenderer.h"
#include "Dot.h"
#include <cstdlib>
#include "glm/glm.hpp"
#include <unordered_set>
#include "Quadtree.h"
#include <thread>

Game::Game(DotRenderer* aRenderer, int p_dotAmount)
{
	renderer = aRenderer;

	m_dotAmount = p_dotAmount;

	m_pixelBuffer = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
	m_pixelBufferSizeInByte = sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT;
	memset(m_pixelBuffer, 0, m_pixelBufferSizeInByte);

	m_screenTexture = SDL_CreateTexture(
		renderer->GetSDLRenderer(),
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH,
		SCREEN_HEIGHT);

	for (size_t i = 0; i < m_dotAmount; i++)
	{
		int diry = std::rand() % 2;
		int dirx = std::rand() % 2;

		dirx = -1 ? dirx > 1 : dirx;
		diry = -1 ? diry > 1 : diry;

		Dot* d = new Dot({ std::rand() % SCREEN_WIDTH, std::rand() % SCREEN_HEIGHT }, 3, i);

		OnScreenDots.push_back(d);
	}

	//To debug collision
	OnScreenDots[0]->Radius = 10;
	//To debug collision

	unsigned int noOfHardwareThread = std::thread::hardware_concurrency();

	if (m_noOfThreads > noOfHardwareThread)
	{
		// reduce number of threads to hardware supported number
		m_noOfThreads = noOfHardwareThread;
	}

	m_dotsPerRenderThread = (m_dotAmount + m_noOfThreads - 1) / m_noOfThreads;
}

void Game::Update(float aDeltaTime)
{
	std::unordered_set<int> dotsToReset;

	memset(m_pixelBuffer, 0, m_pixelBufferSizeInByte);
	m_pixelBufferRenderThreads.clear();

	Quadtree qt = Quadtree(SCREEN_WIDTH, SCREEN_HEIGHT, &OnScreenDots, m_noOfThreads);
	qt.Populate();
	qt.CheckCollision(dotsToReset);

	for (auto dotIndex : dotsToReset)
	{
		int diry = std::rand() % 2;
		int dirx = std::rand() % 2;

		dirx = 1 ? dirx > 1 : dirx;
		diry = 1 ? diry > 1 : diry;

		OnScreenDots[dotIndex]->ResetDot({ std::rand() % SCREEN_WIDTH, std::rand() % SCREEN_HEIGHT }, 3);
	}

	for (int i = 0; i < m_noOfThreads - 1; i++)
	{
		// render here
		m_pixelBufferRenderThreads.push_back(
			std::thread(
				&Game::RenderPartition,
				this,
				aDeltaTime,
				i * m_dotsPerRenderThread,
				(i + 1) * m_dotsPerRenderThread));
	}

	m_pixelBufferRenderThreads.push_back(
		std::thread(
			&Game::RenderPartition,
			this,
			aDeltaTime,
			(m_noOfThreads - 1) * m_dotsPerRenderThread,
			OnScreenDots.size()));

	for (auto& thd : m_pixelBufferRenderThreads)
	{
		thd.join();
	}

	SDL_UpdateTexture(m_screenTexture, NULL, m_pixelBuffer, SCREEN_WIDTH * sizeof(uint32_t));
	SDL_RenderTexture(renderer->GetSDLRenderer(), m_screenTexture, NULL, NULL);
}

void Game::RenderPartition(float p_deltaTime, const int start, const int end)
{
	for (int i = start; i < end; i++)
	{
		OnScreenDots[i]->RenderPixelBuffer(p_deltaTime, m_pixelBuffer);
	}
}

void Game::CleanUp()
{

}

Game::~Game()
{
	SDL_DestroyTexture(m_screenTexture);
	delete[] m_pixelBuffer;
}
