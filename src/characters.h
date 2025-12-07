/*

 Game Theme: Escape the Stevens campus during a zombie outbreak. Each character will have a different reason 
 for escaping and unique abilities to help them survive. The zombies were created by the biochem department and 
 are roaming the campus. The goal is to reach the path to NY  while avoiding or defeating zombies.
 This file contains the implementation of character-related classes and functions.
 It includes definitions for character attributes, behaviors, and interactions.
 Caste:
    - Student: Hope to turn in their final project before they flee school. Melee: ruler and trashcan lid. Range: Textbooks.
    - Rat: Hope to make it to Benny for some pizza. Melee: Italian Stiletto & Bite. Range: Water Gun filled with Hudson River Water[Deals poison damage].
    - Professor: Hope to grab the antidote for the zombie virus they created. Melee(Touch Magic): Taser, Poison Needle. Range: fireball spell(molotov cocktail in a handle of fireball), 200 Watt Laser. 
    - Atilla: Hopes to save Tillie the dog and make it off campus safely. Melee: Feathers of Fury(fists). Range: Rubber Duckies.
*/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include "rng.h"
#include "raylib.h"
#ifndef CHARACTERS_H
#define CHARACTERS_H

enum CSVStats {STR=1, DEX, CON, WIS, CHA, INT, MAX_HEALTH, ARMOR, INITIATIVE};

// For stats contained in the Character_Starting_Stats.csv file, do not initialize
// Structure to hold character attributes
struct Attributes 
{
    std::int8_t strength = 0;
    std::int8_t dexterity = 0;
    std::int8_t constitution = 0;
    std::int8_t wisdom = 0; //not important, no longer DND game
    std::int8_t charisma = 0; //not important, no longer DND game
    std::int8_t intelligence = 0; //not important, no longer DND game
};

// Structure to hold character defense stats
struct DefenseStats
{
    std::int8_t armor = 0;
    std::int8_t magicResist = 0; //not important, no longer DND game
};

// Structure to hold character combat stats
struct CombatStats 
{
    std::uint8_t meleeDamage = 0;
    std::uint8_t rangeDamage = 0;
    std::int8_t initiative = 0;
};

// Structure to hold weapon types
struct Weapons {
    std::uint8_t meleeWeapon = 0;
    std::uint8_t rangeWeapon = 0;
};

// Structure to hold character vital stats
struct VitalStats {
    std::int8_t health = 0;
    std::int8_t maxHealth = 0;
};

// Structure to hold character status effects
struct StatusEffects 
{
    bool isPoisoned = false;
    bool isBurning = false;
    bool isWeakened = false;
    bool isStrengthened = false;
    bool isRegenerating = false;
    bool isSlowed = false;
    bool isFast = false;
    bool defending = false;
};

// Data Structure to hold character inventories, WIP
struct Item
{
    std::string name;
    std::string description;
    std::uint8_t quantity = 1;
    std::uint8_t healAmount = 0;
    bool singleuse = false; 
    bool consumed = false;
};

//Consumable based items
struct Consumable: Item
{
    
    //int attackboost = 0;
    //int defenseboost = 0;
    Consumable() {singleuse=true;}
};

//Health potion struct
struct HealthPotion: Consumable
{
    HealthPotion(int amount = 15)
    {
        name = "Health Potion";
        description = "A strange liquid, restores 15HP";
        healAmount = amount;
        quantity = 1;
    }
};

//Inventory class for storing the different items in a vector, currently only health potions are implemented.
//Also allows for easier inventory management with add/remove item functions
class inventory
{
    public: 
        //clear items when loading new game 
        void clearItems()
        {
            items.clear();
        }
        //setting player items when loading game
        void setItems(const std::vector<Item>& newItems)
        {
            items = newItems;
        }
    
        void additem(const Item& item)
        {
            for(auto& it : items)
            {
                if(it.name == item.name && it.healAmount == item.healAmount) 
                {
                it.quantity += item.quantity;
                return;
                }
            }
            items.push_back(item);
        }

