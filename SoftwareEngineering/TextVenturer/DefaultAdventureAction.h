#pragma once

#include "AdventureAction.h"
#include "Command.h"

class Adventure;

class DefaultAdventureAction : public AdventureAction
{
public:
    DefaultAdventureAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class HelpAction : public AdventureAction
{
public:
    HelpAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class LookAroundAction : public AdventureAction
{
public:
    LookAroundAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class ShowInventoryAction : public AdventureAction
{
public:
    ShowInventoryAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class InspectAction : public AdventureAction
{
public:
    InspectAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
    tags requiredParameters() const { return{ "thing" }; }
};

class TakeFromAction : public AdventureAction
{
public:
    TakeFromAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
    tags requiredParameters() const { return{ "item", "location", "prep" }; }
};

class TakeAction : public AdventureAction
{
public:
    TakeAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;  
    tags requiredParameters() const { return{ "item" }; }
};

class PlaceAction : public AdventureAction
{
public:
    PlaceAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
    tags requiredParameters() const { return{ "item", "location", "prep" }; }
};

class UseRoomConnectionAction : public AdventureAction
{
public:
    UseRoomConnectionAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
    tags requiredParameters() const { return{ "door" }; }
};

class GotoAction : public AdventureAction
{
public:
    GotoAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
    tags requiredParameters() const { return{ "place" }; }
};

class EnterRoomAction : public AdventureAction
{
public:
    EnterRoomAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
    tags requiredParameters() const { return{ "room" }; }
};

class CombineItemsAction : public AdventureAction
{
public:
    CombineItemsAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
    tags requiredParameters() const { return{ "item1", "item2" }; }
};