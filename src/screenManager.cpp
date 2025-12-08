/* =================================== screenManager.cpp ====================================
    Project: TTRPG Game ?
    Subsystem: Screen Manager
    Primary Author: Edwin Baiden
    Description: This file defines the ScreenManager class and GameManager class to manage different game screens using raylib and raygui for rendering and GUI
                 elements. It also implements enums for screen states and game states as well as namespace functions for simple animations.

                 Screen States:
                    - MAIN_MENU: The main menu screen where players can start or exit the game.

                    - CHARACTER_SELECT: The character selection screen where players choose
                    their character. You can only select student for now.

                    - GAMEPLAY: The main gameplay screen (not fully implemented here, however
                    combat is being worked on). Values from this screen will be used to
                    determine player actions in combat.

                    - SAVE_QUIT: The save and quit screen (not fully implemented here).

                Each screen has its own styles(managed by functuons) and GUI resources
                (textures and rectangles, which are dynamically allocated and deallocated
                when entering and exiting screens).

                Game States:
                    - EXPLORATION: Exploration mode (not implemented here).

                    - COMBAT: Combat mode where players and enemies take turns attacking
                    each other.

                    - DIALOGUE: Dialogue mode for conversations (not implemented here).

                    - PAUSE_MENU: Pause menu mode (not implemented here).

                Screen Manager Functions:
                    - ScreenManager::ScreenManager(ScreenState initial): Constructor to initialize
                      the ScreenManager with an initial screen state.

                    - ScreenManager::~ScreenManager(): Destructor to clean up resources when the
                      ScreenManager is destroyed.

                    - void ScreenManager::init(): Initialize the screen manager by entering the initial screen.

                    - void ScreenManager::changeScreen(ScreenState newScreen): Request a screen change to a new screen state.

                    - ScreenState ScreenManager::getCurrentScreen() const: Get the current screen state.

                    - void ScreenManager::update(float dt): Update the current screen with delta time.

                    - void ScreenManager::render(): Render the current screen.

                    - void ScreenManager::enterScreen(ScreenState screen): Handle entering a new screen by loading
                      resources and setting styles.

                    - void ScreenManager::exitScreen(ScreenState screen): Handle exiting a screen by unloading
                      resources and cleaning up.

                Game Manager Functions: 
                    - GameManager::GameManager(GameState initial): Constructor to initialize the
*/

#define RAYGUI_IMPLEMENTATION
#include "screenManager.h"
#include "progressLog.h"


//======================= GLOBAL STATIC VARIABLES =======================
/*
    Global static variables to hold shared resources and game states.
    These persist through out the entireity of the game.

    - allStatLines: Holds all the lines of stats for characters from the CSV file  (this is then used to display stats and 
                                                                                    create new characters as the game goes on)
    
    - gameSounds: Holds all the sounds that will be used while the program is opened (on event that something happens 
                                                                                      the appropriate sound will be played)

    - ScreenTextures: Holds textures for different screens (these are dynamically allocated and deallocated
                                                            when entering and exiting screens)

    - ScreenRects: Holds rectangles for different screens (these are dynamically allocated and deallocated
                                                            when entering and exiting screens)

    - characterCards (Character Selection Only but needs to be available to render() and update() and stuff in that state):
    Holds  character chard data such as current position, target position, default row, and texture (needed for animation)

    - CharSelectionStuff (Character Selection Only but needs to be available to render() and update()): 
    Holds data on what character has been selected[0], whoch one is being hovered on[1], and if the character selection menu has been initialized[2]

    - scrollIntroCrawl(Intro Crawl screen only but needs to be available to render() and update()): 
    Holds the intro crawl text that will be scrolled up the screen in the intro crawl screen

    - entities: Holds the player and enemy entities for combat (these are dynamically allocated and deallocated
                                                            when entering and exiting screens; Player is at index 0, enemy is at index 1)
    
    - gameManager: Holds the game manager instance to manage game states and transitions

    - nerdFont: Holds the nerd font used for rendering text in the game

    NOTE: All pointers are initialized to nullptr and dynamically allocated when needed.
          They are cleaned up in the Cleanup functions. This is to avoid memory leaks and 
          ensure proper resource management(vectors would have been better but raw pointers were a req).

    - numScreenTextures: Holds the number of textures in ScreenTextures array
    - numScreenRects: Holds the number of rectangles in ScreenRects array
    - introCrawlYPos: Holds the current Y position of the intro crawl text for scrolling effect
    - gameScenes: Holds all the game scenes with their respective textures, arrows, items, and encounters (using a nodal mapping structure; not sure if this is the best way to do it)
    - activeEncounterID: Holds the ID of the currently active encounter
    - currentSceneIndex: Holds the index of the currently active scene in gameScenes
    - savedPlayerSceneIndex: Holds the index of the player's last saved scene
    - battleWon: Holds a map of encounter IDs to whether the player has won that encounter
    - collectedItems: Holds a vector of item names that the player has collected
    - byteSize: Holds the byte size of the icons that are used through out the game
    - loadedFromSave: Holds whether the game was loaded from a save file
    - savedSucessfully: Holds whether the game was saved successfully

*/ 
static std::istringstream *allStatLines = nullptr; // Used throughout game
static Sound *gameSounds = nullptr; // Used throughout game
static Texture2D *ScreenTextures = nullptr; // Used throughout game
static Rectangle *ScreenRects = nullptr; // Used throughout game
static charCard *characterCards = nullptr; // Used in Character Select state only
static int *CharSelectionStuff = nullptr; // Used in Character Select state only (holds selected character[0], hovered character[1], and initialized state[2])
static std::stringstream *scrollIntroCrawl = nullptr; // Used in Intro Crawl state only
static Character **entities = nullptr; // Used in Combat state only (Player at index 0, Enemy at index 1)
static GameManager *gameManager = nullptr; // Used throughout GAMEPLAY state
static Font *nerdFont = nullptr; // Used throughout game


static int numScreenTextures = 0;
static int numScreenRects = 0;
static float introCrawlYPos = 0.0f;
static int byteSize=0;

//Game scenes and related data (Please review above comment block)
static std::vector<GameScene> gameScenes;
static std::map<int,bool> battleWon;
static std::vector<std::string> collectedItems;
static bool loadedFromSave = false, savedSucessfully = false;
static int activeEncounterID = -1;
static int currentSceneIndex = TEX_ENTRANCE;
static int savedPlayerSceneIndex = TEX_ENTRANCE;

//======================= RESOURCE CLEANUP FUNCTIONS =======================
//These are a set of functions that delete/deallocate any and all dynamic resources being used in the game.

//@brief Safely cleans up all screen textures. This function checks if ScreenTextures is not null, then iterates through each texture, unloading them before deleting the array and setting the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupScreenTextures() 
{
    if (ScreenTextures) {
        for (int i = 0; i < numScreenTextures; ++i) {
            UnloadTexture(ScreenTextures[i]); // Unload each texture
        }
        delete[] ScreenTextures; // Delete the array of textures
        ScreenTextures = nullptr; // Set pointer to nullptr to avoid dangling pointer
    }
    numScreenTextures = 0; //Reset the count of screen textures
}

//@brief Safely cleans up all screen rectangles. This function checks if ScreenRects is not null, then deletes the array and sets the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupScreenRects() 
{
    if (ScreenRects) {
        delete[] ScreenRects; // Delete the array of rectangles
        ScreenRects = nullptr; // Set pointer to nullptr to avoid dangling pointer
    }
    numScreenRects = 0; // Reset the count of screen rectangles
}

//@brief Safely cleans up character cards. This function checks if characterCards is not null, then deletes the array and sets the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupCharacterCards() 
{
    if (characterCards) {
        delete[] characterCards; // Delete the array of character cards
        characterCards = nullptr; // Set pointer to nullptr
    }
}

//@brief Safely cleans up character selection stuff. This function checks if CharSelectionStuff is not null, then deletes the array and sets the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupCharSelectionStuff() 
{
    if (CharSelectionStuff) {
        delete[] CharSelectionStuff; // Delete the array of character selection stuff
        CharSelectionStuff = nullptr; // Set pointer to nullptr
    }
}

//@brief Safely cleans up stat lines. This function checks if allStatLines is not null, then clears the stringstream, deletes it, and sets the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupStatLines() 
{
    if (allStatLines) {
        allStatLines->clear(); // Clear the stringstream flags
        allStatLines->str(""); // Clear the stringstream content
        delete allStatLines; // Delete the stringstream
        allStatLines = nullptr; // Set pointer to nullptr
    }
}

//@brief Safely cleans up the intro crawl text. This function checks if scrollIntroCrawl is not null, then clears the stringstream, deletes it, and sets the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupIntroCrawl() 
{
    if (scrollIntroCrawl) {
        scrollIntroCrawl->clear(); // Clear the stringstream flags
        scrollIntroCrawl->str(""); // Clear the stringstream content
        delete scrollIntroCrawl;//Delete
        scrollIntroCrawl = nullptr;
    }
}

//@brief Safely cleans up nerd font. This function checks if nerdFont is not null, then unloads the font, deletes the pointer, and sets it to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupNerdFont() 
{
    if (nerdFont) {
        UnloadFont(*nerdFont); // Unload font
        delete nerdFont; // Delete
        nerdFont = nullptr;//Set to nullptr
    }
}

//@brief Safely cleans up game sounds. This function checks if gameSounds is not null, then iterates through each sound, unloading them before deleting the array and setting the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupGameSounds() 
{
    if (gameSounds) {
        for (int i = 0; i < TOTAL_SOUNDS; ++i) {
            UnloadSound(gameSounds[i]); // Unload each sound
        }
        delete[] gameSounds; // Delete the array of sounds
        gameSounds = nullptr; // Set pointer to nullptr
    }
}

//@brief Safely cleans up entities array. This function checks if entities is not null, then deletes each entity and sets pointers to nullptr before deleting the array and setting the pointer to nullptr.
//@version 1.0
//@author Edwin Baiden
void CleanupEntities() 
{
    if (entities) 
    {
        //If player character exists in array delete
        if (entities[0]) 
        {
            delete entities[0];
            entities[0] = nullptr;
        }

        //If enemy character exists in array delete
        if (entities[1]) 
        {
            delete entities[1];
            entities[1] = nullptr;
        }
        delete[] entities; // Delete the array of entities
        entities = nullptr;
    }
}

//@brief Safely cleans up all screen resources including textures, rects, character cards, selection state, and stat lines.
//@version 1.0
//@author Edwin Baiden
void CleanupAllScreenResources() 
{
    CleanupScreenTextures();
    CleanupScreenRects();
    CleanupCharacterCards();
    CleanupCharSelectionStuff();
    CleanupStatLines();
}


//======================= GAMESCENE FUNCTION DEFINITIONS =======================


//@brief Checks if an item has been collected by comparing the item name with the collected items list.
//@param itemName The name of the item to check for collection status.
//@return true if the item has been collected, false otherwise.
//@version 1.0
//@author Edwin Baiden
bool isItemCollected(const std::string& itemName) {
    for (const auto& item : collectedItems) // Iterate through collected items
        if (item == itemName) return true; // Item found, return true
    return false; // Item not found, return false
}

