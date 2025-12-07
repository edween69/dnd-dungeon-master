#include "progressLog.h"
using json = nlohmann::json;


bool saveProgress(Character** ent, int currentSceneIndex, int activeEncounterID, 
    int savedPlayerSceneIndex, const std::map<int,bool>& battleWon, const std::vector<std::string>& collectedItems)
{
    json j;
    j["player"]["class"] = dynamic_cast<PlayerCharacter*>(ent[0])->characterClass;
    j["player"]["name"] = dynamic_cast<PlayerCharacter*>(ent[0])->name;
    j["player"]["attributes"]["strength"] = ent[0]->att.strength;
    j["player"]["attributes"]["dexterity"] = ent[0]->att.dexterity;
    j["player"]["attributes"]["constitution"] = ent[0]->att.constitution;
    j["player"]["defenseStats"]["armor"] = ent[0]->def.armor;
    j["player"]["CombatStats"]["meleeDamage"] = ent[0]->cbt.meleeDamage;
    j["player"]["CombatStats"]["rangeDamage"] = ent[0]->cbt.rangeDamage;
    j["player"]["combatStats"]["initiative"] = ent[0]->cbt.initiative;
    j["player"]["weapons"]["meleeWeapon"] = dynamic_cast<PlayerCharacter*>(ent[0])->wep.meleeWeapon;
    j["player"]["weapons"]["rangeWeapon"] = dynamic_cast<PlayerCharacter*>(ent[0])->wep.rangeWeapon;
    j["player"]["vitalStats"]["health"] = ent[0]->vit.health;
    j["player"]["vitalStats"]["maxHealth"] = ent[0]->vit.maxHealth;
    j["player"]["inventory"] = json::array();
    for (const auto& item : dynamic_cast<PlayerCharacter*>(ent[0])->getInventory().getItems()) {
        json itemJson;
        itemJson["name"] = item.name;
        itemJson["healAmount"] = item.healAmount;
        itemJson["quantity"] = item.quantity;
        j["player"]["inventory"].push_back(itemJson);
    }

    j["player"]["keys"]["key1"] = dynamic_cast<PlayerCharacter*>(ent[0])->key1;
    j["player"]["keys"]["key2"] = dynamic_cast<PlayerCharacter*>(ent[0])->key2;
    j["player"]["zombiesDefeated"]["zombie1"] = dynamic_cast<PlayerCharacter*>(ent[0])->zombie1Defeated;
    j["player"]["zombiesDefeated"]["zombie2"] = dynamic_cast<PlayerCharacter*>(ent[0])->zombie2Defeated;
    j["player"]["zombiesDefeated"]["zombie3"] = dynamic_cast<PlayerCharacter*>(ent[0])->zombie3Defeated;
    j["world"]["currentSceneIndex"] = currentSceneIndex;
    j["world"]["activeEncounterID"] = activeEncounterID;
    j["world"]["savedPlayerSceneIndex"] = savedPlayerSceneIndex;
    j["world"]["collectedItems"] = collectedItems;

    if (ent[1]!= nullptr) 
    {
        j["combat"]["ZombieHP"] = ent[1]->vit.health;
        j["combat"]["ZombieMaxHP"] = ent[1]->vit.maxHealth;
    } else 
    {
        j["combat"]["ZombieHP"] = 0;
        j["combat"]["ZombieMaxHP"] = 0;
    }
    
    j["combat"]["battleWon"] = json::object();
    for (const auto& [encounterID, won] : battleWon) 
    {
        j["combat"]["battleWon"][std::to_string(encounterID)] = won;
    }


    ChangeDirectory(GetApplicationDirectory());
    if (!std::filesystem::exists("../dat/usrData")) 
    {
        std::filesystem::create_directory("../dat/usrData");
    }
    std::ofstream outFile("../dat/usrData/savegame.json");
    if (!outFile.is_open()) 
    {
        return false; // Failed to open file for writing
    }
    outFile << j.dump(4);
    outFile.close();
    return true;
}

