#pragma once

class TextDisplay;
class Game;
class CommandSystem;
class DefaultAction;
class Player;

// REMOVE ME
class Room;
class RoomConnection;

class Controler
{
private:
    CommandSystem* commandSystem;
    DefaultAction* defaultAction;

    Player* player;

    TextDisplay* textDisplay;
	Game* game;
    int cursorMin;
    int cursorMax;

    string input;

    // TESTING BEGIN
    Room* garden;
    Room* shed;
    RoomConnection* shedDoor;
    // TESTING END

    void updateInput();

public:
    Controler(TextDisplay* textDisplay,Game* game);
    ~Controler();

    void pressChar(byte c);

    void update(float deltaTime);

    void textscrolling(string msg);

    void command(string msg);

};

