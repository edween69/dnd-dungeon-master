/* ================================== progressLog.h =========================================================
    Project: TTRPG Game ?
    Subsystem: Progress Saving and Loading
    Primary Author: Edwin Baiden
    Description: Header file for progress saving and loading functions.
                This file declares functions to save and load game progress,
                including character stats, inventory, and world state using JSON format.
    
                    Functions:
                        - bool saveProgress(Character** entities, int currentSceneIndex, int activeEncounterID,
                        int savedPlayerSceneIndex, const std::map<int,bool>& battleWon, const std::vector<std::string>& collectedItems):
                        Saves the current game progress to a JSON file.
    
                        - bool LoadProgress (Character**& ent, std::istringstream* aStatLines, int& currentSceneIndex, int& activeEncounterID,
                        int& savedPlayerSceneIndex, std::map<int,bool>& battleWon, std::vector<std::string>& collectedItems):
                        Loads game progress from a JSON file into the provided character array and world state variables.
    
                    Uses the nlohmann/json library for JSON serialization and deserialization.

*/
//======================= STANDARD LIBRARY INCLUDES =======================
#include <fstream>
#include <filesystem>

//======================= PROJECT INCLUDES =======================
#include "json.hpp"
#include  "raylib.h"
#include "characters.h"

//=============== HEADER GUARD ===============
#ifndef PROGRESSLOG_H
#define PROGRESSLOG_H

//@brief: Saves the current game progress to a JSON file
//@version: 1.0
//@author: Edwin Baiden
//@param entities - Array of character pointers (player and enemy)
//@param currentSceneIndex - Index of the current scene
//@param activeEncounterID - ID of the active encounter
//@param savedPlayerSceneIndex - Index of the saved player scene
//@param battleWon - Map of encounter IDs to victory status
//@param collectedItems - List of collected item names
//@return - True if saving was successful, false otherwise
bool saveProgress(Character** entities, int currentSceneIndex, int activeEncounterID, 
    int savedPlayerSceneIndex, const std::map<int,bool>& battleWon, const std::vector<std::string>& collectedItems);

//@brief: Loads game progress from a JSON file into the provided character array and world state variables
//@version: 1.0
//@author: Edwin Baiden
//@param ent - Reference to an array of character pointers to populate
//@param aStatLines - Pointer to an array of string streams containing character stats
//@param currentSceneIndex - Reference to store the current scene index
//@param activeEncounterID - Reference to store the active encounter ID
//@param savedPlayerSceneIndex - Reference to store the saved player scene index
//@param battleWon - Reference to a map to populate with encounter victory status
//@param collectedItems - Reference to a vector to populate with collected item names
//@return - True if loading was successful, false otherwise

bool LoadProgress (Character**& ent, std::istringstream* aStatLines, int& currentSceneIndex, int& activeEncounterID, 
    int& savedPlayerSceneIndex, std::map<int,bool>& battleWon, std::vector<std::string>& collectedItems);

#endif //PROGRESSLOG_H