#pragma once

#include "GameDisplayer.h"

class MainMenu : public GameDisplayer
{
private:
    enum MenuPoint
    {
        mpPlay,
        mpEditor,
        mpOptions,
        mpExit,
        
        MENU_POINT_COUNT
    };

    static const std::string MenuPointStrings[MENU_POINT_COUNT];
    static const Color MenuPointDefaultColor;
    static const Color MenuPointSelectedColor;

    static const float MarkerMoveDelay;
    static const UINT MarkerLength;
    static const Color MarkerDefaultColor;
    static const Color MarkerSelectedColor;

    UINT getMenuPointLine(MenuPoint mp);
    UINT getMenuPointOffset(UINT x);
    uvec2 getMenuPointPos(MenuPoint mp, UINT x);

    MenuPoint selection;
    bool selectionChanged;

    UINT selectionMarker[MENU_POINT_COUNT];
    float markerMoveTimer;

public:
    MainMenu(TextDisplay* textDisplay);
    ~MainMenu();

    void notifySwitch();

    void pressKey(byte key);

    void update(float deltaTime);

};