//@brief Initializes all game scenes with their respective textures, arrows (and where they lead to), items, and encounters.
//@param playerCharacter Pointer to the player character (used for scene initialization per character type [has not been fully implemented due to time]).
//@version 1.0
//@author Edwin Baiden
void InitGameScenes(Character* playerCharacter) 
{
    
    // Clear existing scenes if any
    gameScenes.clear();
    
    //Using dynamic_cast to check if the player character is of type Student, if yes then 
    if (dynamic_cast<Student*>(playerCharacter)) 
    {
        // Initialize scenes for Student character

        // Load screen textures
        numScreenTextures = TOTAL_EXP_TEX;
        ScreenTextures = new Texture2D[numScreenTextures];

        ChangeDirectory(GetApplicationDirectory()); // Change to application directory so that relative paths works (Cause MacOS)
        ScreenTextures[TEX_ENTRANCE] = LoadTexture("../assets/images/environments/Building1/Hallway/Entrance.png");
        ScreenTextures[TEX_EXIT] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-4].png");
        ScreenTextures[TEX_FRONT_OFFICE] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-2].png");
        ScreenTextures[TEX_WEST_HALLWAY_TOWARD] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-1].png");
        ScreenTextures[TEX_WEST_HALLWAY_AWAY] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[1-2].png");
        ScreenTextures[TEX_EAST_HALLWAY_TOWARD] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-3].png");
        ScreenTextures[TEX_EAST_HALLWAY_AWAY] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[3-1].png");
        ScreenTextures[TEX_CLASSROOM_1] = LoadTexture("../assets/images/environments/Building1/Class-Office/Classroom1.png");
        ScreenTextures[TEX_CLASSROOM_2] = LoadTexture("../assets/images/environments/Building1/Class-Office/Classroom2.png");
        ScreenTextures[TEX_CLASSROOM_3] = LoadTexture("../assets/images/environments/Building1/Class-Office/ClassroomZombies.png");
        ScreenTextures[TEX_IN_OFFICE] = LoadTexture("../assets/images/environments/Building1/Class-Office/Office.png");
        ScreenTextures[TEX_BATH_MEN] = LoadTexture("../assets/images/environments/Building1/Bathrooms/BathroomM.png");
        ScreenTextures[TEX_BATH_WOM] = LoadTexture("../assets/images/environments/Building1/Bathrooms/BathroomG.png");
        ScreenTextures[TEX_KEY_1] = LoadTexture("../assets/images/items/Key1.png");
        ScreenTextures[TEX_KEY_2] = LoadTexture("../assets/images/items/Key2.png");
        ScreenTextures[TEX_HEALTH_POTION] = LoadTexture("../assets/images/items/HealthPotion.png");
        ScreenTextures[TEX_BAT] = LoadTexture("../assets/images/items/BaseballBat.png");
        ScreenTextures[TEX_ARROW] = LoadTexture("../assets/images/UI/explorationArrow.png");
        ScreenTextures[TEX_MINIMAP] = LoadTexture("../assets/images/environments/Building1/NewLayout.png");
        ScreenTextures[TEX_TURTLE] = LoadTexture("../assets/images/UI/turtleIcon.png");

        // Initialize game scenes
        gameScenes.resize(TEX_BATH_WOM + 1);

        // Entrance Scene
        GameScene* s = &gameScenes[TEX_ENTRANCE];
        s->sceneName = "Entrance";
        s->textureIndex = TEX_ENTRANCE;
        s->minimapCoords = {0.475f, 0.8f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {
            //{{x, y, width, height}, direction, targetSceneIndex, isEnabled, hoverText, requiredKeyName}
            {{550, 500, 150, 150}, LEFT, TEX_WEST_HALLWAY_AWAY, true, "Go West", ""},
            {{1220, 500, 150, 150}, RIGHT, TEX_EAST_HALLWAY_TOWARD, true, "Go East", ""},
            {{885, 650, 150, 150}, UP, TEX_FRONT_OFFICE, true, "Go to Office Front", ""},
            {{885, 875, 150, 150}, DOWN, TEX_EXIT, true, "Exit Building", "Key 2"}
        };

        // Exit Scene
        s = &gameScenes[TEX_EXIT];
        s->sceneName = "Exit";
        s->textureIndex = TEX_EXIT;
        s->environmentTexture = "../assets/images/environments/Building1/Hallway/Hallway[2-4].png";
        s->minimapCoords = {0.5f, 0.825f};
        s->minimapRotation = 180.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_ENTRANCE, true, "Enter Building", ""}};
        s->hasEncounter = true;
        s->encounterID = 2;
        s->combatBgX = (SCREEN_CENTER_X - (ScreenTextures[TEX_EXIT].width) / 2.0f);
        s->combatBgY = (SCREEN_CENTER_Y - (ScreenTextures[TEX_EXIT].height) / 2.0f- 175.0f);
        s->playerCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_EXIT].width) / 2.0f - 450.0f);
        s->playerCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_EXIT].height) / 2.0f - 700.0f);
        s->enemyCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_EXIT].width) / 2.0f - 675.0f);
        s->enemyCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_EXIT].height) / 2.0f - 750.0f);
        s->playerScale = {600.0f,650.0f};
        s->enemyScale = {400.0f,500.0f};

        s = &gameScenes[TEX_FRONT_OFFICE];
        s->sceneName = "Office Front";
        s->textureIndex = TEX_FRONT_OFFICE;
        s->minimapCoords = {0.45f, 0.475f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {
            {{550, 725, 150, 150}, LEFT, TEX_WEST_HALLWAY_TOWARD, true, "Go West", ""},
            {{1250, 725, 150, 150}, RIGHT, TEX_EAST_HALLWAY_TOWARD, true, "Go East", ""},
            {{885, 875, 150, 150}, DOWN, TEX_EXIT, true, "Exit Building", "Key 2"},
            {{885, 650, 150, 150}, UP, TEX_IN_OFFICE, true, "Enter Office", ""}
        };

        s = &gameScenes[TEX_WEST_HALLWAY_TOWARD];
        s->sceneName = "West Hallway";
        s->textureIndex = TEX_WEST_HALLWAY_TOWARD;
        s->minimapCoords = {0.25f, 0.475f};
        s->minimapRotation = 270.0f;
        s->sceneArrows = {
            {{500, 535, 150, 150}, LEFT, TEX_CLASSROOM_1, true, "Enter Classroom 1", ""},
            {{1250, 535, 150, 150}, RIGHT, TEX_CLASSROOM_2, true, "Enter Classroom 2", "Key 1"},
            {{875, 750, 150, 150}, DOWN, TEX_WEST_HALLWAY_AWAY, true, "Return East", ""}
        };

        s = &gameScenes[TEX_WEST_HALLWAY_AWAY];
        s->sceneName = "West Hallway";
        s->textureIndex = TEX_WEST_HALLWAY_AWAY;
        s->minimapCoords = {0.2f, 0.475f};
        s->minimapRotation = 90.0f;
        s->sceneArrows = {
            {{855, 850, 150, 150}, DOWN, TEX_WEST_HALLWAY_TOWARD, true, "Return West", ""},
            {{855, 550, 150, 150}, UP, TEX_EAST_HALLWAY_TOWARD, true, "Go East", ""},
            {{500, 500, 150, 150}, LEFT, TEX_FRONT_OFFICE, true, "Go to Office Entrance", ""},
            {{1250, 500, 150, 150}, RIGHT, TEX_EXIT, true, "Exit Building", "Key 2"}
        };

        s = &gameScenes[TEX_EAST_HALLWAY_TOWARD];
        s->sceneName = "East Hallway";
        s->textureIndex = TEX_EAST_HALLWAY_TOWARD;
        s->minimapCoords = {0.675f, 0.475f};
        s->minimapRotation = 90.0f;
        s->sceneArrows = {
            {{885, 600, 150, 150}, UP, TEX_CLASSROOM_3, true, "Enter Classroom 3", ""},
            {{500, 600, 150, 150}, LEFT, TEX_BATH_MEN, true, "Enter Men's Bathroom", ""},
            {{1350, 600, 150, 150}, RIGHT, TEX_BATH_WOM, true, "Enter Women's Bathroom", ""},
            {{885, 850, 150, 150}, DOWN, TEX_EAST_HALLWAY_AWAY, true, "Go West", ""}
        };

        s = &gameScenes[TEX_EAST_HALLWAY_AWAY];
        s->sceneName = "East Hallway";
        s->textureIndex = TEX_EAST_HALLWAY_AWAY;
        s->minimapCoords = {0.7f, 0.5f};
        s->minimapRotation = 270.0f;
        s->sceneArrows = {
            {{855, 850, 150, 150}, DOWN, TEX_EAST_HALLWAY_TOWARD, true, "Return East", ""},
            {{855, 550, 150, 150}, UP, TEX_WEST_HALLWAY_TOWARD, true, "Go West", ""},
            {{1250, 500, 150, 150}, RIGHT, TEX_FRONT_OFFICE, true, "Go to Office Entrance", ""},
            {{550, 500, 150, 150}, LEFT, TEX_EXIT, true, "Go to Exit", "Key 2"}
        };

        s = &gameScenes[TEX_CLASSROOM_1];
        s->sceneName = "Classroom 1";
        s->textureIndex = TEX_CLASSROOM_1;
        s->environmentTexture = "../assets/images/environments/Building1/Class-Office/Classroom1.png";
        s->minimapCoords = {0.19f, 0.625f};
        s->minimapRotation = 180.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_WEST_HALLWAY_TOWARD, true, "Exit Classroom", ""}};
        s->sceneItems = {{"Key 2", "Pick up Key 2", {600, 625, 150, 150}, TEX_KEY_2, true}};
        s->hasEncounter = true;
        s->encounterID = 0;
        s->combatBgX = (SCREEN_CENTER_X - (ScreenTextures[TEX_CLASSROOM_1].width) / 2.0f);
        s->combatBgY = (SCREEN_CENTER_Y - (ScreenTextures[TEX_CLASSROOM_1].height) / 2.0f- 175.0f);
        s->playerCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_CLASSROOM_1].width) / 2.0f - 500.0f);
        s->playerCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_CLASSROOM_1].height) / 2.0f - 790.0f);
        s->enemyCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_CLASSROOM_1].width) / 2.0f - 670.0f);
        s->enemyCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_CLASSROOM_1].height) / 2.0f - 795.0f);
        s->playerScale = {600.0f,700.0f};
        s->enemyScale = {400.0f,400.0f};


        s = &gameScenes[TEX_CLASSROOM_2];
        s->sceneName = "Classroom 2";
        s->textureIndex = TEX_CLASSROOM_2;
        s->minimapCoords = {0.15f, 0.325f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_WEST_HALLWAY_TOWARD, true, "Exit Classroom", ""}};
        s->sceneItems = {{"Health Potion", "Pick up Health Potion", {500, 480, 150, 150}, TEX_HEALTH_POTION, false}};

        s = &gameScenes[TEX_CLASSROOM_3];
        s->sceneName = "Classroom 3";
        s->textureIndex = TEX_CLASSROOM_3;
        s->minimapCoords = {0.15f, 0.65f};
        s->minimapRotation = 90.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_EAST_HALLWAY_TOWARD, true, "Exit Classroom", ""}};

        s = &gameScenes[TEX_IN_OFFICE];
        s->sceneName = "Office";
        s->textureIndex = TEX_IN_OFFICE;
        s->environmentTexture = "../assets/images/environments/Building1/Class-Office/Office.png";
        s->minimapCoords = {0.45f, 0.35f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_FRONT_OFFICE, true, "Exit Office", ""}};
        s->sceneItems = {
            {"Key 1", "Pick up Key 1", {600, 400, 90, 90}, TEX_KEY_1, false},
            {"Baseball Bat", "Pick up Baseball Bat", {800, 500, 300, 150}, TEX_BAT, false}
        };
        s->hasEncounter = true;
        s->encounterID = 1;
        s->combatBgX = (SCREEN_CENTER_X - (ScreenTextures[TEX_IN_OFFICE].width) / 2.0f);
        s->combatBgY = (SCREEN_CENTER_Y - (ScreenTextures[TEX_IN_OFFICE].height) / 2.0f- 150.0f);
        s->playerCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_IN_OFFICE].width) / 2.0f - 500.0f);
        s->playerCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_IN_OFFICE].height) / 2.0f - 1075.0f);
        s->enemyCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_IN_OFFICE].width) / 2.0f - 700.0f);
        s->enemyCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_IN_OFFICE].height) / 2.0f - 1295.0f);
        s->playerScale = {700.0f,700.0f};
        s->enemyScale = {300.0f,500.0f};

        s = &gameScenes[TEX_BATH_MEN];
        s->sceneName = "Men's Bathroom";
        s->textureIndex = TEX_BATH_MEN;
        s->minimapCoords = {0.85f, 0.325f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_EAST_HALLWAY_TOWARD, true, "Exit Bathroom", ""}};

        s = &gameScenes[TEX_BATH_WOM];
        s->sceneName = "Women's Bathroom";
        s->textureIndex = TEX_BATH_WOM;
        s->environmentTexture = "../assets/images/environments/Building1/Class-Office/BathroomG.png";
        s->minimapCoords = {0.8f, 0.6f};
        s->minimapRotation = 180.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_EAST_HALLWAY_TOWARD, true, "Exit Bathroom", ""}};
        
    }
}

