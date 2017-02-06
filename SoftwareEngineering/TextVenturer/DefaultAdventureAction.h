#pragma once

#include "AdventureAction.h"

class AdventureAction;

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
};

class PickupAction : public AdventureAction
{
public:
    PickupAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class PickupFromAction : public AdventureAction
{
public:
    PickupFromAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class UseRoomConnectionAction : public AdventureAction
{
public:
    UseRoomConnectionAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class GotoAction : public AdventureAction
{
public:
    GotoAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class EnterRoomAction : public AdventureAction
{
public:
    EnterRoomAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};

class CombineItemsAction : public AdventureAction
{
public:
    CombineItemsAction(Adventure* adventure) : AdventureAction(adventure) {}
    bool run(const Command::Result & params) const;
};