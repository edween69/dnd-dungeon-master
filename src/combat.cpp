/*====================================== combat.cpp ========================================
  Project: TTRPG Game ?
  Subsystem: Combat Engine
  Primary Author: Sebastian Cardona
  Description: This file contains the combat engine and helper functions for a turn based combat state machine.


*/

#include "combat.h"

//@brief: Get the name of a character
//@param c - The character whose name is to be retrieved
//@return: The name of the character as a string reference
//@version: 1.0
//@author: Sebastian Cardona
const std::string &nameOf(const Character &c)
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

/**
 * @brief Namespace to handle resolving attack for both melee and range. This is to avoid the duplicate code between resolve_melee and resolve_range
 * @version 1.0
 * @author Andrew Kurtz
 */
namespace
{
    bool resolveAttack(
        Character &attacker,
        Character &defender,
        std::vector<std::string> &log,
        void (Character::*attackFn)(Character &))
    {
        std::int8_t beforeHP = defender.vit.health;

        // Later: apply defenderIsDefending or armor bonuses here.
        (attacker.*attackFn)(defender);

        std::int8_t delta = std::max<std::int8_t>(0, beforeHP - defender.vit.health);

        if (delta > 0)
        {
            AddNewLogEntry(log, nameOf(defender) + " takes " + std::to_string(delta) + " damage.");
        }
        else
        {
            AddNewLogEntry(log, nameOf(attacker) + " misses.");
        }
        return delta > 0;
    }
}

/**
 * @brief: Resolve a melee attack from an attacker to a defender, considering if the defender is defending
 * @param attacker - The character performing the attack
 * @param defender - The character receiving the attack
 * @param log - A stringstream to log the combat events
 * @version 1.1
 * @return True if damage was dealt, false otherwise
 * @author Sebastian Cardona edited by Andrew
 */
bool resolve_melee(Character &attacker, Character &defender, bool /* defenderIsDefending */, std::vector<std::string> &log)
{
    return resolveAttack(attacker, defender, log, &Character::dealMeleeDamage);
}

/**
 * @brief: Resolve a ranged attack from an attacker to a defender, considering if the defender is defending
 * @param attacker - The character performing the attack
 * @param defender - The character receiving the attack
 * @param log - A stringstream to log the combat events
 * @version 1.1
 * @return True if damage was dealt, false otherwise
 * @author Sebastian Cardona edited by Andrew
 */
bool resolve_ranged(Character &attacker, Character &defender, bool /* defenderIsDefending */, std::vector<std::string> &log)
{
    return resolveAttack(attacker, defender, log, &Character::dealRangeDamage);
}

//@brief: Resolve a ranged attack from an attacker to a defender, considering if the defender is defending
//@param player - The player using the inventory
//@param items - A vector storing the players inventory
//@param log - A stringstream to log the combat events
//@version: 1.0
//@author: Sebastian Cardona
void resolve_inventory(Student &player, std::vector<std::string> &log)
{
    const auto &items = player.inv.getItems();

    if (items.empty())
    {
        AddNewLogEntry(log, nameOf(player) + "'s inventory is empty.\n");
        return;
    }

    std::cout << "\n--- Inventory ---\n";
    for (std::size_t i = 0; i < items.size(); ++i)
    {
        const auto &it = items[i];
        std::cout << (i + 1) << ") "
                  << it.name << " x" << it.quantity
                  << " - " << it.description << "\n";
    }
    std::cout << "0) Cancel\n";
    std::cout << "> ";

    std::string in;
    std::getline(std::cin, in);
    if (in.empty() || in == "0")
    {
        AddNewLogEntry(log, nameOf(player) + " decides not to use an item.\n");
        return;
    }

    int choice = 0;
    try
    {
        choice = std::stoi(in);
    }
    catch (...)
    {
        AddNewLogEntry(log, "Invalid item selection.\n");
        return;
    }

    if (choice < 1 || choice > static_cast<int>(items.size()))
    {
        AddNewLogEntry(log, "Invalid item selection.\n");
        return;
    }

    const Item &selected = items[choice - 1];

    if (selected.healAmount > 0)
    {
        int before = player.vit.health;
        player.heal(selected.healAmount);
        int healed = player.vit.health - before;

        AddNewLogEntry(log, nameOf(player) + " uses " + selected.name + " and heals " + std::to_string(healed) + " HP. " +
                                "HP " + std::to_string(player.vit.health) + "/" + std::to_string(player.vit.maxHealth) + "\n");
        // remove 1 from inventory
        player.inv.removeitem(selected.name, 1);
    }
    else
    {
        AddNewLogEntry(log, "That item can't be used right now.\n");
    }
}

