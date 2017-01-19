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

    controler = new Controler(textDisplay, this);

    window->setSamples(window->getMaxSamples());
    /*
    if (window->isMultisampled())
        controler->writeLine("$delay(0)$yellow()DEBUG: $light_gray()Multisampling: $lime()" + to_string(window->getSamples()));
    else
        controler->writeLine("$delay(0)$yellow()DEBUG: $light_gray()Multisampling: $red()not supported");
    */
    controler->write("$delay(0)$shaking_on()$rainbow(0.2)$rainbow_speed(-6)$rgb(1,0.2,0.2)I don't really now what I should do to test this anymore... Oh well, I still got a lot of time to think about it, don't I?");
    window->setVSync(false);

    controler->DEBUG_startAdventure();

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
        window->setCaption("FPS: " + to_string((int)floor(fps + 0.5f)));
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

float Game::getRawFPS()
{
    return 1.0f / deltaTime;
}

GLWindow* Game::getWindow() const
{
	return window;
}