//======================= SOUND INITIALIZATION =======================
void InitGameSounds() 
{
    gameSounds = new Sound[TOTAL_SOUNDS];
    gameSounds[SND_SELECT] = LoadSound("../assets/sfx/select.wav");
    gameSounds[SND_HIT] = LoadSound("../assets/sfx/hitHurt.wav");
    gameSounds[SND_HEAL] = LoadSound("../assets/sfx/heal.wav");
    gameSounds[SND_ZOM_DEATH] = LoadSound("../assets/sfx/explosion.wav");
    gameSounds[SND_ZOM_GROAN] = LoadSound("../assets/sfx/zombieGroan.wav");
}

//======================= GUI STYLE FUNCTIONS =======================

void defaultStyles() {
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x828282FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0xB6B6B6FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xDADADAFF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xE0E0E0FF);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0xC4C4C4FF);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0xA8A8A8FF);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0x000000FF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0x000000FF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0x000000FF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
}

void startMenuStyles() {
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x646464FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x969696FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xC8C8C8FF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x000000B4);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x323232C8);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x646464DC);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 56);
}

void playerSelectStyles() {
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x006600FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x008800FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0x00CC00FF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x003300C8);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x006600DC);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 6);
    GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED, 0x555555FF);
    GuiSetStyle(BUTTON, BASE_COLOR_DISABLED, 0x222222B4);
    GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED, 0x888888FF);
}

void gamePlayStyles() {
    CleanupNerdFont();
    nerdFont = new Font();
    int codepoints[11] = {ICON_SWORD, ICON_BOW_ARROW, ICON_POISON, ICON_FIRE, ICON_ARROW_DOWN, 
                          ICON_ARROW_UP, ICON_PLUS, ICON_SNAIL, ICON_LIGHTNING, 
                          ICON_SHIELD, ICON_PAUSE};
    ChangeDirectory(GetApplicationDirectory());
    *nerdFont = LoadFontEx("../assets/fonts/JetBrainsMonoNLNerdFontMono-Bold.ttf", 32, codepoints, 11);
    SetTextureFilter(nerdFont->texture, TEXTURE_FILTER_BILINEAR);
    playerSelectStyles();
}



void getIntroCrawlText(std::stringstream *ss, int chosenCharacterIdx) {
    if (!ss) return;
    ss->str("");
    ss->clear();

    ChangeDirectory(GetApplicationDirectory());
    std::ifstream file("../dat/general_Intro.txt");
    if (!file.is_open()) {
        (*ss) << "Error: Unable to open intro crawl text file.";
        return;
    }

    std::string line;
    while (std::getline(file, line)) (*ss) << line << "\n\n";
    file.close();
    (*ss) << "\n\n\n";

    const char* charFiles[] = {
        "../dat/Student_Intro.txt",
        "../dat/Rat_Intro.txt",
        "../dat/Professor_Intro.txt",
        "../dat/Attila_Intro.txt"
    };

    if (chosenCharacterIdx >= 0 && chosenCharacterIdx < 4) {
        file.open(charFiles[chosenCharacterIdx]);
        if (file.is_open()) {
            while (std::getline(file, line)) (*ss) << line << "\n\n";
            file.close();
        }
    }
}
void DrawStatusPanel(const Rectangle &panel, const StatusEffects &entityStatEff, const Font &fnt) {
    struct StatusType { const char *Effect; std::string Icon; Color GoodOrBadEff; };
    std::vector<StatusType> activeStatEffects;

    if (entityStatEff.isPoisoned) activeStatEffects.push_back({"POISONED", CodepointToUTF8(ICON_POISON, &byteSize), RED});
    if (entityStatEff.isBurning) activeStatEffects.push_back({"BURNING", CodepointToUTF8(ICON_FIRE, &byteSize), RED});
    if (entityStatEff.isWeakened) activeStatEffects.push_back({"WEAKENED", CodepointToUTF8(ICON_ARROW_DOWN, &byteSize), RED});
    if (entityStatEff.isSlowed) activeStatEffects.push_back({"SLOWED", CodepointToUTF8(ICON_SNAIL, &byteSize), RED});
    if (entityStatEff.isStrengthened) activeStatEffects.push_back({"STRENGTHENED", CodepointToUTF8(ICON_ARROW_UP, &byteSize), GREEN});
    if (entityStatEff.isRegenerating) activeStatEffects.push_back({"REGENERATING", CodepointToUTF8(ICON_PLUS, &byteSize), GREEN});
    if (entityStatEff.isFast) activeStatEffects.push_back({"FAST", CodepointToUTF8(ICON_LIGHTNING, &byteSize), GREEN});
    if (entityStatEff.defending) activeStatEffects.push_back({"DEFENDING", CodepointToUTF8(ICON_SHIELD, &byteSize), GREEN});

    for (size_t i = 0; i < activeStatEffects.size(); ++i) {
        DrawTextEx(GetFontDefault(), activeStatEffects[i].Effect,
                   {panel.x + 8.0f, 
                    panel.y + 8.0f + (i * 28.0f) + ((28.0f - MeasureTextEx(GetFontDefault(), activeStatEffects[i].Effect, 24.0f, 1.0f).y) / 2.0f)},
                   24.0f, 1.0f, activeStatEffects[i].GoodOrBadEff);
        
        DrawTextEx(fnt, activeStatEffects[i].Icon.c_str(),
                   {panel.x + panel.width - 8.0f - MeasureTextEx(fnt, activeStatEffects[i].Icon.c_str(), 44.0f, 1.0f).x,
                    panel.y + 8.0f + (i * 28.0f) + ((28.0f - MeasureTextEx(fnt, activeStatEffects[i].Icon.c_str(), 44.0f, 1.0f).y) / 2.0f)},
                   44.0f, 1.0f, activeStatEffects[i].GoodOrBadEff);
    }
}

//=================== SCREENMANAGER CLASS ===================

ScreenManager::ScreenManager(ScreenState initial) : currentScreen(initial), scale(1.0f), offset{0.0f, 0.0f} {}

ScreenManager::~ScreenManager() {
    UnloadRenderTexture(target);
    exitScreen(currentScreen);
    
    // Clean up persistent resources that last the entire game session
    CleanupGameSounds();
    CleanupEntities();
    CleanupStatLines();
    CleanupNerdFont();
    CleanupIntroCrawl();
}

void ScreenManager::init() {
    ChangeDirectory(GetApplicationDirectory());
    target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
    InitGameSounds();
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    enterScreen(currentScreen);
}

void ScreenManager::changeScreen(ScreenState newScreen) {
    if (newScreen == currentScreen) return;
    exitScreen(currentScreen);
    currentScreen = newScreen;
    enterScreen(currentScreen);
}

ScreenState ScreenManager::getCurrentScreen() const {
    return currentScreen;
}

Vector2 ScreenManager::GetVirtualMousePosition() {
    Vector2 mouse = GetMousePosition();
    return {
        std::clamp((mouse.x - offset.x) / scale, 0.0f, (float)GAME_SCREEN_WIDTH),
        std::clamp((mouse.y - offset.y) / scale, 0.0f, (float)GAME_SCREEN_HEIGHT)
    };
}

