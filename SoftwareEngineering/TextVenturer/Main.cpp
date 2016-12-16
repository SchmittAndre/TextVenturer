#include "stdafx.h"

#include "Window.h"
#include "Game.h"

// Entrypoint for the Application

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // disable stupid auto scaling for high DPI
    SetProcessDPIAware();

    GLWindow window(hInstance, _T("TextVenturer"));

    Game* game = new Game(&window);   
    window.start(game);               
    delete game;

    return 0;
}