        bool removeitem(const std::string& name, int qty = 1)
        {
            for (auto it = items.begin(); it != items.end(); ++it) 
            {
                if (it->name == name) 
                {
                    if (it->quantity < qty)
                        return false;  

                    it->quantity -= qty;

                    if (it->quantity <= 0)
                        items.erase(it);

                    return true;        
                }
            }
            return false; // item not found
        }
        const std::vector<Item>& getItems() const
        {
            return items;
        }
    private:
        std::vector<Item> items;

};

/**
 * @author: Edwin Baiden
 * @brief: Base class for all characters in the game (including players and NPCs), containing common attributes and methods including health management and status effects.
 * @version: 1.0
 */
class Character 
{
    public:
        // Initial attributes for all characters
        bool isPlayer; // true if player, false if NPC
        Attributes att;
        DefenseStats def;
        CombatStats cbt;
        VitalStats vit;
        StatusEffects statEff;
        Weapons wep;
        
        
        
        // Constructor to initialize all attributes
        Character(bool player, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : isPlayer(player), att(attributes), def(defense), cbt(combat), vit(vital), statEff(statusEffects) {}

        virtual ~Character() = default; // Virtual destructor for proper cleanup of derived classes

        /**
         * @brief: Returns true if the character's health is above zero, indicating they are alive.
         * @return: bool - true if alive, false if dead.
         */
        bool isAlive() const 
        {
            return vit.health > 0;
        }

        /**
         * @brief: Reduces the character's health by the specified damage amount.
         * @param damage - The amount of damage to apply.
         */
        void takeDamage(int damage) 
        {
            vit.health -= damage;
            if (vit.health < 0)
            {
                vit.health = 0;
            }
        }

        /**
         * @author: Andrew
         * @brief: calculates and applies melee damage
         * @param enemy - target that will take damage
         */
        void dealMeleeDamage (Character& enemy)
        {
            this->cbt.meleeDamage = std::max(this->att.dexterity,this->att.strength) + this->wep.meleeWeapon;
            if (enemy.def.armor < roll_d(20) + this->cbt.meleeDamage)
            {
                enemy.takeDamage(roll_d(6) + this->cbt.meleeDamage);
            }
        }

        /**
         * @author: Andrew
         * @brief: calculates and applies range damage
         * @param enemy - target that will take damage
         */
        void dealRangeDamage (Character& enemy)
        {
            this->cbt.rangeDamage = std::max(this->att.dexterity,this->att.wisdom) + this->wep.rangeWeapon;
            if (enemy.def.armor < roll_d(20) + this->cbt.rangeDamage)
            {
                enemy.takeDamage(roll_d(4) + this->cbt.rangeDamage);
            }
        }

        /**
         * @author: Andrew
         * @brief: adds defense bonus
         */
        void startDefense() 
        {
            statEff.defending = true;
            this->def.armor += 5;
           
        }

        /**
         * @author: Andrew
         * @brief: removes defense bonus
         */
        void endDefense() 
        {
            if(statEff.defending) { this->def.armor -= 5; }
            statEff.defending = false;
        }

        
        
        
        virtual const std::string& getName() const = 0; // Pure virtual function to get character name must be implemented by derived classes
};

/**
 * @author: Edwin Baiden
 * @brief: Derived class representing player-controlled characters, with additional attributes such as name and character class.
 * @version: 1.0
 */
class PlayerCharacter : public Character 
{
    // Player-specific attributes
    public:
        std::string name;
        std::string characterClass; // e.g., Student, Rat, Professor, Atilla
        bool key1 = false;
        bool key2 = false;
        bool zombie1Defeated = false;
        bool zombie2Defeated = false;
        bool zombie3Defeated = false;
        const std::string& getName() const override { return name; } // Override to return player character's name

