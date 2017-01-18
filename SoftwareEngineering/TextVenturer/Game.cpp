#include "stdafx.h"
#include "TextDisplay.h"
#include "Controler.h"
#include "Window.h"

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
   
    font = new BMPFont();
    font->loadFromPNG("data/font/font.png");

    //textDisplay = new TextDisplay(textShader, font, 20, 11, GLWindow::aspect);
    //textDisplay = new TextDisplay(textShader, font, 40, 22, GLWindow::aspect);
    textDisplay = new TextDisplay(textShader, font, 60, 33, GLWindow::aspect);

    controler = new Controler(textDisplay,this);

    window->setSamples(window->getMaxSamples());                                      
    if (window->isMultisampled())
        controler->writeLine("$delay(0)$yellow()DEBUG: $light_gray()Multisampling: $lime()" + to_string(window->getSamples()));
    else
        controler->writeLine("$delay(0)$yellow()DEBUG: $light_gray()Multisampling: $red()not supported");

    controler->DEBUG_startAdventure();

    QueryPerformanceFrequency(&frequency);
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
    glUniform1f(textShader->getUniformLocation("aspect"), (float)width / height);
}

void Game::pressChar(byte c) const
{                             
    controler->pressChar(c);
}

void Game::pressKey(byte key) const
{
    controler->pressKey(key);
}

GLWindow* Game::getWindow() const
{
	return window;
}