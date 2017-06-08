#pragma once

#include "GameDisplayer.h"

class MainMenu : public GameDisplayer
{
private:
    enum MenuPoint
    {
        mpPlay,
        mpOptions,
        mpExit,
        
        MENU_POINT_COUNT
    };

    static const std::string MenuPointStrings[MENU_POINT_COUNT];
    static const Color MenuPointDefaultColor;
    static const Color MenuPointSelectedColor;

    static ivec2 getMenuPointPos(MenuPoint mp);

    MenuPoint selection;

    bool selectionChanged;

    void changeSelection(MenuPoint menuPoint);
    
public:
    MainMenu(Controler & Controler);
    ~MainMenu();

    void notifyLoad();

    void pressKey(byte key);

    void update(float deltaTime);

};