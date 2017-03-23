#include "stdafx.h"

#include "Window.h"
#include "Game.h"

#include "RegTools.h"
#include "Registry.h"

// Entrypoint for the Application

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    /*
    if (RegTools::InstallExtension(L"txvs", L"TextVenturerScript", L"TextVenturer Script"))
        ErrorDialog("Extension installed!");
    else
        ErrorDialog("Could not install extension!");
    */

    if (auto key = Registry::Key(Registry::pkCurrentUser, L"SOFTWARE\\TextVenturer", Registry::Key::cmCreate))
    {
        if (Registry::StringValue* value = *key.createDefaultValue(Registry::vtString))
        {
            value->set(L"Dies ist ein unglaublich toller Test!");
        }
        else
            ErrorDialog("Error: " + getErrorString(key.getLastError()));
    }
    else
        ErrorDialog("Error: " + getErrorString(key.getLastError()));

    return 0;

    GLWindow window(hInstance, _T("TextVenturer"));
    
    Game* game = new Game(&window);
    window.start(game);
    delete game;       

    return 0;
}