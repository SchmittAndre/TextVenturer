#include "stdafx.h"

#include "Game.h"

Game::Game()
{
    textShader = new Shader();
    textShader->loadVertFragShader("shader/test");
    textShader->addAttribute(2, "vpos");
    textShader->addAttribute(4, "vcolor");
   
    textDisplay = new TextDisplay(textShader, 60, 25);         
}    

Game::~Game()
{
    delete textShader;
    delete textDisplay;
}

void Game::update()
{
    textDisplay->update();
}

void Game::render()
{
    textDisplay->render();
}

void Game::resize(int width, int height)
{
    glUniform1f(textShader->getUniformLocation("aspect"), (float)width / height);
}
