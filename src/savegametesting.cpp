#include <iostream>
#include "savegame.h"
#include "characters.h"

int main()
{
    //creating fake player
    Attributes att = {10, 5, 7, 0, 0, 0};
    DefenseStats def = {3, 0};
    CombatStats cbt = {0, 0, 5};
    VitalStats vit = {20, 30};
    StatusEffects se;
    Weapons wep = {2, 1};

    PlayerCharacter player("Sebastian", "Student", att, def, cbt, vit, se);

    
    Item potion;
    potion.name = "Health Potion";
    potion.description = "Restores 15 HP";
    potion.quantity = 2;
    player.getInventory().additem(potion);

    
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
    PlayerCharacter loadedPlayer("EMPTY", "NONE", att, def, cbt, vit, se);

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