void ScreenManager::update(float dt) {
    scale = std::min((float)GetScreenWidth() / GAME_SCREEN_WIDTH, (float)GetScreenHeight() / GAME_SCREEN_HEIGHT);
    offset = {((float)GetScreenWidth() - ((float)GAME_SCREEN_WIDTH * scale)) * 0.5f,
              ((float)GetScreenHeight() - ((float)GAME_SCREEN_HEIGHT * scale)) * 0.5f};

    switch (currentScreen) {
    case ScreenState::MAIN_MENU:
        break;

    case ScreenState::CHARACTER_SELECT: {
        if (!characterCards || !CharSelectionStuff) break;

        for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            characterCards[i].texture = ScreenTextures[i + 1];

        if (!CharSelectionStuff[2]) {
            for (int i = 0; i < MAX_CHAR_CARDS; ++i) {
                characterCards[i].defaultRow = {
                    CENTERED_X(MAX_CHAR_CARDS * CHARACTER_CARD_WIDTH + (MAX_CHAR_CARDS - 1) * CHARACTER_CARD_SPACING) + 
                        i * (CHARACTER_CARD_WIDTH + CHARACTER_CARD_SPACING),
                    CENTERED_Y(CHARACTER_CARD_HEIGHT),
                    CHARACTER_CARD_WIDTH,
                    CHARACTER_CARD_HEIGHT
                };
                
                //characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
                characterCards[i].currentAnimationPos = characterCards[i].defaultRow;
                //characterCards[i].currentAnimationPos.y = (float)GAME_SCREEN_HEIGHT + 200.0f;
            }
            CharSelectionStuff[2] = 1;
        }

        if (CharSelectionStuff[0] == -1) {
            for (int i = 0; i < MAX_CHAR_CARDS; i++)
                characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
        } else {
            characterCards[CharSelectionStuff[0]].targetAnimationPos = {
                CENTERED_X(CHARACTER_CARD_WIDTH),
                characterCards[CharSelectionStuff[0]].defaultRow.y,
                CHARACTER_CARD_WIDTH,
                CHARACTER_CARD_HEIGHT
            };

            for (int i = 0, dockIndex = 0; i < MAX_CHAR_CARDS; ++i) {
                if (i != CharSelectionStuff[0]) {
                    characterCards[i].targetAnimationPos = {
                        CHARACTER_DOCK_X,
                        CHARACTER_DOCK_Y_START + CHARACTER_DOCK_SPACING * dockIndex++,
                        CHARACTER_CARD_WIDTH,
                        CHARACTER_CARD_HEIGHT
                    };
                }
            }
        }

        for (int i = 0; i < MAX_CHAR_CARDS; i++) {
            characterCards[i].currentAnimationPos.x = animation::slopeInt(
                characterCards[i].currentAnimationPos.x,
                characterCards[i].targetAnimationPos.x,
                animation::easeInOutCubic(1.0f - expf(-15.0f * dt)));
            characterCards[i].currentAnimationPos.y = animation::slopeInt(
                characterCards[i].currentAnimationPos.y,
                characterCards[i].targetAnimationPos.y,
                animation::easeInOutCubic(1.0f - expf(-15.0f * dt)));
        }

        ScreenRects[R_PLAY_BTN] = {
            CENTERED_X(PLAY_BTN_WIDTH),
            characterCards[CharSelectionStuff[0] == -1 ? 0 : CharSelectionStuff[0]].currentAnimationPos.y +
                CHARACTER_CARD_HEIGHT + PLAY_BTN_OFFSET_Y,
            PLAY_BTN_WIDTH,
            PLAY_BTN_HEIGHT
        };

        SetMouseOffset(0, 0);
        SetMouseScale(1.0f, 1.0f);
        break;
    }

    case ScreenState::INTRO_CRAWL:
        if (!scrollIntroCrawl) break;
        introCrawlYPos -= INTRO_CRAWL_SPEED * dt;
        if (introCrawlYPos <= INTRO_CRAWL_END_Y || IsKeyPressed(KEY_ENTER))
            changeScreen(ScreenState::GAMEPLAY);
        break;

    case ScreenState::GAMEPLAY:
        gameManager->update(dt);
        break;
    }
}

void ScreenManager::render() {
    scale = std::min((float)GetScreenWidth() / GAME_SCREEN_WIDTH, (float)GetScreenHeight() / GAME_SCREEN_HEIGHT);
    offset = {((float)GetScreenWidth() - ((float)GAME_SCREEN_WIDTH * scale)) * 0.5f,
              ((float)GetScreenHeight() - ((float)GAME_SCREEN_HEIGHT * scale)) * 0.5f};

    SetMouseOffset(-offset.x, -offset.y);
    SetMouseScale(1.0f / scale, 1.0f / scale);

    BeginTextureMode(target);
    ClearBackground(BLACK);

    switch (currentScreen) {
    case ScreenState::MAIN_MENU:
    {
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);
        DrawTexture(ScreenTextures[1], CENTERED_X(ScreenTextures[1].width), -150, WHITE);

        if (GuiButton(ScreenRects[0], !loadedFromSave ? "START" : "RESTART"))
        {
            changeScreen(ScreenState::CHARACTER_SELECT);
            loadedFromSave = false;
            activeEncounterID = -1;
            currentSceneIndex = TEX_ENTRANCE;
            savedPlayerSceneIndex = TEX_ENTRANCE;
            battleWon.clear();
            collectedItems.clear();
            if (entities)
            {
                for (int i = 0; i < 2; ++i)
                {
                    if (entities[i])
                    {
                       delete entities[i];
                        entities[i] = nullptr;
                    }
                    
                }
            }
        }
        if (GuiButton(ScreenRects[2], "EXIT")) 
        {
            exitScreen(currentScreen);
            loadedFromSave = false;
            CloseWindow();
        }
        
        int prevStateMM = GuiGetState();
        if (!loadedFromSave) GuiDisable();
        if(GuiButton(ScreenRects[1], "RELOAD SAVED GAME"))
        {
            changeScreen(ScreenState::GAMEPLAY);
        }
        GuiSetState(prevStateMM);
        break;
    }
    case ScreenState::CHARACTER_SELECT: {
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);
        CharSelectionStuff[1] = -1;

        for (int i = 0; i < MAX_CHAR_CARDS; i++) {
            DrawTexturePro(characterCards[i].texture,
                        {0.0f, 0.0f, (float)characterCards[i].texture.width, (float)characterCards[i].texture.height},
                        characterCards[i].currentAnimationPos, 
                        {0.0f, 0.0f}, 
                        0.0f, 
                        CharSelectionStuff[0] == i ? WHITE : Color{100, 100, 100, 200} 
            );

            DrawRectangleLinesEx(characterCards[i].currentAnimationPos, 4.0f, Color{0, 68, 0, 255});

            if (CheckCollisionPointRec(GetMousePosition(), characterCards[i].currentAnimationPos))
                CharSelectionStuff[1] = i;

            if (i == 0 && GuiButton(characterCards[i].currentAnimationPos, ""))
            {
                CharSelectionStuff[0] = (CharSelectionStuff[0] == i) ? -1 : i;
                PlaySound(gameSounds[SND_SELECT]);
            }

            if (CharSelectionStuff[0] == i) {
                DrawRectangleLinesEx({characterCards[i].currentAnimationPos.x - 6.0f,
                                      characterCards[i].currentAnimationPos.y - 6.0f,
                                      characterCards[i].currentAnimationPos.width + 12.0f,
                                      characterCards[i].currentAnimationPos.height + 12.0f}, 4, YELLOW);
                DrawRectangleLinesEx({characterCards[i].currentAnimationPos.x - 12.0f,
                                      characterCards[i].currentAnimationPos.y - 12.0f,
                                      characterCards[i].currentAnimationPos.width + 24.0f,
                                      characterCards[i].currentAnimationPos.height + 24.0f}, 2, YELLOW);
            }
        }

        if (CharSelectionStuff[1] != -1 && CharSelectionStuff[1] != CharSelectionStuff[0]) {
            ScreenRects[R_INFO_BOX] = {
                (characterCards[CharSelectionStuff[1]].currentAnimationPos.x + CHARACTER_CARD_WIDTH + 260.0f < (float)GAME_SCREEN_WIDTH) ?
                    (characterCards[CharSelectionStuff[1]].currentAnimationPos.x + CHARACTER_CARD_WIDTH + 5.0f) :
                    (characterCards[CharSelectionStuff[1]].currentAnimationPos.x - 265.0f),
                characterCards[CharSelectionStuff[1]].currentAnimationPos.y - 250.0f,
                260.0f, 240.0f
            };

            DrawRectangleRec(ScreenRects[R_INFO_BOX], Color{0, 40, 0, 200});
            DrawRectangleLinesEx(ScreenRects[R_INFO_BOX], 3.0f, Color{40, 255, 80, 255});

            const char* charNames[] = {"Student", "Rat", "Professor", "Attila"};
            DrawText(TextFormat("Caste: %s", charNames[CharSelectionStuff[1]]),
                    (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 20), 24, WHITE);

            if (CharSelectionStuff[1] == 0) {
                DrawText(TextFormat("Health: %d", getStatForCharacterID(allStatLines, "Student", CSVStats::MAX_HEALTH)),
                        (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 50), 20, WHITE);
                DrawText(TextFormat("Armor: %d", getStatForCharacterID(allStatLines, "Student", CSVStats::ARMOR)),
                        (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 80), 20, WHITE);
                DrawText(TextFormat("Dexterity: %d", getStatForCharacterID(allStatLines, "Student", CSVStats::DEX)),
                        (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 110), 20, WHITE);
                DrawText(TextFormat("Constitution: %d", getStatForCharacterID(allStatLines, "Student", CSVStats::CON)),
                        (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 140), 20, WHITE);
                DrawText(TextFormat("Initiative: %d", getStatForCharacterID(allStatLines, "Student", CSVStats::INITIATIVE)),
                        (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 170), 20, WHITE);
            } else {
                DrawText("Not Available", (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 50), 20, WHITE);
            }
        }

        int prevState = GuiGetState();
        if (CharSelectionStuff[0] == -1) GuiDisable();

        if (GuiButton(ScreenRects[R_PLAY_BTN], "Play Game") && CharSelectionStuff[0] != -1) {
            entities = new Character*[2]{nullptr, nullptr};
            CreateCharacter(entities, allStatLines, "Student", "Steve");
            scrollIntroCrawl = new std::stringstream();
            getIntroCrawlText(scrollIntroCrawl, CharSelectionStuff[0]);
            introCrawlYPos = INTRO_CRAWL_START_Y;

            EndTextureMode();
            changeScreen(ScreenState::INTRO_CRAWL);
            return;
        }
        GuiSetState(prevState);
        break;
    }

    case ScreenState::INTRO_CRAWL:
        if (scrollIntroCrawl) {
            scrollIntroCrawl->clear();
            scrollIntroCrawl->seekg(0, std::ios::beg);
            std::string line;

            for (float y = introCrawlYPos; std::getline(*scrollIntroCrawl, line); y += INTRO_CRAWL_LINE_HEIGHT) {
                if (!line.empty())
                    DrawText(line.c_str(), CENTERED_X(MeasureText(line.c_str(), INTRO_CRAWL_FONT_SIZE)),
                            (int)y, INTRO_CRAWL_FONT_SIZE, GOLD);
            }
            DrawText("Press ENTER to skip", 20, (int)(float)GAME_SCREEN_HEIGHT - 40, 20, GRAY);
        }
        break;

    case ScreenState::GAMEPLAY:
        gameManager->update(GetFrameTime());
        gameManager->render();
        if (gameManager->backToMainMenu) {
            gameManager->backToMainMenu = false;
            delete gameManager;
            gameManager = nullptr;
            changeScreen(ScreenState::MAIN_MENU);
        }
        break;
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(target.texture,
                   {0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height},
                   {offset.x, offset.y, (float)GAME_SCREEN_WIDTH * scale, (float)GAME_SCREEN_HEIGHT * scale},
                   {0.0f, 0.0f}, 0.0f, WHITE);
    SetMouseOffset(0, 0);
    SetMouseScale(1.0f, 1.0f);
    EndDrawing();
}

