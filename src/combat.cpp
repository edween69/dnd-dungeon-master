/*WIP combat state machine*/

/* NOTE: COMBAT IS NOT IMPLEMENTED HERE YET, TO TEST COMBAT SEE trialSebastian.cpp
#include "combat.h"
#include "rng.h"

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
    a.startDefense();
    log << a.getName() << " is defending.\n";
}

//keep
// AI engine
Action ai_choose(const Character& self, const Character& foe) 
{
    //Edit ai actions 25% chance do nothing
    if (roll_d(4) == 1)
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
                     "1) Melee Attack\n"
                     "3) Defend\n"
                     "4) Use Range\n"
                     "5) Use Item\n> ";

        string in;
        getline(cin, in);
        if (in.empty()) continue;
        
        switch (in[0])
        {
        case '1':
            return {ActionType::Attack, -1, "UseMelee"};
            break;
        case '2':
            return {ActionType::Defend, -1, "Defend"};
            break;
        case '3':
            return {ActionType::UseRange, -1, "useRange"};
            break;
        case '4':
            cout << "Inventory not implemented yet";
            return {ActionType::UseItem, -1, "useItem"}; //Need to create inventory
            break;
        //case '5':
            //return {ActionType::Flee, -1, "Flee"};
        default:
            cout << "Invalid Choice";
            break;
        }
    }
}

void startStats ()
{
    istringstream*  allStatLines = storeAllStatLines(openStartingStatsCSV());

    

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
        0 // Magic resist not used
    };

    CombatStats studentCombat = {
        5, // meleeDamage for now not defined in CSV yet
        3, // rangeDamage for now not defined in CSV yet
        getStatForCharacterID(allStatLines, "Student", CSVStats::INITIATIVE)
    };

    VitalStats studentVital = {
        getStatForCharacterID(allStatLines, "Student", CSVStats::MAX_HEALTH),
        getStatForCharacterID(allStatLines, "Student", CSVStats::MAX_HEALTH)
    };

    StatusEffects studentStatus = {};

    Student Steve("Steve",
        studentAttrs,
        studentDef,
        studentCombat,
        studentVital,
        studentStatus
    );

    Attributes ZombieAttrs = {
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::STR),
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::DEX),
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::CON),
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::WIS),
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::CHA),
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::INT)
    };
    DefenseStats ZombieDef = {
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::ARMOR),
        0 // Magic resist not used
    };
    CombatStats ZombieCombat = {
        4, // meleeDamage for now not defined in CSV yet
        0, // rangeDamage for now not defined in CSV yet
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::INITIATIVE)
    };
    VitalStats ZombieVital = {
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::MAX_HEALTH),
        getStatForCharacterID(allStatLines, "Zombie_Standard", CSVStats::MAX_HEALTH)
    };
    StatusEffects ZombieStatus = {};
    NonPlayerCharacter Zombie("Zombie",
        ZombieAttrs,
        ZombieDef,
        ZombieCombat,
        ZombieVital,
        ZombieStatus
    );
}

// Game state machine
void runCombat(Student& stnt,NonPlayerCharacter& zombie) 
{
    bool playerTurn = true;
    Action currentAction;
    stringstream combatLog;

    cout << "== Turn-Based Combat Demo  ==\n";
    cout << stnt.getName() << " vs. " << zombie.getName() << "\n";

    auto print_status = [&]() 
    {
        cout << "\n--------------------------------\n";
        cout << stnt.getName() << " HP " << stnt.vit.health << "/" << stnt.vit.maxHealth
                  << (stnt.statEff.defending ? " [DEFENDING]" : "") << "\n";
        cout << zombie.getName() << " HP " << zombie.vit.health << "/" << zombie.vit.maxHealth
                  << (zombie.statEff.defending ? " [DEFENDING]" : "") << "\n";
        cout << "--------------------------------\n";
    };

    print_status();

    while (stnt.isAlive() && zombie.isAlive()) {
        combatLog.str(""); combatLog.clear();

        // Reset defending each new turn
        if (playerTurn) stnt.endDefense();
        else zombie.endDefense();

        // ---- Choose Action ----
        if (playerTurn)
            currentAction = player_choose();
        else {
            currentAction = ai_choose(zombie, stnt);
            cout << zombie.getName() << " chose: " << currentAction.description << "\n";
        }

        //Resolve turn
        if (playerTurn) {
            if (currentAction.type == ActionType::UseMelee)
                apply_attack(stnt, zombie, combatLog);
            else if (currentAction.type == ActionType::UseRange)
                apply_ranged(stnt, zombie, combatLog);
            else if (currentAction.type == ActionType::Defend)
                apply_defend(stnt, combatLog);
        } else {
            if (currentAction.type == ActionType::UseMelee)
                apply_attack(zombie, stnt, combatLog);
            else if (currentAction.type == ActionType::UseRange)
                apply_ranged(zombie, stnt, combatLog);
            else if (currentAction.type == ActionType::Defend)
                apply_defend(zombie, combatLog);
        }

        

        std::cout << combatLog.str();

        //Check for End
        if (!stnt.isAlive()) {
            cout << "\n>>> You were defeated...\n"; 
            //Need to add end screen flag
            break;
        }
        if (!zombie.isAlive()) {
            cout << "\n>>> Victory! The " << zombie.getName() << " is destroyed.\n"; 
            stnt.endDefense();
            //return to normal screen flag
            break;
        }

        

        //End Turn
        playerTurn = !playerTurn;
        print_status();
    }

    cout << "\nThanks for playing\n";
    
}
*/