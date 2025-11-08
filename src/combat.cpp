/*WIP combat state machine*/
#include "combat.h"

using namespace std;

static int clampi(int v, int lo, int hi) //remove
{ 
    return max(lo, min(v, hi)); 
}

//deal melee damage
// Applying attack stats
void apply_attack(Character& attacker, Character& defender, std::stringstream& log) 
{
    int before = defender.vit.health; // read current HP from Character
    // Use Character's methods to apply the attack
    attacker.dealMeleeDamage(defender); // this calls defender.takeDamage(...)
    int after = defender.vit.health;
    int dmg = before - after;
    log << attacker.getName() << " attacks for " << dmg
        << " damage! " << defender.getName() << " HP: "
        << defender.vit.health << "/" << defender.vit.maxHealth << "\n";
}

void apply_ranged(Character& attacker, Character& defender, std::stringstream& log)
{
    int before = defender.vit.health;
    attacker.dealRangeDamage(defender);
    int after = defender.vit.health;
    int dmg = before - after;
    log << attacker.getName() << " fires for " << dmg
        << " damage! " << defender.getName() << " HP: "
        << defender.vit.health << "/" << defender.vit.maxHealth << "\n";
}

//character.h
//Applying defense stats
void apply_defend(Character& a, std::stringstream& log) 
{
    a.statEff.defending = true;
    log << a.getName() << " is defending.\n";
}

//keep
// AI engine
Action ai_choose(const Character& self, const Character& foe) 
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
    Character player{"You", 40, 40, 10, 5}; //We need to send these values as parameters later
    Character zombie{"Zombie", 34, 34, 9, 4}; 

    bool playerTurn = true;
    Action currentAction;
    stringstream combatLog;

    cout << "== Turn-Based Combat Demo  ==\n";
    cout << player.getName() << " vs. " << zombie.getName() << "\n";

    auto print_status = [&]() 
    {
        cout << "\n--------------------------------\n";
        cout << player.getName() << " HP " << player.vit.health << "/" << player.vit.maxHealth
                  << (player.statEff.defending ? " [DEFENDING]" : "") << "\n";
        cout << zombie.getName() << " HP " << zombie.vit.health << "/" << zombie.vit.maxHealth
                  << (zombie.statEff.defending ? " [DEFENDING]" : "") << "\n";
        cout << "--------------------------------\n";
    };

    print_status();

    while (player.isAlive() && zombie.isAlive()) {
        combatLog.str(""); combatLog.clear();

        // Reset defending each new turn
        if (playerTurn) player.statEff.defending = false;
        else zombie.statEff.defending = false;

        // ---- Choose Action ----
        if (playerTurn)
            currentAction = player_choose();
        else {
            currentAction = ai_choose(zombie, player);
            cout << zombie.getName() << " chose: " << currentAction.description << "\n";
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
            cout << "\n>>> Victory! The " << zombie.getName() << " is destroyed.\n"; 
            //return to normal screen flag
            break;
        }

        //End Turn
        playerTurn = !playerTurn;
        print_status();
    }

    cout << "\nThanks for playing\n";
    
}