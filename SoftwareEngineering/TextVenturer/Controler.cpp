#include "stdafx.h"
#include "Command.h"
#include "CommandSystem.h"
#include "TextDisplay.h"
#include "Game.h"
#include "DefaultAction.h"
#include "Adventure.h"
#include "Window.h" 

#include "Controler.h"

void Controler::updateInput()
{
    for (int x = cursorMin; x < cursorMax; x++)
        textDisplay->write(x, textDisplay->getCursorPos().y, ' ');
    textDisplay->write(cursorMin, textDisplay->getCursorPos().y, input);
}

Controler::Controler(TextDisplay* textDisplay, Game* game)
{
    defaultAction = new DefaultAction(this);
    commandSystem = new CommandSystem(this, defaultAction);
    this->textDisplay = textDisplay;
	this->game = game;
    textDisplay->setCursorVisible(true);
    textDisplay->setCursorPos(ivec2(3, textDisplay->getHeight() - 2));
    cursorMin = 3;
    cursorMax = textDisplay->getWidth() - 2;

    adventure = new Adventure(this);
    newLine = false;
    writepos = 1;

    textDisplay->write(1, textDisplay->getHeight() - 2, '>');
}

Controler::~Controler()
{
    delete adventure;
    delete defaultAction;
    delete commandSystem;
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
    if (!textbuffer.empty())
    {
        state.time = state.time - deltaTime;

        while (!textbuffer.empty() && state.time <= 0)
        {
            state.time += state.delay;
            if (newLine)
            {
                textDisplay->move(ivec2(1, 2), uvec2(textDisplay->getWidth() - 2, textDisplay->getHeight() - 5), ivec2(1, 1));
                textDisplay->clearLine(textDisplay->getHeight() - 4, 1, textDisplay->getWidth() - 2);
                newLine = false;
            }
            size_t count = textDisplay->writeSingleChar(ivec2(writepos++, textDisplay->getHeight() - 4), textbuffer.front(), state);
            textbuffer.front() = textbuffer.front().substr(count);
            if (textbuffer.front().empty())
            {
                writepos = 1;
                textbuffer.pop();
                newLine = true;
            }
        }
    }
}

void Controler::writeLine(string msg, TextDisplay::State & state)
{
    string text = regex_replace(msg, regex("([^$])\\$[^$ ]+?\\([^ ]*?\\)"), "$1"); // replace "[^$]$___(___)" with "[^$]"
    text = regex_replace(text, regex("^\\$[^$ ]+?\\([^ ]*?\\)"), "");              // replace "^$___(___)" with ""
    text = regex_replace(text, regex("\\$\\$"), "$");                              // replace "$$" with "$"

    if (text.size() > textDisplay->getWidth() - 2)
    {
        size_t spacePos = msg.find_last_of(' ', textDisplay->getWidth() - 2);
        if (spacePos == string::npos)
            spacePos = textDisplay->getWidth() - 2;
        textbuffer.push(msg.substr(0, spacePos));
        // textDisplay->write(ivec2(1, textDisplay->getHeight() - 4), msg.substr(0, spacePos), state);
        writeLine(msg.substr(msg.find_first_not_of(' ', spacePos)));
    }
    else
        textbuffer.push(msg + "$reset()");
        // textDisplay->write(ivec2(1, textDisplay->getHeight() - 4), msg, state);
}

void Controler::command(string msg)
{
    if (adventure && adventure->isInitialized())
    {
        adventure->sendCommand(msg);
    }
    else
    {
        commandSystem->sendCommand(msg);
    }

    /*
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

    Command use("use <object 1> with <object 2>");
    combine.addAlias("use <object 1> and <object 2>");

	Command play("Play");
	play.addAlias("Go");

    if (help.check(msg))
    {
        textscrolling("Available Commands:");
        textscrolling(help.getName());
        textscrolling(hello.getName());
        textscrolling(clear.getName());
        textscrolling(pickup.getName());
        textscrolling(inventory.getName());
        textscrolling(combine.getName());
		textscrolling(play.getName());
	}
	else if (play.check(msg))
	{
		textscrolling("You find yourself in a shining white room.");
		textscrolling("In front of you there is a sheet of paper and shears.");
		textscrolling("What do you want to do?");
	}
	else if (hello.check(msg))
	{        textscrolling("Hey there!");
    }
    else if (clear.check(msg))
    {
        textDisplay->clear();
    }
    else if (Command::Result params = pickup.check(msg))
	{
		game->getPlayer()->addItem(params["object"]);        textscrolling("Picked up " + params["object"]);
    }
    else if (inventory.check(msg))
    {
        textscrolling("Your inventory contains:");
        int y = 4;
        textscrolling("");
        vector<string> inventory = game->getPlayer()->getInventory();
        for (string item : inventory)
            textscrolling(item);
    }
    else if (Command::Result params = combine.check(msg))
    {
        
		if ((params["object 1"] == "stick" && params["object 2"] == "string") ||
			(params["object 1"] == "string" && params["object 2"] == "stick"))
		{
			if (game->getPlayer()->checkItem(params["object 1"]) &&
				game->getPlayer()->checkItem(params["object 2"]))
			{
				game->getPlayer()->deleteItem(params["object 1"]);
				game->getPlayer()->deleteItem(params["object 2"]);
				game->getPlayer()->addItem("bow");
				textscrolling("Combining " + params["object 1"] + " and " + params["object 2"] + " with a lot of magic!");
			}
			else
			{
				textscrolling("you don't have these items");
			}
		}
		else
		{
			textscrolling("Combining " + params["object 1"] + " and " + params["object 2"] + " with a lot of magic!");
		}
				
    }
    else if (Command::Result params = use.check(msg))
    {
        textscrolling("Used " + params["object 1"] + " with " + params["object 2"] + " and nothing happends.");
    }
    else
    {
        textscrolling("Wait... what?");
    }
    */
}
