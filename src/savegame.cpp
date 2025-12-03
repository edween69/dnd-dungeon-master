/*
savegame.cpp
*/
#include "savegame.h"
#include "characters.h"
#include <fstream>
#include <vector>
#include <limits>

void saveSystem::saveFromGame(const std::string& currentScreen, const PlayerCharacter& player,
    bool z1defeated, bool z2defeated, bool z3defeated)
{
    
    currentScreenID = currentScreen;

    //Player class
    playerName  = player.name;
    playerClass = player.characterClass;

    //Player stats
    att = player.att;
    def = player.def;
    vit = player.vit;
    wep = player.wep;

    //Story Keys
    key1 = player.key1;
    key2 = player.key2;

    //Player Inventory
    savedInventory = player.getInventory().getItems();

    //Story progress
    zombie1defeated = z1defeated;
    zombie2defeated = z2defeated;
    zombie3defeated = z3defeated;
}


// Push data from this saveSystem back into game objects
void saveSystem::loadGame(std::string& currentScreenOut, PlayerCharacter& playerOut,
    bool& z1defeatedOut, bool& z2defeatedOut, bool& z3defeatedOut) const
{
    currentScreenOut = currentScreenID;

    //Player identity
    playerOut.name = playerName;
    playerOut.characterClass = playerClass;

    //Stats
    playerOut.att = att;
    playerOut.def = def;
    playerOut.vit = vit;
    playerOut.wep = wep;

    //Keys
    playerOut.key1 = key1;
    playerOut.key2 = key2;

    //temporary inventory for loading
    inventory& invRef = playerOut.getInventory();
    invRef.clearItems();
    invRef.setItems(savedInventory);


    // Story flags
    z1defeatedOut = zombie1defeated;
    z2defeatedOut = zombie2defeated;
    z3defeatedOut = zombie3defeated;
}


bool saveSystem::saveToFile(const std::string& filename) const
{
    std::ofstream out(filename);
    if (!out) return false;

    //Current screen
    out << currentScreenID << "\n";

    //Player identity
    out << playerName  << "\n";
    out << playerClass << "\n";

    //Attributes
    out << int(att.strength)     << " "
        << int(att.dexterity)    << " "
        << int(att.constitution) << " "
        << int(att.wisdom)       << " "
        << int(att.charisma)     << " "
        << int(att.intelligence) << "\n";

    //Defense
    out << int(def.armor)       << " "
        << int(def.magicResist) << "\n";

    //Vital stats
    out << int(vit.health)    << " "
        << int(vit.maxHealth) << "\n";

    //Weapons
    out << int(wep.meleeWeapon) << " "
        << int(wep.rangeWeapon) << "\n";

    //Story Progress
    out << key1 << " "
        << key2 << " "
        << zombie1defeated << " "
        << zombie2defeated << " "
        << zombie3defeated << "\n";

    //Inventory
    out << savedInventory.size() << "\n";
    for (const auto& it : savedInventory)
    {
        out << it.name        << "\n";
        out << it.description << "\n";
        out << int(it.quantity)   << " "
            << int(it.healAmount) << " "
            << it.singleuse       << " "
            << it.consumed        << "\n";
    }

    return true;
}

bool saveSystem::loadFromFile(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in) return false;

    //Current screen
    std::getline(in, currentScreenID);
    if (!in) return false;

    //Player identity
    std::getline(in, playerName);
    std::getline(in, playerClass);
    if (!in) return false;

    int tmp;

    //Attributes
    in >> tmp; att.strength     = static_cast<std::int8_t>(tmp);
    in >> tmp; att.dexterity    = static_cast<std::int8_t>(tmp);
    in >> tmp; att.constitution = static_cast<std::int8_t>(tmp);
    in >> tmp; att.wisdom       = static_cast<std::int8_t>(tmp);
    in >> tmp; att.charisma     = static_cast<std::int8_t>(tmp);
    in >> tmp; att.intelligence = static_cast<std::int8_t>(tmp);

    //Defense
    in >> tmp; def.armor       = static_cast<std::int8_t>(tmp);
    in >> tmp; def.magicResist = static_cast<std::int8_t>(tmp);

    //Vital
    in >> tmp; vit.health    = static_cast<std::int8_t>(tmp);
    in >> tmp; vit.maxHealth = static_cast<std::int8_t>(tmp);

    //Weapons
    in >> tmp; wep.meleeWeapon = static_cast<std::uint8_t>(tmp);
    in >> tmp; wep.rangeWeapon = static_cast<std::uint8_t>(tmp);

    //Story Progress
    in >> key1 >> key2
       >> zombie1defeated
       >> zombie2defeated
       >> zombie3defeated;

    //Inventory
    size_t invCount = 0;
    in >> invCount;
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // eat endline after invCount

    savedInventory.clear();
    savedInventory.reserve(invCount);

    for (size_t i = 0; i < invCount; ++i)
    {
        Item it;
        std::string line;

        // name
        std::getline(in, it.name);
        // description
        std::getline(in, it.description);

        int qty, heal;
        bool singleUse, consumed;

        in >> qty >> heal >> singleUse >> consumed;
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        it.quantity   = static_cast<std::uint8_t>(qty);
        it.healAmount = static_cast<std::uint8_t>(heal);
        it.singleuse  = singleUse;
        it.consumed   = consumed;

        savedInventory.push_back(it);
    }

    return true;
}