void ScreenManager::enterScreen(ScreenState s) {
    switch (s) {
    case ScreenState::MAIN_MENU: {
        startMenuStyles();
        numScreenTextures = 2;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png");
        ScreenTextures[1] = LoadTexture("../assets/images/UI/gameTitle.png");

        numScreenRects = 3;
        ScreenRects = new Rectangle[numScreenRects];
        ScreenRects[0] = {CENTERED_X(MAIN_BUTTON_WIDTH), SCREEN_CENTER_Y + MAIN_BUTTON_OFFSET_Y, MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT};
        ScreenRects[1] = {CENTERED_X(MAIN_BUTTON_WIDTH), SCREEN_CENTER_Y + MAIN_BUTTON_OFFSET_Y + MAIN_BUTTON_SPACING, MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT};
        ScreenRects[2] = {CENTERED_X(MAIN_BUTTON_WIDTH), SCREEN_CENTER_Y + MAIN_BUTTON_OFFSET_Y + 2 * MAIN_BUTTON_SPACING, MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT};
        allStatLines = storeAllStatLines(openStartingStatsCSV());
        loadedFromSave = LoadProgress(entities, allStatLines, currentSceneIndex, activeEncounterID, savedPlayerSceneIndex, battleWon, collectedItems);
        break;
    }

    case ScreenState::CHARACTER_SELECT: {
        playerSelectStyles();
        characterCards = new charCard[MAX_CHAR_CARDS];

        CharSelectionStuff = new int[3]{-1, -1, 0};

        numScreenTextures = 5;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png");
        ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/south.png");
        ScreenTextures[2] = LoadTexture("../assets/images/characters/pc/Rat-Assassin/rotations/south.png");
        ScreenTextures[3] = LoadTexture("../assets/images/characters/pc/Professor-Mage/rotations/south.png");
        ScreenTextures[4] = LoadTexture("../assets/images/characters/pc/Attila-Brawler/rotations/south.png");

        numScreenRects = 5;
        ScreenRects = new Rectangle[numScreenRects]{};
        break;
    }

    case ScreenState::INTRO_CRAWL:
        break;

    case ScreenState::GAMEPLAY: {
        gamePlayStyles();
        if(entities && entities[0])
        { 
            InitGameScenes(entities[0]);
            if (loadedFromSave) 
            {
                TraceLog(LOG_INFO, "Loading saved game state.");
            }
            gameManager = new GameManager;
            if (activeEncounterID != -1) {
                gameManager->changeGameState(GameState::COMBAT);
            } else {
                gameManager->enterGameState(gameManager->getCurrentGameState());
            }
        }
        break;
    }
    }
}

void ScreenManager::exitScreen(ScreenState s) {
    switch (s) {
    case ScreenState::MAIN_MENU:
    case ScreenState::CHARACTER_SELECT:
    case ScreenState::INTRO_CRAWL: {
        if (scrollIntroCrawl && s == ScreenState::INTRO_CRAWL) {
            CleanupIntroCrawl();
        }
    }
    case ScreenState::GAMEPLAY: {
        if (s == ScreenState::GAMEPLAY && gameManager) {
            gameManager->exitGameState(gameManager->getCurrentGameState());
            delete gameManager;
            gameManager = nullptr;
        }

        CleanupScreenTextures();
        CleanupScreenRects();
        CleanupCharacterCards();
        CleanupCharSelectionStuff();
        //CleanupStatLines();
        break;
    }
    }
}

//=================== GAMEMANAGER CLASS ===================

GameManager::GameManager(GameState initial) : currentGameState(initial), nextGameState(initial), prevGameState(initial) 
{
    ChangeDirectory(GetApplicationDirectory());
}

GameManager::~GameManager() 
{
    exitGameState(currentGameState);
}

void GameManager::changeGameState(GameState newState) 
{
    if (newState == currentGameState) return;
    nextGameState = newState;
    exitGameState(currentGameState);
    prevGameState = currentGameState;
    currentGameState = newState;
    enterGameState(currentGameState);
}

GameState GameManager::getCurrentGameState() const {
    return currentGameState;
}

void GameManager::enterGameState(GameState state) {
    gamePlayStyles();
    if (prevGameState == GameState::PAUSE_MENU && (state == GameState::COMBAT || state == GameState::EXPLORATION))
        return;

    switch (state) {
    case GameState::EXPLORATION: {
        // Clean up any existing resources first
        CleanupScreenTextures();
        CleanupScreenRects();
        
        if (entities && entities[0]) {
            InitGameScenes(entities[0]);
        }

        // Pause button in top-right corner
        numScreenRects = 25;
        ScreenRects = new Rectangle[numScreenRects];
        ScreenRects[R_EXP_PAUSE_BTN] = {(float)GAME_SCREEN_WIDTH - 75 - 10, 50, 75, 75};
        ScreenRects[R_EXP_PAUSE_BG_OVERLAY] = {0, 0, (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT};
        ScreenRects[R_EXP_PAUSE_PANEL] = {PAUSE_PANEL_X, PAUSE_PANEL_Y, PAUSE_PANEL_WIDTH, PAUSE_PANEL_HEIGHT};
        ScreenRects[R_EXP_BTN_RESUME] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_EXP_BTN_SAVE_EXIT] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_EXP_BTN_QUIT_NO_SAVE] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + 2 * (PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING), PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
    
        sceneTransitionTimer = 0.5f; // Set the scene transition timer to 1 second
        break;
    }

    case GameState::COMBAT: {
        // Clean up any existing resources first
        CleanupScreenTextures();
        CleanupScreenRects();
        
        // Ensure stat lines are available for enemy creation
        if (!allStatLines) {
            TraceLog(LOG_INFO, "Reloading stat lines for combat");
            allStatLines = storeAllStatLines(openStartingStatsCSV());
        }

        // Ensure entity array exists
        if (!entities) {
            TraceLog(LOG_INFO, "Allocating entities array for combat");
            entities = new Character*[2]{nullptr, nullptr};
        }

        numScreenTextures = 3;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture(gameScenes[currentSceneIndex].environmentTexture.c_str());
        ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/north-west.png");
        switch (activeEncounterID) 
        {
            case 0:
                ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/Professor1.png");
                break;
            case 1:
                ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/Sorority1.png");
                break;
            case 2:
            default:
                ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/FratBro1.png");
                break;
        }
        TraceLog(LOG_INFO, "Combat screen textures loaded.");

        numScreenRects = 25;
        ScreenRects = new Rectangle[numScreenRects];
        ScreenRects[R_PLAYER_NAME] = {0, 0, 450, 50};
        ScreenRects[R_ENEMY_NAME] = {(float)GAME_SCREEN_WIDTH - 450, 0, 450, 50};
        ScreenRects[R_PLAYER_PANEL] = {0, 50, 450, 832};
        ScreenRects[R_ENEMY_PANEL] = {(float)GAME_SCREEN_WIDTH - 450, 50, 450, 832};
        ScreenRects[R_PLAYER_HP_BG] = {20, 150, 410, 30};
        ScreenRects[R_PLAYER_HP_FG] = {20, 150, 410, 30};
        ScreenRects[R_ENEMY_HP_BG] = {(float)GAME_SCREEN_WIDTH - 430, 150, 410, 30};
        ScreenRects[R_ENEMY_HP_FG] = {(float)GAME_SCREEN_WIDTH - 430, 150, 410, 30};
        ScreenRects[R_PLAYER_STATUS] = {20, 250, 410, 500};
        ScreenRects[R_ENEMY_STATUS] = {(float)GAME_SCREEN_WIDTH - 430, 250, 410, 500};
        ScreenRects[R_BOTTOM_PANEL] = {0, (float)GAME_SCREEN_HEIGHT - 200, (float)GAME_SCREEN_WIDTH, 215};
        ScreenRects[R_BTN_ATTACK] = {20, (float)GAME_SCREEN_HEIGHT - 180, 400, 80};
        ScreenRects[R_BTN_DEFEND] = {20, (float)GAME_SCREEN_HEIGHT - 80, 400, 80};
        ScreenRects[R_BTN_USE_ITEM] = {570, (float)GAME_SCREEN_HEIGHT - 180, 400, 80};
        ScreenRects[R_LOG_BOX] = {(float)GAME_SCREEN_WIDTH - 800, (float)GAME_SCREEN_HEIGHT - 180, 780, 175};
        ScreenRects[R_ATTACK_MENU] = {0};
        ScreenRects[R_MELEE_BTN] = {0};
        ScreenRects[R_RANGED_BTN] = {0};
        ScreenRects[R_ITEM_MENU] = {0};
        ScreenRects[R_PAUSE_BTN] = {(float)GAME_SCREEN_WIDTH - 450 - 75 - 10, 50 - 37.5f, 75, 75};
        ScreenRects[R_PAUSE_BG_OVERLAY] = {0, 0, (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT};
        ScreenRects[R_PAUSE_PANEL] = {PAUSE_PANEL_X, PAUSE_PANEL_Y, PAUSE_PANEL_WIDTH, PAUSE_PANEL_HEIGHT};
        ScreenRects[R_BTN_RESUME] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_BTN_SAVE_EXIT] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_BTN_QUIT_NO_SAVE] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + 2 * (PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING), PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        TraceLog(LOG_INFO, "Combat screen rectangles initialized.");

        
        TraceLog(LOG_INFO, "Active encounter ID: %d", activeEncounterID);
        if (!entities[0]) {
            TraceLog(LOG_ERROR, "Player entity is null in COMBAT");
        } else {
            TraceLog(LOG_INFO," Player: %s", entities[0]->getName().c_str());
        }
        if (activeEncounterID == 0 && !loadedFromSave) 
        {
            if (entities && entities[1]) { delete entities[1]; entities[1] = nullptr; }
            if (allStatLines) {
                CreateCharacter(entities, allStatLines, "Zombie_Prof", "Professor");
                TraceLog(LOG_INFO, "Created enemy: Professor");
            } else {
                TraceLog(LOG_ERROR, "Cannot create enemy: allStatLines is null");
            }
        } else if (activeEncounterID == 1 && !loadedFromSave) 
        {
            if (entities && entities[1]) { delete entities[1]; entities[1] = nullptr; }
            if (allStatLines) {
                CreateCharacter(entities, allStatLines, "Zombie_Standard", "Sorority");
                TraceLog(LOG_INFO, "Created enemy: Sorority");
            } else {
                TraceLog(LOG_ERROR, "Cannot create enemy: allStatLines is null");
            }
        } 
        else if (!loadedFromSave)
        {
            if (entities && entities[1]) { delete entities[1]; entities[1] = nullptr; }
            if (allStatLines) {
                CreateCharacter(entities, allStatLines, "Zombie_Standard", "Frat Bro");
                TraceLog(LOG_INFO, "Created enemy: Frat Bro");
            } else {
                TraceLog(LOG_ERROR, "Cannot create enemy: allStatLines is null");
            }
        } else 
        {
            TraceLog(LOG_INFO, "Loaded enemy from save: %s", entities[1] ? entities[1]->getName().c_str() : "NULL");
            loadedFromSave = false;
        }
        
        // Validate enemy created
        if (!entities || !entities[1]) {
            TraceLog(LOG_ERROR, "Enemy creation failed; aborting COMBAT setup");
            // Fall back to exploration to avoid crash
            nextGameState = GameState::EXPLORATION;
            enterGameState(currentGameState);
            break;
        }

        combatHandler = new CombatHandler;
        // Guard against null player and enemy
        if (!entities[0]) {
            combatHandler->playerTurn = true;
            TraceLog(LOG_WARNING, "Player entity null; defaulting playerTurn=true");
        } else if (!entities[1]) {
            combatHandler->playerTurn = true;
            TraceLog(LOG_WARNING, "Enemy entity null at initiative check; defaulting playerTurn=true");
        } else {
            if (!combatHandler) {
                TraceLog(LOG_ERROR, "CombatHandler is null!");
            }
            combatHandler->playerTurn = entities[0]->cbt.initiative >= entities[1]->cbt.initiative;
        }
        combatHandler->playerIsDefending = false;
        combatHandler->enemyIsDefending = false;
        combatHandler->playerHitFlashTimer = 0.0f;
        combatHandler->enemyHitFlashTimer = 0.0f;
        combatHandler->gameOverState = false;
        combatHandler->victoryState = false;
        combatHandler->gameOverTimer = 0.0f;
        combatHandler->showAttackMenu = false;
        combatHandler->showItemMenu = false;
        combatHandler->log.clear();
        combatHandler->logScrollOffset = 0.0f;
        AddNewLogEntry(combatHandler->log, "A wild " + entities[1]->getName() + " appears!");
        combatHandler->enemyActionDelay = 1.0f;
        break;
    }

    case GameState::PAUSE_MENU:
        break;
    }
}

