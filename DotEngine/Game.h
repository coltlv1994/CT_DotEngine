#pragma once
#include <vector>
#include "Bitmap.h"
#include "Dot.h"
#include "Quadtree.h"

static const int SCREEN_WIDTH = 1600;
static const int SCREEN_HEIGHT = 1600;

class DotRenderer;

class Game
{
public:
	Game(DotRenderer* aRenderer);
	~Game();
	void Update(float aDeltaTime);
	void CleanUp();
private:
	DotRenderer* renderer;
	std::vector<Dot*> m_onScreenDots;
	Quadtree* m_qt = nullptr;
	//Bitmap* dotBitmap;
};

