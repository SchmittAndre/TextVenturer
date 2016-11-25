#include "stdafx.h"
#include "TextDisplay.h"
#include "Game.h"
#include "Player.h"
#include "Controler.h"
#include "window.h"

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
	//this->textDisplay->write(2, 2, "available commands:");
	//this->textDisplay->write(2, 3, "pick up, inventory, hello");
	textscrolling("available commands:");
	textscrolling("pick up, inventory, hello");
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

void Controler::textscrolling(string msg) 
{
	string temp;
	for (int j = 0; j < (textDisplay->getHeight()-3); j++)
	{ 
		
		if (j == (textDisplay->getHeight() - 4))
		{
			textDisplay->clearline(j);
			textDisplay->write(2, j, msg);
		}
		else
		{
			textDisplay->clearline(j);
			textDisplay->write(0, j, textDisplay->getLine(j + 1));
		}
	}
}

void Controler::command(string msg)
{
	//textDisplay->clear();
	transform(msg.begin(), msg.end(), msg.begin(), toupper);
	if (msg == "HELLO")
	{
		//textDisplay->write(2, 2, "Hallo du penner");
		textscrolling("Hallo du penner");
	}
	else if (msg.substr(0,7)=="PICK UP")
	{
		string temp = "Picked up" + msg.substr(7, msg.length()-7);
		game->getPlayer()->additem(msg.substr(8, msg.length() - 8));
		//textDisplay->write(2, 2, temp);
		textscrolling(temp);
	}
	else if (msg == "CLEAR")
	{
		textDisplay->clear();
	}
	else if (msg == "INVENTORY")
	{
		//textDisplay->clear();
		textDisplay->write(2, 2, "In your Inventory is:");
		textscrolling("In your Inventory is:");
		int temp = 3;
		vector<string> inventory = game->getPlayer()->getInventory();
			for (string item : inventory)
			{
				//textDisplay->write(2, temp, item);
				textscrolling(" "+item);
				temp++;
			}
	}
	 else if (msg == "EXIT")
	 {
		 game->getWindow()->stop();
	 }
	else
	{
		textDisplay->clear();
		textDisplay->write(2, 2, "HÄ");
	}
}
