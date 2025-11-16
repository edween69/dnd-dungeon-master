/*====================================== combat.cpp ========================================
  Project: TTRPG Game ?
  Subsystem: Combat Engine
  Primary Author: Sebastian Cardona
  Description: This file... @SebastianCardona please fill this in

*/

#include "combat.h"

//@brief: Get the name of a character
//@param c - The character whose name is to be retrieved
//@return: The name of the character as a string reference
//@version: 1.0
//@author: Sebastian Cardona
const std::string& nameOf(const Character& c)
{
     return c.getName(); 
}


//@brief: Clamp an integer value between a lower and upper bound
//@param v - The value to be clamped
//@param lo - The lower bound
//@param hi - The upper bound
//@return: The clamped value
//@version: 1.0
//@author: Sebastian Cardona
int clampi(int v, int lo, int hi)
{ 
    return std::max(lo, std::min(v, hi)); 
}

//@brief: Resolve a melee attack from an attacker to a defender, considering if the defender is defending
//@param attacker - The character performing the attack
//@param defender - The character receiving the attack
//@param defenderIsDefending - Boolean indicating if the defender is in a defending state
//@param log - A stringstream to log the combat events
//@version: 1.0
//@author: Sebastian Cardona
void resolve_melee(Character& attacker, Character& defender, bool defenderIsDefending, std::stringstream& log)
{
    int beforeHP = defender.vit.health;

    if (defenderIsDefending) defender.startDefense();
    attacker.dealMeleeDamage(defender);
    defender.endDefense();

    int delta = std::max(0, beforeHP - defender.vit.health);
    if (delta > 0)
        log << nameOf(defender) << " takes " << delta << " damage." << std::endl;
    else
        log << nameOf(attacker) << " misses." << std::endl;
}

//@brief: Resolve a ranged attack from an attacker to a defender, considering if the defender is defending
//@param attacker - The character performing the attack
//@param defender - The character receiving the attack
//@param defenderIsDefending - Boolean indicating if the defender is in a defending state
//@param log - A stringstream to log the combat events
//@version: 1.0
//@author: Sebastian Cardona
void resolve_ranged(Character& attacker, Character& defender, bool defenderIsDefending, std::stringstream& log) 
{
    int beforeHP   = defender.vit.health;
    //int originalAR = defender.def.armor;

    if (defenderIsDefending)
        //defender.def.armor = originalAR + defend_bonus_armor(defender);//
        defender.startDefense();
    

    attacker.dealRangeDamage(defender);
    //defender.def.armor = originalAR;
    defender.endDefense();

    int delta = std::max(0, beforeHP - defender.vit.health);
    if (delta > 0) 
    {
        log << nameOf(defender) << " takes " << delta << " damage." << std::endl;
    } 
    else 
    {
        log << nameOf(attacker) << " misses." << std::endl;
    }
}

//@brief: AI function to choose an action for a non-player character during combat
//@param self - The NPC character making the decision
//@param foe - The player character being targeted
//@version: 1.0
//@author: Sebastian Cardona
Action ai_choose(const NonPlayerCharacter& /*self*/, const PlayerCharacter& /*foe*/) 
{
    int roll = roll_d(4);           
    if (roll == 1) return {ActionType::Defend, "Defend"};
    else
        return {ActionType::Attack, "Attack"};
}