#include <fstream>
#include <filesystem>
#include "json.hpp"
#include  "raylib.h"
#include "characters.h"


#ifndef PROGRESSLOG_H
#define PROGRESSLOG_H

bool saveProgress(Character** entities, int currentSceneIndex, int activeEncounterID, 
    int savedPlayerSceneIndex, const std::map<int,bool>& battleWon, const std::vector<std::string>& collectedItems);

bool LoadProgress (Character**& ent, std::istringstream* aStatLines, int& currentSceneIndex, int& activeEncounterID, 
    int& savedPlayerSceneIndex, std::map<int,bool>& battleWon, std::vector<std::string>& collectedItems);

#endif //PROGRESSLOG_H