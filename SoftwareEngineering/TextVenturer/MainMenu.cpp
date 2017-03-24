#include "stdafx.h"

#include "AsciiArt.h"
#include "TextDisplay.h"
#include "DisplayChar.h"

#include "MainMenu.h"

const std::string MainMenu::MenuPointStrings[MENU_POINT_COUNT] = {
    "Play",
    "Editor",
    "Options",
    "Exit"
};                                                        
const Color MainMenu::MenuPointDefaultColor = Color(0.2f, 0.4f, 0.8f);
const Color MainMenu::MenuPointSelectedColor = Color(0.4f, 0.6f, 1.0f);
                                    
const float MainMenu::MarkerMoveDelay = 0.02f;
const UINT MainMenu::MarkerLength = 10;
const Color MainMenu::MarkerDefaultColor = Color(0.8f, 0.0f, 0.0f);
const Color MainMenu::MarkerSelectedColor = Color(1.0f, 0.2f, 0.0f);

UINT MainMenu::getMenuPointLine(MenuPoint mp)
{
    return 13 + mp * 4;
}

UINT MainMenu::getMenuPointOffset(UINT x)
{
    return 15 + x * 3;
}

uvec2 MainMenu::getMenuPointPos(MenuPoint mp, UINT x)
{
    return uvec2(getMenuPointOffset(x), getMenuPointLine(mp));
}

MainMenu::MainMenu(TextDisplay * textDisplay)
    : GameDisplayer(textDisplay)
{

}

MainMenu::~MainMenu()
{

}

void MainMenu::notifySwitch()
{
    AsciiArt logo;
    logo.loadFromFile("data\\AsciiArt\\logo.txva");
    getTextDisplay()->draw(3, logo);

    getTextDisplay()->setCursorVisible(false);

    for (UINT i = 0; i < MENU_POINT_COUNT; i++)
    {
        MenuPoint& mp = reinterpret_cast<MenuPoint&>(i);
        for (UINT x = 0; x < (UINT)MenuPointStrings[mp].size(); x++)
        {   
            DisplayChar* c = getTextDisplay()->getDisplayChar(getMenuPointPos(mp, x));
            c->setChar(MenuPointStrings[mp][x]);
            c->setScale(3);
            c->setColor(MenuPointDefaultColor);
        }

        selectionMarker[mp] = 0;
    }
    markerMoveTimer = 0;
    selectionChanged = true;
    selection = mpPlay;
}

void MainMenu::pressKey(byte key)
{
    switch (key)
    {
    case VK_DOWN:
        selection = MenuPoint((selection + 1) % MENU_POINT_COUNT);
        selectionChanged = true;
        break;
    case VK_UP:
        selection = MenuPoint((selection + MENU_POINT_COUNT - 1) % MENU_POINT_COUNT);
        selectionChanged = true;
        break;     
    }
}

void MainMenu::update(float deltaTime)
{
    markerMoveTimer -= deltaTime;
    while (markerMoveTimer <= 0)
    {
        markerMoveTimer += MarkerMoveDelay;
        for (UINT i = 0; i < MENU_POINT_COUNT; i++)
        {
            MenuPoint& mp = reinterpret_cast<MenuPoint&>(i);

            bool changed = false;
            if (mp == selection)
            {
                if (selectionMarker[mp] < MarkerLength)
                {
                    selectionMarker[mp]++;
                    changed = true;
                }
            }
            else
            {
                if (selectionMarker[mp] > 0)
                {
                    selectionMarker[mp]--;
                    changed = true;
                }
            }              

            UINT line = getMenuPointLine(mp);
            
            if (changed)
            {
                for (int x = -4; x < (int)selectionMarker[mp] - 3; x++)
                {
                    getTextDisplay()->write(x, line - 1, "_|\\ ");
                    getTextDisplay()->write(x, line,     "_| > ");
                    getTextDisplay()->write(x, line + 1, " |/ ");
                }

                for (UINT x = 0; x < MarkerLength; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        DisplayChar* c = getTextDisplay()->getDisplayChar(x, line + y);
                        if (mp == selection)
                        {
                            c->setColor(MarkerSelectedColor);
                        }
                        else
                        {
                            c->setColor(MarkerDefaultColor);
                        }
                    }
                }
            }

            if (selectionChanged)
            {
                for (UINT x = 0; x < MenuPointStrings[mp].size(); x++)
                {
                    DisplayChar* c = getTextDisplay()->getDisplayChar(getMenuPointOffset(x), line);
                    if (mp == selection)
                    {
                         c->setShaking(1);
                         c->setColor(MenuPointSelectedColor);
                    }
                    else
                    {
                        c->setShaking(0);
                        c->setColor(MenuPointDefaultColor);
                    }
                }
            }
        }
        selectionChanged = false;
    }
}
