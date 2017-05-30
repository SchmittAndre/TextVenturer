#include "stdafx.h"

#include "Window.h"
#include "Game.h"

// Entrypoint for the Application

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GLWindow window(hInstance, _T("TextVenturer"));
    Game game(window);
    window.start(game);                            
    return 0;
}