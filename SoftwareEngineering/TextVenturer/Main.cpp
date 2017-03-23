#include "stdafx.h"

#include "Window.h"
#include "Game.h"

#include "RegTools.h"
#include "Registry.h"

// Entrypoint for the Application

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    RegTools::InstallExtension(L"txvs", L"TextVenturerScript", L"TextVenturer Script");

    return 0;

    GLWindow window(hInstance, _T("TextVenturer"));
    
    Game* game = new Game(&window);
    window.start(game);
    delete game;       

    return 0;
}