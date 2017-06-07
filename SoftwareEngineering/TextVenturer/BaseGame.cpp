#include "stdafx.h"

#include "BaseGame.h"

void BaseGame::updateDeltaTime()
{
    LARGE_INTEGER newTime;
    QueryPerformanceCounter(&newTime);
    deltaTime = (float)(newTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
    lastTime = newTime;
}

BaseGame::BaseGame()
{
    QueryPerformanceFrequency(&frequency);
    fps = 0;
    updateDeltaTime();
}

float BaseGame::getDeltaTime() const
{
    return deltaTime;
}

float BaseGame::getFPS() const
{
    return fps;
}

float BaseGame::getRawFPS() const
{
    return 1.0f / deltaTime;
}

void BaseGame::update()
{
    updateDeltaTime();
    fps = fps * 0.9f + getRawFPS() * 0.1f;
    update(deltaTime);
}
