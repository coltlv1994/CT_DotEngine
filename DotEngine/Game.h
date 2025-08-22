#pragma once
#include <vector>
#include <unordered_set>
#include <thread>
#include "Bitmap.h"
#include "Dot.h"
#include "SDL3/SDL.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

class DotRenderer;

class Game
{
public:
	Game(DotRenderer* aRenderer, int p_dotAmount);
	~Game();
	void Update(float aDeltaTime);
	void CleanUp();
	void RenderPartition(std::vector<Dot*> &p_dots, float p_deltaTime);
private:
	DotRenderer* renderer;
	std::vector<Dot*> OnScreenDots;
	unsigned int m_noOfThreads = 8; //default will use 8 logical cores
	std::unordered_set<int> dotsToReset;
	SDL_Texture* m_screenTexture;
	uint32_t* m_pixelBuffer;
	size_t m_pixelBufferSizeInByte;
	int m_dotsPerRenderThread;
	std::vector<std::vector<Dot*>> m_dotRenderTask;
	std::vector<std::thread> m_pixelBufferRenderThreads;
	int m_dotAmount;
};

