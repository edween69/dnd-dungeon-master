/*======================================== combat.h ========================================
  Project: TTRPG Game ?
  Subsystem: Combat Engine
  Primary Author: Sebastian Cardona
  Description: This file... @SebastianCardona please fill this in

*/
#include "characters.h"
#include <sstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>


//@brief: Enum representing different action types during combat
//@version: 1.0
//@author: Sebastian Cardona
enum class ActionType {Attack, Defend, UseRange, UseItem, None};

//@brief: Struct representing an action taken during combat, including its type and description
//@version: 1.0
//@author: Sebastian Cardona
struct Action 
{
    ActionType type = ActionType::None;
    std::string desc; 
};

//@brief: Struct to handle combat state including turn management and logging(will be used to handle combat flow in the screenManager later)
//@version: 1.0
//@author: Edwin Baiden
struct CombatHandler
{
    bool playerTurn = true;
    bool playerIsDefending  = false;
    bool enemyIsDefending   = false;

    std::stringstream log;
    float enemyActionDelay = 1.0f;
};

//Function prototypes
const std::string& nameOf(const Character& c);
int clampi(int v, int lo, int hi);
void resolve_melee(Character& attacker, Character& defender, bool defenderIsDefending, std::stringstream& log);
void resolve_ranged(Character& attacker, Character& defender, bool defenderIsDefending, std::stringstream& log);
Action ai_choose(const NonPlayerCharacter& /*self*/, const PlayerCharacter& /*foe*/);