bool LoadProgress (Character**& ent, std::istringstream* aStatLines, int& currentSceneIndex, int& activeEncounterID, 
    int& savedPlayerSceneIndex, std::map<int,bool>& battleWon, std::vector<std::string>& collectedItems)
{
    ent  = new Character*[2]{nullptr, nullptr};
    
    ChangeDirectory(GetApplicationDirectory());
    std::ifstream inFile("../dat/usrData/savegame.json");
    if (!inFile.is_open()) 
    {
        return false; // No save file found
    }

    json j;
    inFile >> j;
    inFile.close();

    
    if (j["player"]["class"]== "Student") 
    {
       CreateCharacter(ent, aStatLines, "Student", j["player"]["name"].get<std::string>());
    }else if (j["player"]["class"]== "Rat")
    {
        CreateCharacter(ent, aStatLines, "Rat", j["player"]["name"].get<std::string>());
    }else if (j["player"]["class"]== "Professor") 
    {
        CreateCharacter(ent, aStatLines, "Professor", j["player"]["name"].get<std::string>());
    }else if (j["player"]["class"]== "Attila") 
    {
        CreateCharacter(ent, aStatLines, "Attila", j["player"]["name"].get<std::string>());
    }
    ent[0]->att.strength = j["player"]["attributes"]["strength"].get<std::int8_t>();
    ent[0]->att.dexterity = j["player"]["attributes"]["dexterity"].get<std::int8_t>();
    ent[0]->att.constitution = j["player"]["attributes"]["constitution"].get<std::int8_t>();
    ent[0]->def.armor = j["player"]["defenseStats"]["armor"].get<std::int8_t>();
    ent[0]->cbt.meleeDamage = j["player"]["CombatStats"]["meleeDamage"].get<std::uint8_t>();
    ent[0]->cbt.rangeDamage = j["player"]["CombatStats"]["rangeDamage"].get<std::uint8_t>();
    ent[0]->cbt.initiative = j["player"]["combatStats"]["initiative"].get<std::int8_t>();
    dynamic_cast<PlayerCharacter*>(ent[0])->wep.meleeWeapon = j["player"]["weapons"]["meleeWeapon"].get<std::uint8_t>();
    dynamic_cast<PlayerCharacter*>(ent[0])->wep.rangeWeapon = j["player"]["weapons"]["rangeWeapon"].get<std::uint8_t>();
    ent[0]->vit.health = j["player"]["vitalStats"]["health"].get<std::int8_t>();
    ent[0]->vit.maxHealth = j["player"]["vitalStats"]["maxHealth"].get<std::int8_t>();

    dynamic_cast<PlayerCharacter*>(ent[0])->getInventory().clearItems();
    for (const auto& itemJson : j["player"]["inventory"]) 
    {
        Item item;
        item.name = itemJson["name"].get<std::string>();
        item.healAmount = itemJson["healAmount"].get<std::uint8_t>();
        item.quantity = itemJson["quantity"].get<std::uint8_t>();
        dynamic_cast<PlayerCharacter*>(ent[0])->getInventory().additem(item);
    }
    dynamic_cast<PlayerCharacter*>(ent[0])->key1 = j["player"]["keys"]["key1"].get<bool>();
    dynamic_cast<PlayerCharacter*>(ent[0])->key2 = j["player"]["keys"]["key2"].get<bool>();
    dynamic_cast<PlayerCharacter*>(ent[0])->zombie1Defeated = j["player"]["zombiesDefeated"]["zombie1"].get<bool>();
    dynamic_cast<PlayerCharacter*>(ent[0])->zombie2Defeated = j["player"]["zombiesDefeated"]["zombie2"].get<bool>();
    dynamic_cast<PlayerCharacter*>(ent[0])->zombie3Defeated = j["player"]["zombiesDefeated"]["zombie3"].get<bool>();


    currentSceneIndex = j["world"]["currentSceneIndex"].get<int>();
    activeEncounterID = j["world"]["activeEncounterID"].get<int>();
    savedPlayerSceneIndex = j["world"]["savedPlayerSceneIndex"].get<int>();
    collectedItems = j["world"]["collectedItems"].get<std::vector<std::string>>();

    if (ent[1]== nullptr) 
    {
        CreateCharacter(ent, aStatLines, "Zombie_Standard", "Zombie");
    }
    ent[1]->vit.health = j["combat"]["ZombieHP"].get<std::int8_t>();
    ent[1]->vit.maxHealth = j["combat"]["ZombieMaxHP"].get<std::int8_t>();

    battleWon.clear();
    for (auto& [encounterIDStr, won] : j["combat"]["battleWon"].items()) 
    {
        battleWon[std::stoi(encounterIDStr)] = won.get<bool>();
    }
    return true;
}