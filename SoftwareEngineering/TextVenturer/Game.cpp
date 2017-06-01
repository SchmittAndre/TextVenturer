#include "stdafx.h"

#include "Window.h"
#include "TextDisplay.h"
#include "Controler.h"

#include "Game.h" 

const Shader::Attributes Game::textShaderAttributes = {
    Shader::Attribute(2, "vpos"),
    Shader::Attribute(2, "vtexcoord"),
    Shader::Attribute(4, "vcolor"),
    Shader::Attribute(2, "vborderlow"),
    Shader::Attribute(2, "vborderhigh")
};

void Game::updateDeltaTime()
{
    LARGE_INTEGER newTime;
    QueryPerformanceCounter(&newTime);
    deltaTime = (float)(newTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
    lastTime = newTime;
}

Game::Game(GLWindow & window)
    : window(window)
    , textShader(textShaderAttributes, L"data/shader/test")
    , textDisplay(textShader, font, 60, 33, GLWindow::defaultAspect)
    , controler(*this, textDisplay)
    , font("data/font/font.png")
{    
    window.setSamples(window.getMaxSamples());
    window.setVSync(true);
    
    fpsUpdate = 0;
    fps = 0;

    QueryPerformanceFrequency(&frequency);
    
    LARGE_INTEGER seed;
    QueryPerformanceCounter(&seed);
    srand((DWORD)seed.QuadPart);                          
    updateDeltaTime();     
}    

void Game::update()
{
    updateDeltaTime();

    fpsUpdate -= deltaTime;
    fps = fps * 0.9f + getRawFPS() * 0.1f;
    if (fpsUpdate <= 0)
    {
        window.setCaption(L"TextVenturer - FPS: " + std::to_wstring((int)floor(fps + 0.5f)));
        fpsUpdate = 0.5;
    }

    controler.update(deltaTime);
    textDisplay.update(deltaTime);
}

void Game::render()
{
    textDisplay.render();
}

void Game::resize(int width, int height)
{
    textShader.enable();                                  
    glUniform1f(textShader.getUniformLocation("aspect"), window.getAspect());
    glUniform1f(textShader.getUniformLocation("scale"), window.getScale());
}

void Game::pressChar(byte c) 
{                  
    controler.pressChar(c);
}

void Game::pressKey(byte key)
{
    controler.pressKey(key);
}

float Game::getRawFPS() const
{
    return 1.0f / deltaTime;
}

GLWindow & Game::getWindow() const
{
	return window;
}