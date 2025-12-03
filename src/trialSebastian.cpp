//trialSebastian.cpp
//testing file
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include "characters.h"    
#include "savegame.h"


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

    Student player("Steve", studentAttrs, studentDef, studentCombat, studentVital, studentStatus);

    
    player.key1 = true;
    player.key2 = false;
    player.zombie1Defeated = true;
    player.zombie2Defeated = false;
    player.zombie3Defeated = true;

    //save testing
    saveSystem save;
    save.saveFromGame(
        "GYM_HALLWAY",
        player,
        player.zombie1Defeated,
        player.zombie2Defeated,
        player.zombie3Defeated
    );

    if (save.saveToFile("test_save.txt"))
        std::cout << "Game saved!\n";
    else
        std::cout << "Failed to save!\n";

    
    //Reset
    Student loadedPlayer("Steve", studentAttrs, studentDef, studentCombat, studentVital, studentStatus);

    loadedPlayer.key1 = false;
    loadedPlayer.key2 = false;

    bool z1 = false, z2 = false, z3 = false;
    std::string loadedScreen;

    //Loading
    saveSystem save2;
    if (save2.loadFromFile("test_save.txt"))
    {
        std::cout << "Game loaded!\n";
        save2.loadGame(loadedScreen, loadedPlayer, z1, z2, z3);
    }
    else
    {
        std::cout << "Failed to load!\n";
        return 0;
    }

    //testing
    std::cout << "\n--- LOADED DATA ---\n";
    std::cout << "Screen: " << loadedScreen << "\n";
    std::cout << "Player Name: " << loadedPlayer.name << "\n";
    std::cout << "Class: " << loadedPlayer.characterClass << "\n";
    std::cout << "HP: " << int(loadedPlayer.vit.health) << "/" << int(loadedPlayer.vit.maxHealth) << "\n";

    std::cout << "Key1: " << loadedPlayer.key1 << "\n";
    std::cout << "Key2: " << loadedPlayer.key2 << "\n";

    std::cout << "Zombie1: " << z1 << "\n";
    std::cout << "Zombie2: " << z2 << "\n";
    std::cout << "Zombie3: " << z3 << "\n";

    //Print inventory
    std::cout << "\nInventory:\n";
    for (auto& item : loadedPlayer.getInventory().getItems())
    {
        std::cout << "- " << item.name << " x" << int(item.quantity) << "\n";
    }

    return 0;
} 
