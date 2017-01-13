#pragma once

class Command;

#include "AdventureAction.h"

class DefaultAdventureAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class HelpAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class ShowInventoryAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class InspectAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class PickupAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class ChangeRoomAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class GotoLocationAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class CombineItemsAction : public AdventureAction
{
public:
    void run(Command::Result params);
};

class UseItemAction : public AdventureAction
{
public:
    void run(Command::Result params);
};