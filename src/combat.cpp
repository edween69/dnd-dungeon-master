/*WIP combat state machine*/
#include "combat.h"

using namespace std;

static int clampi(int v, int lo, int hi) //remove
{ 
    return max(lo, min(v, hi)); 
}

int calc_damage(const Actor& attacker, const Actor& defender) 
{
    if (ActionType::type == Attack)
    {
        player.dealMeleeDamage(Zombie);
    }
    else
    {
        player.dealRangedDamage(Zombie);
    }
    
    /*int guard = defender.defending + (defender.defending ? defender.defending/2 + 3 : 0);
    int base = attacker.attack - guard;
    // small random spread
    base += rng.range(-2, 2);
    base = max(1, base); //Always deals at least 1 damage
    return base;*/
}

//deal melee damage
// Applying attack stats
void apply_attack(Actor& attacker, Actor& defender, std::stringstream& log) 
{
    int dmg = calc_damage(attacker, defender);
    defender.hp = clampi(defender.hp - dmg, 0, defender.maxHP);
    log << attacker.name << " attacks for " << dmg
        << " damage! " << defender.name << " HP: "
        << defender.hp << "/" << defender.maxHP << "\n";
}
//character.h
//Applying defense stats
void apply_defend(Actor& a, std::stringstream& log) 
{
    a.defending = true;
    log << a.name << " is defending.\n";
}
//keep
// AI engine
Action ai_choose(const Actor& self, const Actor& foe) 
{
    //Edit ai actions 25% chance do nothing
    if (AIrng.chance(25))
        return {ActionType::None, -1, "Did Nothing"};
    else
        return {ActionType::Attack, -1, "Attack"};
}
//keep
//Edit player selection, WIP; will use switch later
Action player_choose() 
{
    while (true) 
    {
         cout << "\nChoose action:\n"
                     "1) Attack\n"
                     "2) Defend\n"
                     "3) Use Range\n"
                     "4) Use Item\n> ";

        string in;
        getline(cin, in);
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
    //hp = character.vit.health() 
    //  maxhp = character.vit.maxHealth()
    // attack = character.
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
            cout << "\n>>> You were defeated...\n"; 
            //Need to add end screen flag
            break;
        }
        if (!zombie.isAlive()) {
            cout << "\n>>> Victory! The " << zombie.name << " is destroyed.\n"; 
            //return to normal screen flag
            break;
        }

        //End Turn
        playerTurn = !playerTurn;
        print_status();
    }

    cout << "\nThanks for playing\n";
    
}