void GameManager::exitGameState(GameState state) {
    switch (state) {
    case GameState::EXPLORATION:
        // Clean up exploration textures when leaving (unless going to pause)
        if (nextGameState != GameState::PAUSE_MENU) {
            CleanupScreenTextures();
            CleanupScreenRects();
        }
        break;

    case GameState::COMBAT:
        if (nextGameState != GameState::PAUSE_MENU) {
            if (combatHandler) {
                delete combatHandler;
                combatHandler = nullptr;
            }
            if (entities[1]) {
                delete entities[1];
                entities[1] = nullptr;
            }
            CleanupScreenTextures();
            CleanupScreenRects();
        }
        break;

    case GameState::PAUSE_MENU:
        break;
    }
}

void GameManager::render() {
    switch (currentGameState) {
    case GameState::EXPLORATION: {
        if (gameScenes.empty() || !ScreenTextures) break;

        DrawTexturePro(ScreenTextures[gameScenes[currentSceneIndex].textureIndex],
                      {0.0f, 0.0f, (float)ScreenTextures[gameScenes[currentSceneIndex].textureIndex].width,
                       (float)ScreenTextures[gameScenes[currentSceneIndex].textureIndex].height},
                      {0.0f, 0.0f, (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT}, {0.0f, 0.0f}, 0.0f, WHITE);

        DrawRectangleRec(ScreenRects[R_EXP_PAUSE_BTN], COL_BUTTON);
        DrawRectangleLinesEx(ScreenRects[R_EXP_PAUSE_BTN], 3.0f, BLACK);
        if (GuiButton(ScreenRects[R_EXP_PAUSE_BTN], "")) {
            changeGameState(GameState::PAUSE_MENU);
        }
        DrawTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize),
                   {ScreenRects[R_EXP_PAUSE_BTN].x + (ScreenRects[R_EXP_PAUSE_BTN].width - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).x) / 2.0f,
                    ScreenRects[R_EXP_PAUSE_BTN].y + (ScreenRects[R_EXP_PAUSE_BTN].height - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).y) / 2.0f},
                   FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

        for (const auto &item : gameScenes[currentSceneIndex].sceneItems) {
            if (!isItemCollected(item.itemName) &&
                (!item.requiresVictory || (gameScenes[currentSceneIndex].hasEncounter && battleWon[gameScenes[currentSceneIndex].encounterID]))) {
                DrawTexturePro(ScreenTextures[item.textureIndex],
                              {0, 0, (float)ScreenTextures[item.textureIndex].width, (float)ScreenTextures[item.textureIndex].height},
                              item.clickArea, {0, 0}, 0.0f, WHITE);
            }
        }

        for (const auto &arrow : gameScenes[currentSceneIndex].sceneArrows) {
            if (!arrow.isEnabled || (!arrow.requiredKeyName.empty() && !isItemCollected(arrow.requiredKeyName)))
                continue;
            float scaledWidth = arrow.clickArea.width+ arrow.clickArea.width * animation::sinPulse(0.2f, PI, animation::easeInOutCubic(fmodf(GetTime(), 1.0f)));
            float scaledHeight = arrow.clickArea.height + arrow.clickArea.height * animation::sinPulse(0.2f, PI, animation::easeInOutCubic(fmodf(GetTime(), 1.0f)));
            DrawTexturePro(ScreenTextures[TEX_ARROW],
                          {0.0f, 0.0f, (float)ScreenTextures[TEX_ARROW].width, (float)ScreenTextures[TEX_ARROW].height},
                          {arrow.clickArea.x + arrow.clickArea.width / 2.0f, arrow.clickArea.y + arrow.clickArea.height / 2.0f,
                           scaledWidth, scaledHeight},
                          {scaledWidth/2.0f, scaledHeight/2.0f},
                          ARROW_ROTATION(arrow.dir), WHITE);
        }

        DrawRectangleLinesEx({MINIMAP_X, MINIMAP_Y, MINIMAP_SIZE, MINIMAP_SIZE}, MINIMAP_BORDER, BLACK);
        DrawTexturePro(ScreenTextures[TEX_MINIMAP],
                      {0, 0, (float)ScreenTextures[TEX_MINIMAP].width, (float)ScreenTextures[TEX_MINIMAP].height},
                      {MINIMAP_X, MINIMAP_Y, MINIMAP_SIZE, MINIMAP_SIZE}, {0, 0}, 0.0f, WHITE);

        DrawTexturePro(ScreenTextures[TEX_TURTLE],
                      {0, 0, (float)ScreenTextures[TEX_TURTLE].width, (float)ScreenTextures[TEX_TURTLE].height},
                      {MINIMAP_X + gameScenes[currentSceneIndex].minimapCoords.x * MINIMAP_SIZE - 16,
                       MINIMAP_Y + gameScenes[currentSceneIndex].minimapCoords.y * MINIMAP_SIZE - 16, 32, 32},
                      {16, 16}, gameScenes[currentSceneIndex].minimapRotation, WHITE);

        DrawText(gameScenes[currentSceneIndex].sceneName.c_str(), MINIMAP_X, MINIMAP_Y - 30, 30, WHITE);
        DrawRectangleLinesEx({MINIMAP_X, MINIMAP_Y, MINIMAP_SIZE, MINIMAP_SIZE}, MINIMAP_BORDER, BLACK);
        DrawRectangle(0, 0, (float)GAME_SCREEN_WIDTH, 40, BLACK);

        std::string infoText;
        for (const auto &item : gameScenes[currentSceneIndex].sceneItems) {
            if (!isItemCollected(item.itemName) &&
                (!item.requiresVictory || (gameScenes[currentSceneIndex].hasEncounter && battleWon[gameScenes[currentSceneIndex].encounterID])) &&
                CheckCollisionPointRec(GetMousePosition(), item.clickArea)) {
                infoText = item.hoverText;
                break;
            }
        }

        if (infoText.empty()) {
            for (const auto &arrow : gameScenes[currentSceneIndex].sceneArrows) {
                if (arrow.isEnabled && (arrow.requiredKeyName.empty() || isItemCollected(arrow.requiredKeyName)) &&
                    CheckCollisionPointRec(GetMousePosition(), arrow.clickArea)) {
                    infoText = arrow.hoverText;
                    break;
                }
            }
        }

        bool hasVisibleItems = false;
        if (infoText.empty())
            for (const auto &item : gameScenes[currentSceneIndex].sceneItems) 
            {
                if (!isItemCollected(item.itemName) && (!item.requiresVictory || (gameScenes[currentSceneIndex].hasEncounter && battleWon[gameScenes[currentSceneIndex].encounterID]))) 
                {
                    hasVisibleItems = true;
                    break;
                }
            }
        
        bool hasArrowsVisible = false;
        for (const auto &arrow : gameScenes[currentSceneIndex].sceneArrows) {
            if (arrow.isEnabled && (arrow.requiredKeyName.empty() || isItemCollected(arrow.requiredKeyName))) {
                hasArrowsVisible = true;
                break;
            }
        }

        if (infoText.empty()) 
        {
            if (hasVisibleItems) {
                infoText = "Please select the item(s) to add it to your inventory.";
            } else if (hasArrowsVisible) {
                infoText = "Please select an arrow to navigate.";
            } else {
                infoText = "There's nothing here.";
            }

        }

        DrawText(infoText.c_str(), 20,ScreenRects[R_LOG_BOX].y + 5, 30, WHITE);
        break;
    }

    case GameState::COMBAT: {
        if (!combatHandler || !entities[0] || !entities[1] || !ScreenTextures || !ScreenRects ||!nerdFont) break;

        DrawTexture(ScreenTextures[0], gameScenes[currentSceneIndex].combatBgX, gameScenes[currentSceneIndex].combatBgY, WHITE);

        DrawTexturePro(ScreenTextures[1],
                      {0.0f, 0.0f, (float)ScreenTextures[1].width, (float)ScreenTextures[1].height},
                      {gameScenes[currentSceneIndex].playerCharX, gameScenes[currentSceneIndex].playerCharY, gameScenes[currentSceneIndex].playerScale.x, gameScenes[currentSceneIndex].playerScale.y}, {0.0f, 0.0f}, 0.0f,
                      combatHandler->playerHitFlashTimer > 0.0f ? RED : WHITE);
            //texture, sourceRec, destRec, origin, rotation, tint
        DrawTexturePro(ScreenTextures[2],
                     
                      {0.0f, 0.0f, (float)ScreenTextures[2].width, (float)ScreenTextures[2].height},
                      {gameScenes[currentSceneIndex].enemyCharX, gameScenes[currentSceneIndex].enemyCharY, gameScenes[currentSceneIndex].enemyScale.x, gameScenes[currentSceneIndex].enemyScale.y}, {0.0f, 0.0f}, 0.0f,
                      combatHandler->enemyHitFlashTimer > 0.0f ? RED : WHITE);

        DrawRectangleRec(ScreenRects[R_PLAYER_NAME], COL_NAME_BAR);
        DrawRectangleRec(ScreenRects[R_ENEMY_NAME], COL_NAME_BAR);
        DrawRectangleRec(ScreenRects[R_BOTTOM_PANEL], COL_BOTTOM_PANEL);
        DrawRectangleRec(ScreenRects[R_PLAYER_PANEL], COL_STATUS_PANEL);
        DrawRectangleRec(ScreenRects[R_ENEMY_PANEL], COL_STATUS_PANEL);
        DrawRectangleRec(ScreenRects[R_PLAYER_HP_BG], COL_HP_BG);
        DrawRectangleRec(ScreenRects[R_PLAYER_HP_FG], COL_HP_FG);
        DrawRectangleRec(ScreenRects[R_ENEMY_HP_BG], COL_HP_BG);
        DrawRectangleRec(ScreenRects[R_ENEMY_HP_FG], COL_HP_FG);
        DrawRectangleRec(ScreenRects[R_PLAYER_STATUS], COL_STATUS_INNER);
        DrawRectangleRec(ScreenRects[R_ENEMY_STATUS], COL_STATUS_INNER);
        DrawRectangleRec(ScreenRects[R_LOG_BOX], COL_LOG_BOX);
        DrawRectangleRec(ScreenRects[R_BTN_ATTACK], COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_BTN_DEFEND], COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_BTN_USE_ITEM], COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_PAUSE_BTN], COL_BUTTON);

        if (GuiButton(ScreenRects[R_PAUSE_BTN], "")) {
            gameManager->changeGameState(GameState::PAUSE_MENU);
        }
        
        if (ScreenRects[R_PAUSE_BTN].height == 0 || ScreenRects[R_PAUSE_BTN].width == 0)
        {
            TraceLog(LOG_ERROR, "ScreenRects[R_PAUSE_BTN] is null!");
        }
        DrawTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize),
                   {ScreenRects[R_PAUSE_BTN].x + (ScreenRects[R_PAUSE_BTN].width - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).x) / 2.0f,
                    ScreenRects[R_PAUSE_BTN].y + (ScreenRects[R_PAUSE_BTN].height - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).y) / 2.0f},
                   FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

        for (int i = 0; i < 15; ++i)
            if (i != R_PLAYER_HP_FG && i != R_ENEMY_HP_FG)
                DrawRectangleLinesEx(ScreenRects[i], 3.0f, BLACK);

        DrawText(("Player: " + entities[0]->getName()).c_str(),
                (int)(ScreenRects[R_PLAYER_NAME].x + 20), (int)(ScreenRects[R_PLAYER_NAME].y + 10), FONT_SIZE_NAME, WHITE);
        DrawText(("Enemy: " + entities[1]->getName()).c_str(),
                (int)(ScreenRects[R_ENEMY_NAME].x + 20), (int)(ScreenRects[R_ENEMY_NAME].y + 10), FONT_SIZE_NAME, WHITE);
        DrawText(TextFormat("HP: %d / %d", entities[0]->vit.health, entities[0]->vit.maxHealth),
                (int)(ScreenRects[R_PLAYER_PANEL].x + 30), (int)(ScreenRects[R_PLAYER_PANEL].y + 130), FONT_SIZE_HP, WHITE);
        DrawText(TextFormat("HP: %d / %d", entities[1]->vit.health, entities[1]->vit.maxHealth),
                (int)(ScreenRects[R_ENEMY_PANEL].x + 30), (int)(ScreenRects[R_ENEMY_PANEL].y + 130), FONT_SIZE_HP, WHITE);

        if (combatHandler->playerTurn) {
            if (GuiButton(ScreenRects[R_BTN_ATTACK], "ATTACK")) {
                combatHandler->playerIsDefending = false;
                entities[0]->endDefense();
                combatHandler->showAttackMenu = !combatHandler->showAttackMenu;
                combatHandler->showItemMenu = false;
                AddNewLogEntry(combatHandler->log, combatHandler->showAttackMenu ? "Choose your attack." : "Attack cancelled.");
                combatHandler->logScrollOffset = 1000.0f;
            }

            if (combatHandler->showAttackMenu) {
                ScreenRects[R_ATTACK_MENU] = {ScreenRects[R_BTN_ATTACK].x + ScreenRects[R_BTN_ATTACK].width + 10,
                                              ScreenRects[R_BTN_ATTACK].y - 150, 300.0f, 140.0f};
                ScreenRects[R_MELEE_BTN] = {ScreenRects[R_ATTACK_MENU].x + 10, ScreenRects[R_ATTACK_MENU].y + 10,
                                           ScreenRects[R_ATTACK_MENU].width - 20, 50.0f};
                ScreenRects[R_RANGED_BTN] = {ScreenRects[R_ATTACK_MENU].x + 10, ScreenRects[R_ATTACK_MENU].y + 75,
                                            ScreenRects[R_ATTACK_MENU].width - 20, 50.0f};

                DrawRectangleRec(ScreenRects[R_ATTACK_MENU], COL_BOTTOM_PANEL);
                DrawRectangleLinesEx(ScreenRects[R_ATTACK_MENU], 3.0f, BLACK);

                if (GuiButton(ScreenRects[R_MELEE_BTN], "")) {
                    combatHandler->showAttackMenu = false;
                    combatHandler->playerIsDefending = false;
                    combatHandler->enemyHitFlashTimer = resolve_melee(*entities[0], *entities[1], combatHandler->enemyIsDefending, combatHandler->log) ? 0.2f : 0.0f;
                    if (combatHandler->enemyHitFlashTimer > 0.0f) PlaySound(gameSounds[SND_HIT]);
                    combatHandler->logScrollOffset = 1000.0f;
                    combatHandler->playerTurn = false;
                    combatHandler->enemyActionDelay = 0.6f;
                }

                DrawText("MELEE", (int)(ScreenRects[R_MELEE_BTN].x + 20), (int)(ScreenRects[R_MELEE_BTN].y + 10),
                        FONT_SIZE_BTN, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));
                DrawTextEx(*nerdFont, CodepointToUTF8(ICON_SWORD, &byteSize),
                          {ScreenRects[R_MELEE_BTN].x + ScreenRects[R_MELEE_BTN].width - 50, ScreenRects[R_MELEE_BTN].y + 2},
                          FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

                if (GuiButton(ScreenRects[R_RANGED_BTN], "")) {
                    combatHandler->showAttackMenu = false;
                    combatHandler->playerIsDefending = false;
                    combatHandler->enemyHitFlashTimer = resolve_ranged(*entities[0], *entities[1], combatHandler->enemyIsDefending, combatHandler->log) ? 0.2f : 0.0f;
                    combatHandler->logScrollOffset = 1000.0f;
                    combatHandler->playerTurn = false;
                    combatHandler->enemyActionDelay = 0.6f;
                }

                DrawText("RANGED", (int)(ScreenRects[R_RANGED_BTN].x + 20), (int)(ScreenRects[R_RANGED_BTN].y + 10),
                        FONT_SIZE_BTN, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));
                DrawTextEx(*nerdFont, CodepointToUTF8(ICON_BOW_ARROW, &byteSize),
                          {ScreenRects[R_RANGED_BTN].x + ScreenRects[R_RANGED_BTN].width - 50, ScreenRects[R_RANGED_BTN].y + 2},
                          FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

                if (!entities[1]->isAlive()) {
                    AddNewLogEntry(combatHandler->log, "You have defeated " + entities[1]->getName() + "!");
                    combatHandler->gameOverTimer = 2.0f;
                    combatHandler->victoryState = true;
                    return;
                }
            } else {
                ScreenRects[R_ATTACK_MENU] = {0};
                ScreenRects[R_MELEE_BTN] = {0};
                ScreenRects[R_RANGED_BTN] = {0};
            }

            if (GuiButton(ScreenRects[R_BTN_DEFEND], "DEFEND")) {
                combatHandler->showAttackMenu = false;
                combatHandler->playerIsDefending = true;
                entities[0]->startDefense();
                combatHandler->showItemMenu = false;
                AddNewLogEntry(combatHandler->log, entities[0]->getName() + " is defending!");
                combatHandler->logScrollOffset = 1000.0f;
                combatHandler->playerTurn = false;
                combatHandler->enemyActionDelay = 0.6f;
            }

            if (GuiButton(ScreenRects[R_BTN_USE_ITEM], "USE ITEM")) {
                combatHandler->playerIsDefending = false;
                entities[0]->endDefense();
                combatHandler->showItemMenu = !combatHandler->showItemMenu;
                combatHandler->showAttackMenu = false;
                if (dynamic_cast<PlayerCharacter*>(entities[0])->inv.getItems().empty()) {
                    AddNewLogEntry(combatHandler->log, "No items in inventory.");
                    combatHandler->showItemMenu = false;
                } else if (combatHandler->showItemMenu) {
                    AddNewLogEntry(combatHandler->log, "Choose an item to use.");
                }
                combatHandler->logScrollOffset = 1000.0f;
            }

            if (combatHandler->showItemMenu) {
                auto &items = dynamic_cast<PlayerCharacter*>(entities[0])->inv.getItems();
                ScreenRects[R_ITEM_MENU] = {ScreenRects[R_BTN_USE_ITEM].x + ScreenRects[R_BTN_USE_ITEM].width + 10,
                                           ScreenRects[R_BTN_USE_ITEM].y - (55.0f * items.size()) - 20.0f,
                                           ScreenRects[R_BTN_USE_ITEM].width, (55.0f * items.size()) + 20.0f};

                DrawRectangleRec(ScreenRects[R_ITEM_MENU], COL_BOTTOM_PANEL);
                DrawRectangleLinesEx(ScreenRects[R_ITEM_MENU], 3.0f, BLACK);

                for (size_t i = 0; i < items.size(); i++) {
                    if (GuiButton({ScreenRects[R_ITEM_MENU].x + 10.0f, ScreenRects[R_ITEM_MENU].y + 10.0f + (i * 55.0f),
                                  ScreenRects[R_ITEM_MENU].width - 20.0f, 50.0f}, "")) {
                        if (items[i].healAmount > 0) {
                            if (entities[0]->vit.health == entities[0]->vit.maxHealth) {
                                AddNewLogEntry(combatHandler->log, entities[0]->getName() + "'s health is already full!");
                                combatHandler->logScrollOffset = 1000.0f;
                                combatHandler->showItemMenu = false;
                                continue;
                            }
                            int beforeHeal = entities[0]->vit.health;
                            dynamic_cast<PlayerCharacter*>(entities[0])->heal(items[i].healAmount);
                            AddNewLogEntry(combatHandler->log, entities[0]->getName() + " used " + items[i].name + " and healed " +
                                          std::to_string(entities[0]->vit.health - beforeHeal) + " HP!");
                            PlaySound(gameSounds[SND_HEAL]);
                            dynamic_cast<PlayerCharacter*>(entities[0])->inv.removeitem(items[i].name, 1);
                            combatHandler->logScrollOffset = 1000.0f;
                            combatHandler->playerTurn = false;
                            combatHandler->enemyActionDelay = 0.6f;
                            combatHandler->showItemMenu = false;
                        }
                    }

                    std::string itemLabel = items[i].name;
                    for (char &c : itemLabel) c = toupper(c);
                    itemLabel += " (x" + std::to_string(items[i].quantity) + ")";
                    DrawText(itemLabel.c_str(), (int)(ScreenRects[R_ITEM_MENU].x + 20 + (i * 0)),
                            (int)(ScreenRects[R_ITEM_MENU].y + 20 + (i * 55.0f)), FONT_SIZE_BTN,
                            GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));
                }
            }
        } else {
            int prevState = GuiGetState();
            GuiDisable();
            GuiButton(ScreenRects[R_BTN_ATTACK], "ATTACK");
            GuiButton(ScreenRects[R_BTN_DEFEND], "DEFEND");
            GuiButton(ScreenRects[R_BTN_USE_ITEM], "USE ITEM");
            GuiSetState(prevState);
        }

        BeginScissorMode((int)ScreenRects[R_LOG_BOX].x + 1, (int)ScreenRects[R_LOG_BOX].y + 1, (int)ScreenRects[R_LOG_BOX].width - 2, (int)ScreenRects[R_LOG_BOX].height - 2);
        float logY = ScreenRects[R_LOG_BOX].y + 5.0f - combatHandler->logScrollOffset;

        for (size_t i = 0; i < combatHandler->log.size(); ++i)
        {
            if (i != combatHandler->log.size() - 1)
            {
                // For all but the last log entry, use gray color
                DrawText(TextFormat(". %s", combatHandler->log[i].c_str()), (int)(ScreenRects[R_LOG_BOX].x + 10), (int)logY, FONT_SIZE_LOG, GRAY);
            }
            else
            {
                DrawText(TextFormat("> %s", combatHandler->log[i].c_str()), (int)(ScreenRects[R_LOG_BOX].x + 10), (int)logY, FONT_SIZE_LOG, BLACK);
            }
            logY += LOG_LINE_HEIGHT;
        }

        EndScissorMode();
        DrawStatusPanel(ScreenRects[R_PLAYER_STATUS], entities[0]->statEff, *nerdFont);
        DrawStatusPanel(ScreenRects[R_ENEMY_STATUS], entities[1]->statEff,*nerdFont);
        break;
    }

    case GameState::PAUSE_MENU: {
        GameState tempState = currentGameState;
        currentGameState = prevGameState;
        GuiDisable();
        render();
        GuiEnable();
        currentGameState = tempState;

        DrawRectangleRec(ScreenRects[R_PAUSE_BG_OVERLAY], Fade(BLACK, 0.7f));
        DrawRectangleRec(ScreenRects[R_PAUSE_PANEL], COL_BOTTOM_PANEL);
        DrawRectangleLinesEx(ScreenRects[R_PAUSE_PANEL], 3.0f, BLACK);
        DrawText("PAUSED", CENTERED_X(MeasureText("PAUSED", 40)), (int)(ScreenRects[R_PAUSE_PANEL].y + 10), 40, WHITE);

        DrawRectangleRec(ScreenRects[R_BTN_RESUME], COL_BUTTON);
        if (GuiButton(ScreenRects[R_BTN_RESUME], "Resume"))
            currentGameState = prevGameState;

        DrawRectangleRec(ScreenRects[R_BTN_SAVE_EXIT], COL_BUTTON);
        if (GuiButton(ScreenRects[R_BTN_SAVE_EXIT], "Save & Exit")) {
            savedSucessfully = saveProgress(entities, currentSceneIndex, activeEncounterID, savedPlayerSceneIndex, battleWon, collectedItems);
            backToMainMenu = true;
        }

        DrawRectangleRec(ScreenRects[R_BTN_QUIT_NO_SAVE], COL_BUTTON);
        if (GuiButton(ScreenRects[R_BTN_QUIT_NO_SAVE], "Exit (No Save)")) {
            backToMainMenu = true;
        }
        break;
    }
    }
}

