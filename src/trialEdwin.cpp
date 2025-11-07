#include "characters.h"
#include "combat.h"
#include <iostream>

using namespace std;

mt19937& rng() 
{
    static mt19937 gen(std::random_device{}()); // In an effort to limit the number of included libraries, use random_devices instead of time()
    return gen;
}

int main()
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

    cout << "Student " << Steve.name << " Stats:";
    cout << "\nStrength: " << Steve.att.strength;
    cout << "\nDexterity: " << Steve.att.dexterity;
    cout << "\nConstitution: " << Steve.att.constitution;
    cout << "\nMax Health: " << Steve.vit.maxHealth;
    cout << "\nArmor: " << Steve.def.armor;
    cout << "\nInitiative: " << Steve.cbt.initiative << endl;

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

    Zombie.dealMeleeDamage(Steve);
    Steve.dealMeleeDamage(Zombie);

    cout<< "\nAfter combat:";
    cout << "\nStudent " << Steve.name << " Health: " << Steve.vit.health;
    cout << "\nZombie Health: " << Zombie.vit.health << endl;

    delete allStatLines; 
    return 0;
}