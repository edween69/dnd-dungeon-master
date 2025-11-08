#include <iostream>  
#include <cstdlib>   
#include <string>        
#include <sstream>       
#include <random>        
#include <algorithm>     
#include <limits>    
#include <ctime>
#include <algorithm>
#include "characters.h"
#include "rng.h"
#ifndef COMBAT_H
#define COMBAT_H

void run_combat();

//keep
//Enumerated action types
enum class ActionType { Attack, Defend, UseRange, UseItem, Flee, None };

//steve.vit.health 
struct Actor 
{
    string name;
    int maxHP, hp, attack, defense;
    bool defending = false;
    bool isAlive() const {return hp > 0;} //reference characters.h
};

//remove actor
// Actor struct for player and zombie
struct Action
{
    ActionType type = ActionType::None;
    int ItemNum = -1;//however many the player has
    string description;

};

#endif