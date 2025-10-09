/*

 Game Theme: Escape the Stevens campus during a zombie outbreak. Each character will have a different reason  for escaping and unique abilities to help them survive. The zombies were created by the biochem department and are roaming the campus. The goal is to reach the path to NY  while avoiding or defeating zombies.
 This file contains the implementation of character-related classes and functions.
 It includes definitions for character attributes, behaviors, and interactions.
 Caste:
    - Student: Hope to turn in their final project before they flee school. Melee: ruler and trashcan lid. Range: Textbooks.
    - Rat: Hope to make it to Benny for some pizza. Melee: Italian Stiletto & Bite. Range: Water Gun filled with Hudson River Water[Deals poison damage].
    - Professor: Hope to grab the antidote for the zombie virus they created. Melee(Touch Magic): Taser, Poison Needle. Range: fireball spell(molotov cocktail in a handle of fireball), 200 Watt Laser. 
    - Atilla: Hopes to save Tillie the dog and make it off campus safely. Melee: Feathers of Fury(fists). Range: Rubber Duckies.
*/

#include <string>
#include <iostream>
#ifndef CHARACTERS_H
#define CHARACTERS_H


// Structure to hold character attributes
struct Attributes 
{
    int strength;
    int dexterity;
    int constitution;
    int wisdom;
    int charisma;
    int intelligence;
};

// Structure to hold character defense stats
struct DefenseStats
{
    int armor;
    int armorClass; //armorclass = flat num + &armor + &dexterity
    int magicResist;
};

// Structure to hold character combat stats
struct CombatStats 
{
    int meleeDamage;
    int rangeDamage;
    int initiative;
};

// Structure to hold character vital stats
struct VitalStats {
    int health = 100;
    int maxHealth = 100;
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
            att.strength = 2;
            att.intelligence = 1;
            att.dexterity = 1;
            att.constitution = 1;
            att.wisdom = 0;
            att.charisma = 0;
            def.armor = 2; // Ruler and Trashcan Lid
            def.armorClass = 10 + def.armor + att.dexterity;
            def.magicResist = 1;
            cbt.meleeDamage = 5; // Ruler and Trashcan Lid
            cbt.rangeDamage = 3; // Textbooks
            cbt.initiative = 1;
            vit = VitalStats();
            statEff = StatusEffects();
        };

};

class Rat : public PlayerCharacter 
{
    public:
        Rat(const std::string& playerName, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : PlayerCharacter(playerName, "Rat", attributes, defense, combat, vital, statusEffects)

        {
            // Default attributes for Rat character
            att.strength = 1;
            att.intelligence = 1;
            att.dexterity = 2;
            att.constitution = 1;
            att.wisdom = 0;
            att.charisma = 0;
            def.armor = 1; // Rat Fur
            def.armorClass = 10 + def.armor + att.dexterity;
            def.magicResist = 1;
            cbt.meleeDamage = 4; // Italian Stiletto & Bite
            cbt.rangeDamage = 3; // Water Gun filled with Hudson River Water[Deals poison damage]
            cbt.initiative = 2;
            vit = VitalStats();
            statEff = StatusEffects();
        };

};

class Professor : public PlayerCharacter 
{
    public:
        Professor(const std::string& playerName, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : PlayerCharacter(playerName, "Professor", attributes, defense, combat, vital, statusEffects)

        {
            // Default attributes for Professor character
            att.strength = 1;
            att.intelligence = 2;
            att.dexterity = 1;
            att.constitution = 1;
            att.wisdom = 1;
            att.charisma = 0;
            def.armor = 1; // Lab Coat
            def.armorClass = 10 + def.armor + att.dexterity;
            def.magicResist = 2;
            cbt.meleeDamage = 4; // Taser, Poison Needle
            cbt.rangeDamage = 5; // fireball spell(molotov cocktail in a handle of fireball), 200 Watt Laser
            cbt.initiative = 1;
            vit = VitalStats();
            statEff = StatusEffects();
        };

};

class Atilla : public PlayerCharacter 
{
    public:
        Atilla(const std::string& playerName, Attributes attributes, DefenseStats defense, CombatStats combat, VitalStats vital, StatusEffects statusEffects)
            : PlayerCharacter(playerName, "Atilla", attributes, defense, combat, vital, statusEffects)

        {
            // Default attributes for Atilla character
            att.strength = 2;
            att.intelligence = 1;
            att.dexterity = 1;
            att.constitution = 2;
            att.wisdom = 0;
            att.charisma = 0;
            def.armor = 3; // Kevlar Vest
            def.armorClass = 10 + def.armor + att.dexterity;
            def.magicResist = 1;
            cbt.meleeDamage = 6; // Feathers of Fury(fists)
            cbt.rangeDamage = 4; // Rubber Duckies
            cbt.initiative = 1;
            vit = VitalStats();
            statEff = StatusEffects();
        };

};

#endif
