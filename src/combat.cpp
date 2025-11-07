/*WIP combat state machine*/
#include <iostream>  
#include <cstdlib>   
#include <string>        
#include <sstream>       
#include <random>        
#include <algorithm>     
#include <limits>    
#include <ctime>
#include <algorithm>

using namespace std;

struct RNG
{
    RNG() 
    {
        // Seed the random number generator once when RNG is created
        srand(static_cast<unsigned>(std::time(0)));
    }
    // Edit after finding the real stats
    // Generate a random integer between a and b 
    int range(int a, int b) 
    {
        return a + rand() % (b - a + 1);
    }
    //For determining AI actions
    bool chance(int percent) 
    {
        return range(1, 100) <= percent;
    }
} rng;

// Actor struct for player and zombie
struct Actor 
{
    string name;
    int maxHP, hp, attack, defense;
    bool defending = false;
    bool isAlive() const {return hp > 0;}

};

//Enumerated action types
enum class ActionType { Attack, Defend, UseRange, UseItem, Flee, None };

struct Action
{
    ActionType type = ActionType::None;
    int ItemNum = -1;
    string description;

};

static int clampi(int v, int lo, int hi)
{ 
    return max(lo, min(v, hi)); 
}

int calc_damage(const Actor& attacker, const Actor& defender) 
{
    int guard = defender.defending + (defender.defending ? defender.defending/2 + 3 : 0);
    int base = attacker.attack - guard;
    // small random spread
    base += rng.range(-2, 2);
    base = max(1, base); //Always deals at least 1 damage
    return base;
}

// Applying attack stats
void apply_attack(Actor& attacker, Actor& defender, std::stringstream& log) 
{
    int dmg = calc_damage(attacker, defender);
    defender.hp = clampi(defender.hp - dmg, 0, defender.maxHP);
    log << attacker.name << " attacks for " << dmg
        << " damage! " << defender.name << " HP: "
        << defender.hp << "/" << defender.maxHP << "\n";
}

//Applying defense stats
void apply_defend(Actor& a, std::stringstream& log) 
{
    a.defending = true;
    log << a.name << " is defending.\n";
}

// AI engine
Action ai_choose(const Actor& self, const Actor& foe) 
{
    //Edit ai actions 25% chance do nothing
    if (rng.chance(25))
        return {ActionType::None, -1, "Did Nothing"};
    else
        return {ActionType::Attack, -1, "Attack"};
}

//Edit player selection, WIP; will use switch later
Action player_choose() 
{
    while (true) 
    {
         std::cout << "\nChoose action:\n"
                     "1) Attack\n"
                     "2) Defend\n"
                     "3) Use Item (N/A)\n"
                     "4) Ranged (N/A)\n> ";

        std::string in;
        std::getline(std::cin, in);
        if (in.empty()) continue;
        
        switch (in[0])
        {
        case '1':
            return {ActionType::Attack, -1, "Attack"};
            break;
        case '2':
            return {ActionType::Defend, -1, "Defend"};
            break;
        case '3':
            cout << "Range not implemented yet";
            return {ActionType::UseRange, -1, "useRange"};
        case '4':
            cout << "Inventory not implemented yet";
            return {ActionType::UseItem, -1, "useItem"}; //Need to create inventory
        //case '5':
            //return {ActionType::Flee, -1, "Flee"};
        
        default:
            cout << "Invalid Choice";
            break;
        }
    }
}

// Game state machine
void runCombat() 
{
    
    //string name; int maxHP, hp, attack, defense;
    Actor player{"You", 40, 40, 10, 5}; //We need to send these values as parameters later
    Actor zombie{"Zombie", 34, 34, 9, 4};

    bool playerTurn = true;
    Action currentAction;
    stringstream combatLog;

    cout << "== Turn-Based Combat Demo  ==\n";
    cout << player.name << " vs. " << zombie.name << "\n";

    auto print_status = [&]() 
    {
        cout << "\n--------------------------------\n";
        cout << player.name << " HP " << player.hp << "/" << player.maxHP
                  << (player.defending ? " [DEFENDING]" : "") << "\n";
        cout << zombie.name << " HP " << zombie.hp << "/" << zombie.maxHP
                  << (zombie.defending ? " [DEFENDING]" : "") << "\n";
        cout << "--------------------------------\n";
    };

    print_status();

    while (player.isAlive() && zombie.isAlive()) {
        combatLog.str(""); combatLog.clear();

        // Reset defending each new turn
        if (playerTurn) player.defending = false;
        else zombie.defending = false;

        // ---- Choose Action ----
        if (playerTurn)
            currentAction = player_choose();
        else {
            currentAction = ai_choose(zombie, player);
            cout << zombie.name << " chose: " << currentAction.description << "\n";
        }

        //Resolve turn
        if (playerTurn) {
            if (currentAction.type == ActionType::Attack)
                apply_attack(player, zombie, combatLog);
            else if (currentAction.type == ActionType::Defend)
                apply_defend(player, combatLog);
        } else {
            if (currentAction.type == ActionType::Attack)
                apply_attack(zombie, player, combatLog);
            else if (currentAction.type == ActionType::Defend)
                apply_defend(zombie, combatLog);
        }

        std::cout << combatLog.str();

        //Check for End
        if (!player.isAlive()) {
            cout << "\n>>> You were defeated...\n"; //Need to add end screen
            break;
        }
        if (!zombie.isAlive()) {
            cout << "\n>>> Victory! The " << zombie.name << " is destroyed.\n"; //return to normal screen
            break;
        }

        //End Turn
        playerTurn = !playerTurn;
        print_status();
    }

    cout << "\nThanks for playing\n";
    
}