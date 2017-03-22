#include "stdafx.h"

#include "Window.h"
#include "Game.h"

#include "Registry.h"

// Entrypoint for the Application

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    using namespace Registry;
    
    if (Key key = Key(L".txvs", pkClassesRoot, Key::cmReadOnly))
    {
        ErrorDialog("Key opened!");
        if (Key subkey = Key(L"test", key, Key::cmReadWrite))
        {
            if (Value* value = subkey.createValue(L"test", vtString))
            {
                ErrorDialog("ValueType: " + std::to_string(value->getType()));

                if (StringValue* strValue = dynamic_cast<StringValue*>(value))
                {
                    ErrorDialog(strValue->get());
                    if (!strValue->set(L"Ain't this super fucking awesome?"))
                        ErrorDialog("Could not set value! Error: " + getErrorString(strValue->getLastError()));
                }
                else
                {
                    ErrorDialog("Not a string value!");
                }
            }
            else
            {
                ErrorDialog("Could not get value! Error: " + getErrorString(subkey.getLastError()));
            }            
        }
        else
        {
            ErrorDialog("Could not open/create subkey! Error: " + getErrorString(subkey.getLastError()));
        }
    }
    else
    {
        ErrorDialog("Could not open key! Error: " + getErrorString(key.getLastError()));
    }

    return 0;

    GLWindow window(hInstance, _T("TextVenturer"));
    
    Game* game = new Game(&window);
    window.start(game);
    delete game;       

    return 0;
}