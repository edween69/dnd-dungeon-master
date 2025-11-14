#include <cstdlib>
#include <string>
#include "characters.h"
#include "rng.h"
#ifndef COMBAT_H
#define COMBAT_H

//keep
//Enumerated action types
enum class ActionType { Attack, Defend, UseMelee, UseRange, UseItem, Flee, None };

//remove actor
// Actor struct for player and zombie
struct Action
{
    ActionType type = ActionType::None;
    int ItemNum = -1;//however many the player has
    std::string description;

};

void apply_attack(Character& attacker, Character& defender, std::stringstream& log);
void apply_ranged(Character& attacker, Character& defender, std::stringstream& log);
void apply_defend(Character& a, std::stringstream& log);
Action ai_choose(const Character& self, const Character& foe);
Action player_choose();
void startStats ();
void runCombat(Student& stnt,NonPlayerCharacter& zombie);

#endif