//@brief: AI function to choose an action for a non-player character during combat
//@param self - The NPC character making the decision
//@param foe - The player character being targeted
//@version: 1.0
//@author: Sebastian Cardona
Action ai_choose(const NonPlayerCharacter & /*self*/, const PlayerCharacter & /*foe*/)
{
    int roll = roll_d(4);
    if (roll == 1)
        return {ActionType::Defend, "Defend"};
    else
        return {ActionType::Attack, "Attack"};
}

Action player_choose()
{
    while (true)
    {
        std::cout << "\nChoose action:\n"
                     "1) Attack\n"
                     "2) Defend\n"
                     "3) Use Range\n"
                     "4) Use Item\n> ";
        std::string in;
        std::getline(std::cin, in);
        if (in.empty())
            continue;

        switch (in[0])
        {
        case '1':
            return {ActionType::Attack, "Attack"};
        case '2':
            return {ActionType::Defend, "Defend"};
        case '3':
            return {ActionType::UseRange, "UseRange"};
        case '4':
            return {ActionType::UseItem, "UseItem"};
        default:
            std::cout << "Invalid choice\n";
        }
    }
}

//@brief: Add a new log entry to the combat handler's log
//@param handler - The combat handler managing the combat state
//@param entry - The log entry to be added
void AddNewLogEntry(std::vector<std::string> &log, const std::string &entry)
{
    log.push_back(entry);

    // Adding a cap  so the log doesn't grow too much
    const int MAX_LOG_ENTRIES = 50;
    if (log.size() > MAX_LOG_ENTRIES)
    {
        auto eraseBegin = log.begin();
        auto eraseEnd   = log.end() - static_cast<std::ptrdiff_t>(MAX_LOG_ENTRIES);
        log.erase(eraseBegin, eraseEnd); // Remove the oldest entry
    }
}

/*
//@brief: function to run combat engine
//@param player - The player character
//@param enemy - The NPC character currently in combat with player
//@version: 1.0
//@author: Sebastian Cardona
static void runCombat(Student& player, NonPlayerCharacter& enemy)
{
    bool playerTurn = true; // later: base on initiative
    bool playerDef  = false;
    bool enemyDef   = false;
    //remake to string stream if needed
    std::vector<std::string> log;

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

        log.clear();

        if (playerTurn)
        {

            playerDef = false;

            Action act = player_choose();

            if (act.type == ActionType::Attack)
            {
                resolve_melee(player, enemy, enemyDef, log);
            }
            else if (act.type == ActionType::Defend)
            {
                playerDef = true;
                AddNewLogEntry(log, nameOf(player) + " Defends.\n");
            }
            else if (act.type == ActionType::UseRange)
            {
                resolve_ranged(player, enemy, enemyDef, log);
            }
            else if (act.type == ActionType::UseItem)
            {
                resolve_inventory(player, log);
            }
            else
            {
                AddNewLogEntry(log, nameOf(player) + " hesitates.\n");
            }
        }
        else
        {
            // Enemies turn now separate
            enemyDef = false;

            Action act = ai_choose(enemy, player);
            std::cout << nameOf(enemy) << " chose: " << act.desc << "\n";

            if (act.type == ActionType::Attack)
            {
                resolve_melee(enemy, player, playerDef, log);
            }
            else if (act.type == ActionType::Defend)
            {
                enemyDef = true;
                AddNewLogEntry(log, nameOf(enemy) + " Defends.\n");
            }
            else if (act.type == ActionType::UseRange)
            {
                resolve_ranged(enemy, player, playerDef, log);
            }
            else
            {
                AddNewLogEntry(log, nameOf(enemy) + " does nothing.\n");
            }
        }
        //Displaying log after one round of combat
        for(const auto& entry: log)
        {
            std::cout << entry << "\n";
        }


        if (!player.isAlive())
        {
            std::cout << "\n>>> You died: Game over\n";
            break;
        }
        if (!enemy.isAlive())
        {
            std::cout << "\n>>> You killed: " << nameOf(enemy) << "\n";
            break;
        }

        playerTurn = !playerTurn;// changing turns
        print_status();
    }
}
*/