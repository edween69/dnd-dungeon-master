#include <cstdlib>
#include "characters.h"
#include "rng.h"
#ifndef COMBAT_H
#define COMBAT_H

void run_combat();

//keep
//Enumerated action types
enum class ActionType { Attack, Defend, UseRange, UseRange, UseItem, Flee, None };

//remove actor
// Actor struct for player and zombie
struct Action
{
    ActionType type = ActionType::None;
    int ItemNum = -1;//however many the player has
    string description;

};

#endif