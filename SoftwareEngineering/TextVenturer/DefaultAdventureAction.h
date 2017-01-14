#pragma once

class Command;

#include "AdventureAction.h"

class DefaultAdventureAction : public AdventureAction
{
public:
    DefaultAdventureAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class HelpAction : public AdventureAction
{
public:
    HelpAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class LookAroundAction : public AdventureAction
{
public:
    LookAroundAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class ShowInventoryAction : public AdventureAction
{
public:
    ShowInventoryAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class InspectAction : public AdventureAction
{
public:
    InspectAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class PickupAction : public AdventureAction
{
public:
    PickupAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class UseRoomConnectionAction : public AdventureAction
{
public:
    UseRoomConnectionAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class GotoAction : public AdventureAction
{
public:
    GotoAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class CombineItemsAction : public AdventureAction
{
public:
    CombineItemsAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class UseItemAction : public AdventureAction
{
public:
    UseItemAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class LockLocationAction : public AdventureAction
{
public:
    LockLocationAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};

class UnlockLocationAction : public AdventureAction
{
public:
    UnlockLocationAction(Adventure* adventure) : AdventureAction(adventure) {}
    void run(const Command::Result & params) const;
};