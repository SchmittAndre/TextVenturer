#pragma once

class Contoler;
class CommandSystem;
class Player;
class Room;

class Adventure
{
private:
    Controler* controler;

    CommandSystem* commandSystem;          
    Player* player;

    vector<Room*> rooms;
    vector<Location*> locations;
    vector<Item*> items;
    
public:
    Adventure(Controler* controler);
    ~Adventure();

    void loadFromFile(string filename);
    
    void sendCommand(string command);

    Player* getPlayer();
};

