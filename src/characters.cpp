/*  File hold functions related to character creation and status.

    ***Add unique points -From Andrew - To Andrew
*/

#include "characters.h"

/**
 * @brief Loads starting character stats from character CSV file to be read by storeAllStatsLines()
 * @return StartingStatFile - string stream to be fed into storeAllStatsLines()
 */
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

/**
 * @brief Stores character starting starts from openStartingStatsCSV()
 * @param StartingStatFile - comes from openStartingStatsCSV(), carries character starting data
 * @return a new pointer
 */
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

/**
 * @brief 
 * @param allLines - string stream from storeAllStatLines()
 * @param characterID - name of the character ie student
 * @param stat - stat value index
 * @return stoi(cell) - is the value of the desired stat
 * @return -128 - error value
 */
std::int8_t getStatForCharacterID(std::istringstream* allLines, std::string characterID, CSVStats stat)
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
    return -128; // Return -128 if character ID or stat not found
}