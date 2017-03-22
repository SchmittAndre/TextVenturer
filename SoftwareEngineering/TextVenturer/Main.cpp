#include "stdafx.h"

#include "Window.h"
#include "Game.h"

#include "Registry.h"

// Entrypoint for the Application

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    namespace Reg = Registry;
    
    if (Reg::Key key = Reg::Key(L".txvs", Reg::pkClassesRoot, true))
    {
        ErrorDialog("Key opened!");
        if (Reg::Key subkey = Reg::Key(L"test", key, true))
        {
            if (Reg::Value* value = subkey.getValue(L"test"))
            {
                ErrorDialog("ValueType: " + std::to_string(value->getType()));

                if (Reg::StringValue* strValue = dynamic_cast<Reg::StringValue*>(value))
                {
                    ErrorDialog(strValue->get());
                    strValue->set(L"Ain't this super fucking awesome?");
                }
                else
                {
                    ErrorDialog("Not a string value!");
                }
            }
            else
            {
                ErrorDialog("Could not get value! ErrorCode: " + std::to_string(subkey.getLastError()));
            }            
        }
        else
        {
            ErrorDialog("Could not open/create subkey ErrorCode: " + std::to_string(key.getLastError()));
        }
    }
    else
    {
        ErrorDialog("Could not open key! ErrorCode: " + std::to_string(key.getLastError()));
    }

    return 0;

    GLWindow window(hInstance, _T("TextVenturer"));
    
    Game* game = new Game(&window);
    window.start(game);
    delete game;       

    return 0;
}