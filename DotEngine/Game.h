#pragma once
#include <vector>
#include "Bitmap.h"
#include "Dot.h"

static const int SCREEN_WIDTH = 1600;
static const int SCREEN_HEIGHT = 1600;

class DotRenderer;

class Game
{
public:
	Game(DotRenderer* aRenderer);
	void Update(float aDeltaTime);
	void CleanUp();
private:
	DotRenderer* renderer;
	std::vector<Dot*> OnScreenDots;
	//Bitmap* dotBitmap;
};

