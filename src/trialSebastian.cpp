//Combat engine
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include "characters.h"    

/*Note: Combat and GUI are separate currently. 
Before compilation directory must be in src
g++ trialSebastian.cpp characters.cpp rng.cpp -o trialSebastian.exe -I "C:\raylib\raylib\src" -std=c++17 -Wall -Wextra -g
*/

//Helpers
static inline const std::string& nameOf(const Character& c) { return c.getName(); }
static inline int clampi(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }

//Temporary Armor
static inline int defend_bonus_armor(const Character& target) 
{
    return std::max(2, target.def.armor/5); //15/5 bonus armor
}

// Apply melee attack, using armor class.
static void resolve_melee(Character& attacker, Character& defender, bool defenderIsDefending, std::stringstream& log) 
{
    int beforeHP   = defender.vit.health;
    int originalAR = defender.def.armor;

    if (defenderIsDefending)
        defender.def.armor = originalAR + defend_bonus_armor(defender);

    
    attacker.dealMeleeDamage(defender);

    defender.def.armor = originalAR;

    int delta = std::max(0, beforeHP - defender.vit.health);
    if (delta > 0) 
    {
        log << nameOf(defender) << " takes " << delta
            << " damage.\n" << nameOf(defender) << " HP "
            << defender.vit.health << "/" << defender.vit.maxHealth << "\n";
    } 
    else 
    {
        log << nameOf(attacker) << " misses.\n";
    }
}

//Apply Ranged Attacks
static void resolve_ranged(Character& attacker, Character& defender, bool defenderIsDefending, std::stringstream& log) 
{
    int beforeHP   = defender.vit.health;
    int originalAR = defender.def.armor;

    if (defenderIsDefending)
    {
        defender.def.armor = originalAR + defend_bonus_armor(defender);
    }

    attacker.dealRangeDamage(defender);
    defender.def.armor = originalAR;

    int delta = std::max(0, beforeHP - defender.vit.health);
    if (delta > 0) 
    {
        log << nameOf(defender) << " takes " << delta << " damage.\n" << nameOf(defender) << " HP "
        << defender.vit.health << "/" << defender.vit.maxHealth << "\n";
    } 
    else 
    {
        log << nameOf(attacker) << " misses.\n";
    }
}

// Menu
enum class ActionType { Attack, Defend, UseRange, UseItem, None };
struct Action { ActionType type = ActionType::None; std::string desc; };

static Action player_choose() 
{
    while (true) 
    {
        std::cout << "\nChoose action:\n"
                     "1) Attack\n"
                     "2) Defend\n"
                     "3) Use Range (Need to implement\n"
                     "4) Use Item  (Need to implement)\n> ";
        std::string in;
        std::getline(std::cin, in);
        if (in.empty()) continue;

        switch (in[0]) 
        {
            case '1': return {ActionType::Attack, "Attack"};
            case '2': return {ActionType::Defend, "Defend"};
            case '3': return {ActionType::None, "UseRange"}; //Need to fix
            case '4': std::cout << "Inventory not implemented yet\n"; return {ActionType::UseItem, "UseItem"};
            default:  std::cout << "Invalid choice\n";
        }
    }
}

// AI for combat, defends 25% of the time
static Action ai_choose(const NonPlayerCharacter& /*self*/, const PlayerCharacter& /*foe*/) 
{
    int roll = roll_d(4);           
    if (roll == 1) return {ActionType::Defend, "Defend"};
    else
        return {ActionType::Attack, "Attack"};
}

