#include "stdafx.h"
#include "TextDisplay.h"
#include "Game.h"
#include "Player.h"
#include "Command.h"

#include "Controler.h"

void Controler::updateInput()
{
    for (int x = cursorMin; x < cursorMax; x++)
        textDisplay->write(x, textDisplay->getCursorPos().y, ' ');
    textDisplay->write(cursorMin, textDisplay->getCursorPos().y, input);
}

Controler::Controler(TextDisplay* textDisplay, Game* game)
{
    this->textDisplay = textDisplay;
	this->game = game;
    textDisplay->setCursorVisible(true);
    textDisplay->setCursorPos(ivec2(3, textDisplay->getHeight() - 2));
    cursorMin = 3;
    cursorMax = textDisplay->getWidth() - 2;
}

void Controler::pressChar(byte c)
{
    switch (c)
    {
    case VK_BACK:
        if (textDisplay->getCursorPos().x > cursorMin)
        {
            ivec2 p = textDisplay->getCursorPos();
            p.x--;
            textDisplay->setCursorPos(p);
            textDisplay->resetCursorTime();
            input.resize(input.length() - 1);
            updateInput();
        }
        break;
    case VK_RETURN:
    {
        command(input);

        input = "";
        updateInput();
        textDisplay->setCursorPos(ivec2(cursorMin, textDisplay->getCursorPos().y));

        break;
    }
    case 127: // CTRL-BACK
    {
        ivec2 p = textDisplay->getCursorPos();
        p.x--;
        while (textDisplay->getChar(p) == ' ' && p.x >= cursorMin)
        {
            pressChar(VK_BACK);
            p.x--;
        }
        while (textDisplay->getChar(p) != ' ' && p.x >= cursorMin)
        {
            pressChar(VK_BACK);
            p.x--;
        }
        break;
    }                     
    default:
        if (textDisplay->getCursorPos().x < cursorMax)
        {
            ivec2 p = textDisplay->getCursorPos();
            p.x++;
            textDisplay->setCursorPos(p);
            textDisplay->resetCursorTime();
            input += c;
            updateInput();
        }
    }                   
}

void Controler::update(float deltaTime)
{

}

void Controler::command(string msg)
{
    textDisplay->clear();

    Command help("help");
    help.addAlias("list commands");

    Command hello("hello");
    hello.addAlias("hello!");

    Command clear("clear");
    clear.addAlias("clear screen");
    clear.addAlias("clear display");

    Command pickup("pick up <object>");
    pickup.addAlias("pickup <object>");

    Command inventory("inventory");
    inventory.addAlias("show inventory");

    Command combine("combine <object 1> with <object 2>");
    combine.addAlias("combine <object 1> and <object 2>");

    if (help.check(msg))
    {
        textDisplay->write(2, 2, "Avaliable Commands:");
        int y = 4;
        textDisplay->write(2, y++, help.getName());
        textDisplay->write(2, y++, hello.getName());
        textDisplay->write(2, y++, clear.getName());
        textDisplay->write(2, y++, pickup.getName());
        textDisplay->write(2, y++, inventory.getName());
        textDisplay->write(2, y++, combine.getName());  
    }
	else if (hello.check(msg))
	{
		textDisplay->write(2, 2, "Hey there!");
	}
    else if (clear.check(msg))
    {
        textDisplay->clear();
    }
    else if (Command::Result params = pickup.check(msg))
	{
		game->getPlayer()->additem(params["object"]);
		textDisplay->write(2, 2, "Picked up " + params["object"]);
	}
	else if (inventory.check(msg))
	{
		textDisplay->clear();
		textDisplay->write(2, 2, "Your inventory contains:");
		int y = 4;
		vector<string> inventory = game->getPlayer()->getInventory();
		for (string item : inventory)
			textDisplay->write(2, y++, item);
	}
    else if (Command::Result params = combine.check(msg))
    {
        textDisplay->write(2, 2, "Combining " + params["object 1"] + " and " + params["object 2"] + " with a lot of magic!");
    }
	else
	{
		textDisplay->write(2, 2, "Wait... what?");
	}   
}
