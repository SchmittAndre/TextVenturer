#include "stdafx.h"

#include "Game.h"

Game::Game()
{
    shader = new Shader();
    shader->loadVertFragShader("shader/test");
    shader->addAttribute(2, "vpos");
    shader->addAttribute(4, "vcolor");
   
    testvao = new VAO(shader);
    testvao->generate(3, buStaticDraw);
    testvao->map(baWriteOnly);

    float data[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };

    testvao->addVertex(data + 0);
    testvao->addVertex(data + 6);
    testvao->addVertex(data + 12);

    testvao->unmap();
}    

Game::~Game()
{
}

void Game::update()
{
    
}

void Game::render()
{
    testvao->render();
}