        // Constructor to initialize player character attributes
        PlayerCharacter(const std::string& playerName, const std::string& charClass, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : Character(true, attributes, defense, combat, vital, statusEffects), name(playerName), characterClass(charClass) {}

        // Heal the character and ensure health doesn't exceed maxHealth
        //@brief: Increases the character's health by the specified amount, up to their maximum health.
        //@param amount - The amount of health to restore.
        inventory inv;

        void heal(int amount) 
        {
            vit.health += amount;
            if (vit.health > vit.maxHealth) 
            {
                vit.health = vit.maxHealth;
            }
        }

        
        inventory& getInventory() { return inv; } //getter for inventory
        const inventory& getInventory() const { return inv; }

        virtual ~PlayerCharacter() = default; // Virtual destructor can be overridden if needed
};

/**
 * @author: Edwin Baiden
 * @brief: Derived class representing non-player characters (NPCs), with additional attributes such as NPC type.
 * @version: 1.0
 */
class NonPlayerCharacter : public Character 
{
    // NPC-specific attributes
    public:
        std::string npcType; // e.g., Zombie, Civilian, Security
        const std::string& getName() const override { return npcType; } // Override to return NPC type as name
        // Constructor to initialize NPC attributes
        NonPlayerCharacter(const std::string& type, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : Character(false, attributes, defense, combat, vital, statusEffects), npcType(type) {}


        virtual ~NonPlayerCharacter() = default; // Virtual destructor
};

// Specific player character classes inheriting from PlayerCharacter
class Student : public PlayerCharacter
{
    public:
        Student(const std::string& playerName, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : PlayerCharacter(playerName, "Student", attributes, defense, combat, vital, statusEffects)

        {
            // Default attributes for Student character
            wep.meleeWeapon = 2; // ruler
            wep.rangeWeapon = 2; // textbooks
            HealthPotion Hpotion; //Adding potion to inventory for student for testing
            //Damageboost Dpotion
            inv.additem(Hpotion);
        };
};

class Rat : public PlayerCharacter 
{
    public:
        Rat(const std::string& playerName, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : PlayerCharacter(playerName, "Rat", attributes, defense, combat, vital, statusEffects)

        {
            // Default attributes for Rat character
            wep.meleeWeapon = 3; // Italian Stiletto & Bite
            wep.rangeWeapon = 1; // Water Gun filled with Hudson River Water[Deals poison damage]
        };
};

class Professor : public PlayerCharacter 
{
    public:
        Professor(const std::string& playerName, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : PlayerCharacter(playerName, "Professor", attributes, defense, combat, vital, statusEffects)

        {
            // Default attributes for Professor character
            wep.meleeWeapon = 3; // Taser, Poison Needle
            wep.rangeWeapon = 4; // fireball spell(molotov cocktail in a handle of fireball), 200 Watt Laser
        };
};

class Atilla : public PlayerCharacter 
{
    public:
        Atilla(const std::string& playerName, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : PlayerCharacter(playerName, "Atilla", attributes, defense, combat, vital, statusEffects)

        {
            // Default attributes for Atilla character
            wep.meleeWeapon = 1; // Feathers of Fury(fists)
            wep.rangeWeapon = 2; // Rubber Duckies
        };
};

// Class for default zombie type
class Zombie : public NonPlayerCharacter
{
    public:
        Zombie(Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : NonPlayerCharacter("Zombie", attributes, defense, combat, vital, statusEffects)
        {
            wep.meleeWeapon = 3;
            wep.rangeWeapon = 2; // default value, may not be used
            
        };
};

std::ifstream* openStartingStatsCSV();
std::istringstream* storeAllStatLines(std::ifstream* statsFile);
std::int8_t getStatForCharacterID(std::istringstream* allStats, std::string characterID, CSVStats stat);
void CreateCharacter(Character**& entities, std::istringstream* allStats, std::string ID, std::string name);

struct charCard 
{
    Rectangle defaultRow;
    Rectangle currentAnimationPos;
    Rectangle targetAnimationPos;
    Texture2D texture;
};

#endif // CHARACTERS_H
