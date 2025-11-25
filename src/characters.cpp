/*  File hold functions related to character creation and status.

    ***Add unique points -From Andrew - To Andrew
*/

#include "characters.h"

std::ifstream* openStartingStatsCSV()
{
    //Using filesystem to get path of the CSV relative to the executable
    ChangeDirectory(GetApplicationDirectory());
    std::ifstream* StartingStatFile = new std::ifstream("../dat/Character_Starting_Stats.csv");
    //std::ifstream* StartingStatFile = new std::ifstream("dat/Character_Starting_Stats.csv"); // In the form: ID,Strength,Dexterity,Constitution,Wisdom,Charisma,Intelligence,Max_Health,Armor,Initiative
    if (!StartingStatFile->is_open()) 
    {
        std::cerr << "Error: Could not open the character starting stats file." << std::endl;
    }

    return StartingStatFile;
}

std::istringstream* storeAllStatLines(std::ifstream* StartingStatFile)
{
    if (!StartingStatFile) return nullptr;
    std::string allLines;
    std::string line;
    int count = 1;

    while (std::getline(*StartingStatFile, line)) 
    {
        if (count != 1) 
        {
            allLines += line + "\n";
        }
        count++;
    }

    StartingStatFile->close();
    delete StartingStatFile;
    StartingStatFile = nullptr;

    return new std::istringstream(allLines);
}

int getStatForCharacterID(std::istringstream* allLines, std::string characterID, CSVStats stat)
{
    std::string line;
    std::string currentID;
    allLines->clear();
    allLines->seekg(0,std::ios::beg);
    while (std::getline(*allLines, line)) 
    {
        std::istringstream lineStream(line);
        std::string cell;
        std::getline(lineStream, cell, ',');
        currentID = cell;

        if (currentID == characterID) 
        {
            int currentStatIndex = 0;
            while (std::getline(lineStream, cell, ',')) 
            {
                currentStatIndex++;
                if (currentStatIndex == static_cast<int>(stat)) 
                {
                    return stoi(cell);
                }
            }
        }
    }
    return -1024; // Return -1024 if character ID or stat not found
}