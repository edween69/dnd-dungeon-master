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

#include "characters.h"
#include <iostream>
using namespace std;

// Base Character Class
class Character {
    public:
        bool isPlayer; // true if player, false if NPC
        bool isAlive;

        //character stats
        int health;
        int armor;
        int armorClass; //armorclass = flat num + &armor + &dexterity
        int magicResist;
        int meleeDamage;
        int rangeDamage;
        int initiative;


        //character attributes
        int strength;
        int dexterity;
        int constitution;
        int wisdom;
        int charisma;

        // status effects
        bool isPoisoned;
        bool isBurning;
        bool isWeakened;
        bool isStrengthened;
        bool isRegenerating;
        bool isSlowed;
        bool isFast;

        //Constructor
        Character(bool player, int hp, int arm, int armClass, int mResist, int mDamage, int rDamage, int init,
                  int str, int dex, int con, int wis, int cha)
            : isPlayer(player), isAlive(true), health(hp), armor(arm), armorClass(armClass), magicResist(mResist),
              meleeDamage(mDamage), rangeDamage(rDamage), initiative(init),
              strength(str), dexterity(dex), constitution(con), wisdom(wis), charisma(cha),
              isPoisoned(false), isBurning(false), isWeakened(false), isStrengthened(false),
              isRegenerating(false), isSlowed(false), isFast(false) {}



};

