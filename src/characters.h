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

#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include "raylib.h"
#ifndef CHARACTERS_H
#define CHARACTERS_H

enum CSVStats {STR=1, DEX, CON, WIS, CHA, INT, MAX_HEALTH, ARMOR, INITIATIVE};

/*  Mersenne Twister pseudo­random number engine. This part of the code acts similar to srand(), but it should be better memory wise (need to test).
    Comes from <random>.  */
std::mt19937& rng();
/*  example code:
auto& gen = rng();
std::uniform_int_distribution<int> d20(1,20);
value = d20(gen);  

In the main file:
std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}()); // In an effort t// In an effort to limit the number of included libraries, use random_devices instead of time()o limit the number of included libraries, use random_devices instead of time()
    return gen;
}  */

// For stats contained in the Character_Starting_Stats.csv file, do not initialize
// Structure to hold character attributes
struct Attributes 
{
    int strength;
    int dexterity;
    int constitution;
    int wisdom; //not important, no longer DND game
    int charisma; //not important, no longer DND game
    int intelligence; //not important, no longer DND game
};

// Structure to hold character defense stats
struct DefenseStats
{
    int armor;
    int magicResist; //not important, no longer DND game
};

// Structure to hold character combat stats
struct CombatStats 
{
    int meleeDamage;
    int rangeDamage;
    int initiative;
};

// Structure to hold weapon types
struct Weapons {
    int meleeWeapon;
    int rangeWeapon;
};

// Structure to hold character vital stats
struct VitalStats {
    int health;
    int maxHealth;
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
};


// @author: Edwin Baiden
// @brief: Base class for all characters in the game (including players and NPCs), containing common attributes and methods including health management and status effects.
// @version: 1.0
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

        // Check if character is alive
        //@brief: Returns true if the character's health is above zero, indicating they are alive.
        //@return: bool - true if alive, false if dead.
        bool isAlive() const 
        {
            return vit.health > 0;
        }

        // Apply damage to the character and ensure health doesn't drop below zero
        //@brief: Reduces the character's health by the specified damage amount.
        //@param damage - The amount of damage to apply.
        void takeDamage(int damage) 
        {
            vit.health -= damage;
            if (vit.health < 0)
            {
                vit.health = 0;
            }
        }

        // @brief: calculates and applies melee damage
        // @param enemy - target that will take damage
        void dealMeleeDamage (Character& enemy)
        {
            auto& gen = rng();
            std::uniform_int_distribution<int> d20(1,20), d6(1,6);
            this->cbt.meleeDamage = std::max(this->att.dexterity,this->att.strength) + this->wep.meleeWeapon;
            if (enemy.def.armor < d20(gen) + this->cbt.meleeDamage)
            {
                enemy.takeDamage(d6(gen) + this->cbt.meleeDamage);
            }
            // NEED TO ADD INFO TO INFORM SYSTEM/USER OF MISS
        }

        // @brief: calculates and applies range damage
        // @param enemy - target that will take damage
        void dealRangeDamage (Character& enemy)
        {
            auto& gen = rng();
            std::uniform_int_distribution<int> d20(1,20), d4(1,4);
            this->cbt.rangeDamage = std::max(this->att.dexterity,this->att.wisdom) + this->wep.rangeWeapon;
            if (enemy.def.armor < d20(gen) + this->cbt.rangeDamage)
            {
                enemy.takeDamage(d4(gen) + this->cbt.rangeDamage);
            }
            // NEED TO ADD INFO TO INFORM SYSTEM/USER OF MISS
        }

        // Heal the character and ensure health doesn't exceed maxHealth
        //@brief: Increases the character's health by the specified amount, up to their maximum health.
        //@param amount - The amount of health to restore.
        void heal(int amount) 
        {
            vit.health += amount;
            if (vit.health > vit.maxHealth) 
            {
                vit.health = vit.maxHealth;
            }
        }
        virtual const std::string& getName() const = 0;
};


//@author: Edwin Baiden
//@brief: Derived class representing player-controlled characters, with additional attributes such as name and character class.
//@version: 1.0
class PlayerCharacter : public Character 
{
    // Player-specific attributes
    public:
        std::string name;
        std::string characterClass; // e.g., Student, Rat, Professor, Atilla
        const std::string& getName() const override { return name; }

        // Constructor to initialize player character attributes
        PlayerCharacter(const std::string& playerName, const std::string& charClass, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : Character(true, attributes, defense, combat, vital, statusEffects), name(playerName), characterClass(charClass) {}

        // Additional player-specific methods can be added here
        virtual ~PlayerCharacter() = default; // Virtual destructor
};


//@author: Edwin Baiden
//@brief: Derived class representing non-player characters (NPCs), with additional attributes such as NPC type.
//@version: 1.0
class NonPlayerCharacter : public Character 
{
    // NPC-specific attributes
    public:
        std::string npcType; // e.g., Zombie, Civilian, Security
        const std::string& getName() const override { return npcType; }
        // Constructor to initialize NPC attributes
        NonPlayerCharacter(const std::string& type, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : Character(false, attributes, defense, combat, vital, statusEffects), npcType(type) {}

        // Additional NPC-specific methods can be added here
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

std::ifstream* openStartingStatsCSV();
std::istringstream* storeAllStatLines(std::ifstream* statsFile);
int getStatForCharacterID(std::istringstream* allLines, std::string characterID, CSVStats stat);

struct charCard 
{
    Rectangle defaultRow;
    Rectangle currentAnimationPos;
    Rectangle targetAnimationPos;
    Texture2D texture;
};

#endif // CHARACTERS_H