void GameManager::update(float dt) {
    float scale = std::min((float)GetScreenWidth() / GAME_SCREEN_WIDTH, (float)GetScreenHeight() / GAME_SCREEN_HEIGHT);
    Vector2 virtualMouse = {
        ((GetMousePosition().x - (((float)GetScreenWidth() - ((float)GAME_SCREEN_WIDTH * scale)) * 0.5f)) / scale),
        ((GetMousePosition().y - (((float)GetScreenHeight() - ((float)GAME_SCREEN_HEIGHT * scale)) * 0.5f)) / scale)
    };

    switch (currentGameState) {
    case GameState::EXPLORATION: {
        if (gameScenes.empty()) break;

        if (sceneTransitionTimer > 0.0f) {
            sceneTransitionTimer -= dt;
            if (sceneTransitionTimer < 0.0f) sceneTransitionTimer = 0.0f;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (auto &item : gameScenes[currentSceneIndex].sceneItems) {
                if (!isItemCollected(item.itemName) &&
                    (!item.requiresVictory || (gameScenes[currentSceneIndex].hasEncounter && battleWon[gameScenes[currentSceneIndex].encounterID])) &&
                    CheckCollisionPointRec(virtualMouse, item.clickArea)) {
                    collectedItems.push_back(item.itemName);

                    if (item.itemName == "Health Potion") 
                    {
                            HealthPotion hpotion;
                            dynamic_cast<PlayerCharacter*>(entities[0])->inv.additem(hpotion);
                    }

                    if (item.itemName == "Baseball Bat") 
                    {
                        if (dynamic_cast<Student*>(entities[0]))
                        {
                            dynamic_cast<Student*>(entities[0])->wep.meleeWeapon += 2;
                            dynamic_cast<Student*>(entities[0])->wep.rangeWeapon += 1;
                        } else if (dynamic_cast<Rat*>(entities[0]))
                        {
                            dynamic_cast<Rat*>(entities[0])->wep.meleeWeapon += 2;
                            dynamic_cast<Rat*>(entities[0])->wep.rangeWeapon += 1;
                        } else if (dynamic_cast<Professor*>(entities[0]))
                        {
                            dynamic_cast<Professor*>(entities[0])->wep.meleeWeapon += 2;
                            dynamic_cast<Professor*>(entities[0])->wep.rangeWeapon += 1;
                        } else if (dynamic_cast<Atilla*>(entities[0]))
                        {
                            dynamic_cast<Atilla*>(entities[0])->wep.meleeWeapon += 2;
                            dynamic_cast<Atilla*>(entities[0])->wep.rangeWeapon += 1;
                        }
                    }

                    if (item.itemName == "Key 1")
                    {
                        dynamic_cast<PlayerCharacter*>(entities[0])->key1 = true;
                    }

                    if (item.itemName == "Key 2")
                    {
                        dynamic_cast<PlayerCharacter*>(entities[0])->key2 = true;
                    }
                }
            }

            for (const auto &arrow : gameScenes[currentSceneIndex].sceneArrows) {
                if (!arrow.isEnabled || (!arrow.requiredKeyName.empty() && !isItemCollected(arrow.requiredKeyName)))
                    continue;

                if (CheckCollisionPointRec(virtualMouse, arrow.clickArea)) {
                    currentSceneIndex = arrow.targetSceneIndex;
                    sceneTransitionTimer = 0.25f;

                    if (gameScenes[currentSceneIndex].hasEncounter && !battleWon[gameScenes[currentSceneIndex].encounterID]) {
                        savedPlayerSceneIndex = currentSceneIndex;
                        activeEncounterID = gameScenes[currentSceneIndex].encounterID;
                        changeGameState(GameState::COMBAT);
                    }
                    break;
                }
            }
        }
        break;
    }

    case GameState::COMBAT: {
        if (!combatHandler || !entities[0] || !entities[1]) break;

        ScreenRects[R_PLAYER_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_PLAYER_HP_BG], entities[0]->vit.health, entities[0]->vit.maxHealth);
        ScreenRects[R_ENEMY_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_ENEMY_HP_BG], entities[1]->vit.health, entities[1]->vit.maxHealth);
        combatHandler->playerHitFlashTimer = std::max(0.0f, combatHandler->playerHitFlashTimer - dt);
        combatHandler->enemyHitFlashTimer = std::max(0.0f, combatHandler->enemyHitFlashTimer - dt);

        if (CheckCollisionPointRec(virtualMouse, ScreenRects[R_LOG_BOX])) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f)
                combatHandler->logScrollOffset += wheel * -25.0f;
        }

        if (combatHandler->logScrollOffset < 0.0f) combatHandler->logScrollOffset = 0.0f;
        if (combatHandler->logScrollOffset > std::max(0.0f, (float)(combatHandler->log.size() * LOG_LINE_HEIGHT) - (ScreenRects[R_LOG_BOX].height - 10.0f))) 
            combatHandler->logScrollOffset = std::max(0.0f, (float)(combatHandler->log.size() * LOG_LINE_HEIGHT) - (ScreenRects[R_LOG_BOX].height - 10.0f));

        if (combatHandler->gameOverState || combatHandler->victoryState) {
            combatHandler->gameOverTimer -= dt;
            if (combatHandler->gameOverTimer <= 0.0f) {
                PlaySound(gameSounds[SND_ZOM_DEATH]);
                if (combatHandler->victoryState && activeEncounterID >= 0) {
                    battleWon[activeEncounterID] = true;
                    switch (activeEncounterID) 
                    {
                        case 0:
                        {
                            dynamic_cast<PlayerCharacter*>(entities[0])->zombie1Defeated = true;
                            break;
                        }
                        case 1:
                        {
                            dynamic_cast<PlayerCharacter*>(entities[0])->zombie2Defeated = true;
                            break;
                        }
                        case 2:
                        {
                            dynamic_cast<PlayerCharacter*>(entities[0])->zombie3Defeated = true;
                            break;
                        }
                    }
                    activeEncounterID = -1;
                }
                changeGameState(GameState::EXPLORATION);
            }
            break;
        }

        if (!combatHandler->playerTurn) {
            combatHandler->enemyActionDelay -= dt;
            if (combatHandler->enemyActionDelay <= 0.0f) {
                if (combatHandler->enemyIsDefending)
                    entities[1]->endDefense();
                combatHandler->enemyIsDefending = false;

                Action enemyAction = ai_choose(*(dynamic_cast<NonPlayerCharacter*>(entities[1])), *(dynamic_cast<PlayerCharacter*>(entities[0])));

                if (enemyAction.type == ActionType::Attack) {
                    combatHandler->playerHitFlashTimer = resolve_melee(*entities[1], *entities[0], combatHandler->playerIsDefending, combatHandler->log) ? 0.2f : 0.0f;
                    if (combatHandler->playerHitFlashTimer > 0.0f) PlaySound(gameSounds[SND_HIT]);
                    combatHandler->logScrollOffset = 1000.0f;
                    if (!entities[0]->isAlive()) {
                        AddNewLogEntry(combatHandler->log, "You died.");
                        combatHandler->gameOverTimer = 2.0f;
                        combatHandler->gameOverState = true;
                        return;
                    }
                } else if (enemyAction.type == ActionType::Defend) {
                    combatHandler->enemyIsDefending = true;
                    entities[1]->startDefense();
                    AddNewLogEntry(combatHandler->log, entities[1]->getName() + " is defending!");
                    combatHandler->logScrollOffset = 1000.0f;
                }
                combatHandler->playerTurn = true;
            }
        }
        break;
    }

    case GameState::PAUSE_MENU:
        break;
    }
}

// Stub for saveProgress until json.hpp is available
void saveProgress(Character** entities, int currentSceneIndex, int activeEncounterID, 
                  int savedPlayerSceneIndex, std::map<int,bool>& battleWon, 
                  std::vector<std::string>& collectedItems) {
    TraceLog(LOG_INFO, "Save progress not yet implemented (requires json.hpp)");
}