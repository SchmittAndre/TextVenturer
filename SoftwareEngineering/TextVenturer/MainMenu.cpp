#include "stdafx.h"

#include "AsciiArt.h"
#include "TextDisplay.h"
#include "DisplayChar.h"
#include "Controler.h"
#include "Game.h"
#include "Window.h"

#include "MainMenu.h"

const std::string MainMenu::MenuPointStrings[MENU_POINT_COUNT] = {
    "Play",
    "Options",
    "Exit"
};                                                        
const Color MainMenu::MenuPointDefaultColor = Color(0.2f, 0.4f, 0.8f);
const Color MainMenu::MenuPointSelectedColor = Color(0.4f, 0.6f, 1.0f);
                                
ivec2 MainMenu::getMenuPointPos(MenuPoint mp)
{
    return ivec2(15, 14 + mp * 5);
}

void MainMenu::changeSelection(MenuPoint menuPoint)
{
    selection = menuPoint;
    selectionChanged = true;
}

/*
void MainMenu::updateMenuPoint(MenuPoint menuPoint, bool selected)
{     
    UINT line = getMenuPointLine(menuPoint);
    for (UINT x = 0; x < MenuPointStrings[menuPoint].size(); x++)
    {
        DisplayChar & c = getTextDisplay().getDisplayChar(getMenuPointOffset(x), line);
        if (selected)
        {
            c.setShaking(1);
            c.setColor(MenuPointSelectedColor);
        }
        else
        {
            c.setShaking(0);
            c.setColor(MenuPointDefaultColor);
        }
    }
} 
*/

MainMenu::MainMenu(Controler & controler)
    : GameDisplayer(controler)
{

}

MainMenu::~MainMenu()
{

}

void MainMenu::notifyLoad()
{
    GameDisplayer::notifyLoad();

    AsciiArt logo;
    logo.loadFromFile(L"data\\AsciiArt\\logo.txva");
    getTextDisplay().draw(3, logo);

    getTextDisplay().setCursorVisible(false);

    changeSelection(mpPlay);
}

void MainMenu::pressKey(byte key)
{
    switch (key)
    {
    case VK_DOWN:
    case VK_RIGHT:
    case VK_NEXT:
        changeSelection(MenuPoint((selection + 1) % MENU_POINT_COUNT));
        break;
    case VK_UP:
    case VK_LEFT:
    case VK_PRIOR:
        changeSelection(MenuPoint((selection + MENU_POINT_COUNT - 1) % MENU_POINT_COUNT));
        break;  
    case VK_RETURN:
    case VK_SPACE:
        switch (selection)
        {
        case mpPlay:
            getControler().changeDisplayer(Controler::dtAdventureSelection);
            break;
        case mpOptions:
        {
            throw(ENotImplemented, "Options");
            getControler().changeDisplayer(Controler::dtOptionMenu);
            break;
        }
        case mpExit:
            getControler().getGame().getWindow().stop();
            break;        
        }
        break;
        /*
    case VK_RETURN:       
        for (UINT x = 0; x < getTextDisplay()->getWidth(); x++)
            for (UINT y = 0; y < getTextDisplay()->getHeight(); y++)
            {
                DisplayChar* c = getTextDisplay()->getDisplayChar(x, y);
                c->setShaking(0);
               
                c->setAngularVelocity(((float)rand() / RAND_MAX - 0.5f) * 200);
                c->setAcceleration(vec2(0, -2.5f));
                c->setVelocity(vec2((float)rand() / RAND_MAX - 0.5f, (float)rand() / RAND_MAX - 0.5f) * 2.0f);
                
                
                c->setAngularVelocity(((float)rand() / RAND_MAX - 0.5f) * 90);
                vec2 p((float)x / getTextDisplay()->getWidth(), 1 - (float)y / getTextDisplay()->getHeight());
                p = p * 2 - 1;
                p = p.rotate(p.length() * 30);
                c->setVelocity(p); 
                c->setAcceleration(-p * 2);
                
            }
        break;
    case VK_BACK:
        getTextDisplay()->clear();
        notifyLoad();
        break;
        */
    }
}

void MainMenu::update(float deltaTime)
{
    if (selectionChanged)
    {
        TextDisplay::State state;
        state.scale = vec2(3, 3);
        state.offsetMovement = vec2(3, 0);
        for (UINT i = 0; i < MENU_POINT_COUNT; i++)
        {
            state.offset = vec2(0, 0);
            MenuPoint mp = static_cast<MenuPoint>(i);
            if (mp == selection)
            {
                state.color = MenuPointSelectedColor;
                state.shaking = 1;
            }
            else
            {
                state.color = MenuPointDefaultColor;
                state.shaking = 0;
            }
            getTextDisplay().writeAll(getMenuPointPos(mp), MenuPointStrings[mp], state);
        }

        selectionChanged = false;
    }
}
