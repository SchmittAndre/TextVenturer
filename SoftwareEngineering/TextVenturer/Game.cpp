#include "stdafx.h"

#include "Window.h"
#include "TextDisplay.h"
#include "Controler.h"

#include "Game.h" 

void Game::updateDeltaTime()
{
    LARGE_INTEGER newTime;
    QueryPerformanceCounter(&newTime);
    deltaTime = (float)(newTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
    lastTime = newTime;
}

Game::Game(GLWindow* w)
{
	window = w;
    
    textShader = new Shader();
    textShader->loadVertFragShader("data/shader/test");
    textShader->addAttribute(2, "vpos");
    textShader->addAttribute(2, "vtexcoord");
    textShader->addAttribute(4, "vcolor");
    textShader->addAttribute(2, "vborderlow");
    textShader->addAttribute(2, "vborderhigh");   

    font = new BMPFont();
    font->loadFromPNG("data/font/font.png");
    
    textDisplay = new TextDisplay(textShader, font, 60, 33, GLWindow::defaultAspect);
    
    controler = new Controler(this, textDisplay);

    window->setSamples(window->getMaxSamples());
    window->setVSync(true);

    fpsUpdate = 0;
    fps = 0;

    QueryPerformanceFrequency(&frequency);
    
    LARGE_INTEGER seed;
    QueryPerformanceCounter(&seed);
    srand((DWORD)seed.QuadPart);                          
    updateDeltaTime();     
}    

Game::~Game()
{
    delete controler;
    delete textDisplay;
    delete font;
    delete textShader;
}

void Game::update()
{
    updateDeltaTime();

    fpsUpdate -= deltaTime;
    fps = fps * 0.9f + getRawFPS() * 0.1f;
    if (fpsUpdate <= 0)
    {
        window->setCaption("TextVenturer - FPS: " + std::to_string((int)floor(fps + 0.5f)));
        fpsUpdate = 0.5;
    }

    controler->update(deltaTime);
    textDisplay->update(deltaTime);
}

void Game::render() const
{
    textDisplay->render();
}

void Game::resize(int width, int height)
{
    textShader->enable();                                  
    glUniform1f(textShader->getUniformLocation("aspect"), window->getAspect());
    glUniform1f(textShader->getUniformLocation("scale"), window->getScale());
}

void Game::pressChar(byte c) const
{                  
    controler->pressChar(c);
}

void Game::pressKey(byte key) const
{
    controler->pressKey(key);
}

float Game::getRawFPS() const
{
    return 1.0f / deltaTime;
}

GLWindow* Game::getWindow() const
{
	return window;
}