// Main combat engine
void runCombat(Student& player, NonPlayerCharacter& enemy) 
{
    bool playerTurn = true; //will change to base on intiative
    bool playerDef  = false;  
    bool enemyDef   = false;

    std::stringstream log;

    auto print_status = [&]()
    {
        std::cout << "\n--------------------------------\n"
                  << nameOf(player) << " HP " << player.vit.health << "/" << player.vit.maxHealth
                  << (playerDef ? " [DEFENDING]" : "") << "\n"
                  << nameOf(enemy)  << " HP " << enemy.vit.health  << "/" << enemy.vit.maxHealth
                  << (enemyDef ? " [DEFENDING]" : "")  << "\n"
                  << "--------------------------------\n";
    };

    std::cout << "   Turn-Based Combat   \n";
    std::cout << nameOf(player) << " vs. " << nameOf(enemy) << "\n";
    print_status();

    while (player.isAlive() && enemy.isAlive()) 
    {
        log.str(""); log.clear();

        //Reset Defense
        if (playerTurn) playerDef = false; 
        else enemyDef = false;

        Action act = playerTurn ? player_choose(): ai_choose(enemy, player);
        if (!playerTurn) std::cout << nameOf(enemy) << " chose: " << act.desc << "\n";

        if (playerTurn) 
        {
            if (act.type == ActionType::Attack) 
            {
                resolve_melee(player, enemy, enemyDef, log);
            } 

            else if (act.type == ActionType::Defend) 
            {
                playerDef = true;
                log << nameOf(player) << " Defends.\n";
            } 

            else if (act.type == ActionType::UseRange) 
            {
                resolve_ranged(player, enemy, enemyDef, log);
            } 

            else 
            {
                log << nameOf(player) << " hesitates.\n";
            }

            if (act.type == ActionType::Attack) 
            {
                resolve_melee(enemy, player, playerDef, log);
            } 

            else if (act.type == ActionType::Defend) 
            {
                enemyDef = true;
                log << nameOf(enemy) << " Defends.\n";
            } 

            else if (act.type == ActionType::UseRange) 
            {
                resolve_ranged(enemy, player, playerDef, log);
            } 

            else 
            {
                log << nameOf(enemy) << " does nothing.\n";
            }
        }

        std::cout << log.str();

        if (!player.isAlive())
        { 
            std::cout << "\n>>> You died: Game over\n"; 
            break; 
        }
        if (!enemy.isAlive())  
        { 
            std::cout << "\n>>> You killed: " << nameOf(enemy);
            break; 
        }

        playerTurn = !playerTurn;
        print_status();
    }

    
}

// Temporary main function for debugging
int main() {
    std::istringstream* allStatLines = storeAllStatLines(openStartingStatsCSV());

    Attributes studentAttrs = {
        getStatForCharacterID(allStatLines, "Student", CSVStats::STR),
        getStatForCharacterID(allStatLines, "Student", CSVStats::DEX),
        getStatForCharacterID(allStatLines, "Student", CSVStats::CON),
        getStatForCharacterID(allStatLines, "Student", CSVStats::WIS),
        getStatForCharacterID(allStatLines, "Student", CSVStats::CHA),
        getStatForCharacterID(allStatLines, "Student", CSVStats::INT)
    };
    DefenseStats studentDef = {
        getStatForCharacterID(allStatLines, "Student", CSVStats::ARMOR),
        0
    };
    CombatStats studentCombat = {
        5, // meleeDamage (base)
        3, // rangeDamage (base)
        getStatForCharacterID(allStatLines, "Student", CSVStats::INITIATIVE)
    };
    VitalStats studentVital = {
        getStatForCharacterID(allStatLines, "Student", CSVStats::MAX_HEALTH),
        getStatForCharacterID(allStatLines, "Student", CSVStats::MAX_HEALTH)
    };
    StatusEffects studentStatus = {};

    Student Steve("Steve", studentAttrs, studentDef, studentCombat, studentVital, studentStatus);

    const std::string ZOMBIE_ID = "Zombie_Standard";
    auto safe = [](int v, int def){ return (v < 0 ? def : v); };

    Attributes zombieAttrs = {
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::STR), 3),
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::DEX), 1),
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::CON), 2),
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::WIS), 0),
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::CHA), -4),
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::INT), 0)
    };
    DefenseStats zombieDef = {
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::ARMOR), 12),
    0
    };
    CombatStats zombieCombat = 
    {
    4, 
    0, 
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::INITIATIVE), 1)
    };
    VitalStats zombieVital = {
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::MAX_HEALTH), 15),
    safe(getStatForCharacterID(allStatLines, ZOMBIE_ID, CSVStats::MAX_HEALTH), 15)
    };
    StatusEffects zombieStatus = {};

    NonPlayerCharacter Zombie("Zombie", zombieAttrs, zombieDef, zombieCombat, zombieVital, zombieStatus);
    Zombie.wep.meleeWeapon = 2;  //temporary fix for combat, zombie not dealing damage.


    //For debugging
    std::cerr << "For debugging Z HP=" << zombieVital.maxHealth << " AR=" << zombieDef.armor<< " INIT=" << zombieCombat.initiative << "\n";
    // Start battle
    runCombat(Steve, Zombie);

    std::cout << "\nAfter combat:";
    std::cout << "\nStudent " << Steve.name << " Health: " << Steve.vit.health;
    std::cout << "\nZombie Health: " << Zombie.vit.health << std::endl;

    delete allStatLines;
    return 0;
}
