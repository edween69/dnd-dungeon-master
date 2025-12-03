/*
savegame.h

*/

#include <iostream>
#include <string>
#include <vector>
#include "characters.h"
#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

class saveSystem
{
    //version 1
    public:

    //saving current screen
    std::string currentScreenID;

    //saving player stats and items
    std::string playerName;
    std::string playerClass;
    Attributes   att;
    DefenseStats def;
    VitalStats   vit;
    Weapons      wep;
    std::vector<Item> savedInventory;

    //player story progress
    bool key1 = false;
    bool key2 = false;
    bool zombie1defeated = false;
    bool zombie2defeated = false;
    bool zombie3defeated = false;

    //save game function
    void saveFromGame(
        const std::string& currentScreen,
        const PlayerCharacter& player,
        bool z1defeated,
        bool z2defeated,
        bool z3defeated
    );

    //load game function
    void loadGame(
        std::string& currentScreenOut,
        PlayerCharacter& playerOut,
        bool& z1defeatedOut,
        bool& z2defeatedOut,
        bool& z3defeatedOut
    ) const;

    // File I/O
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
};

#endif