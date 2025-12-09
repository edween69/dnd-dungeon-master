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
                    - GameManager::GameManager(GameState initial): Constructor to initialize the GameManager with an initial game state.

                    - GameManager::~GameManager(): Destructor to clean up resouces when the
                      GameManager is destroyed.

                    - void GameManager::changeGameState(GameState newState): Request a change 
                      to a new game state (handles the transition logic).

                    - GameState GameManager::getCurrentGameState() const: Get the current game state.

                    - void GameManager::enterGameState(GameState state): Handle entering a new game
                      state by loading resources and setting up the enviroment.

                    - void GameManager::exitGameState(GameState state): Handle exiting a game state
                      by unloading resources and cleaning up entities.

                    - void GameManager::render(): Render the current game state (handles drawing 
                      exploration, combat, or pause menu).

                    - void GameManager::update(float dt): Update the current game state logic 
                      (handles inputs and game logic).

                Global/Helper Functions:
                    - Cleanup Functions: A set of functions (CleanupScreenTextures, CleanupEntities, etc.)
                      to safely delete and set pointers to nullptr to avoid memory leaks.

                    - Init Functions: Functions to intialize game scenes (InitGameScenes) and 
                      load sounds (InitGameSounds).

                    - Style Functions: Functions to set the visual style of the GUI (buttons, colors)
                      depending on the current screen context.

                    - Other Helpers: Functions for loading intro text and drawing status panels.
============================================================================================= */


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

// ok so these are all the global variables that we need to keep track of stuff
// i know globals are bad but we need them here for the way the code is structured
static std::istringstream *allStatLines = nullptr; // Used throughout game - holds all the character stats from CSV
static Sound *gameSounds = nullptr; // Used throughout game - all our sound effects go here
static Texture2D *ScreenTextures = nullptr; // Used throughout game - images for whatver screen were on
static Rectangle *ScreenRects = nullptr; // Used throughout game - clickable areas basically
static charCard *characterCards = nullptr; // Used in Character Select state only - the lil cards u click on
static int *CharSelectionStuff = nullptr; // Used in Character Select state only (holds selected character[0], hovered character[1], and initialized state[2])
static std::stringstream *scrollIntroCrawl = nullptr; // Used in Intro Crawl state only - the star wars text thing
static Character **entities = nullptr; // Used in Combat state only (Player at index 0, Enemy at index 1) - basically whos fighting
static GameManager *gameManager = nullptr; // Used throughout GAMEPLAY state - the big boss that controls everything
static Font *nerdFont = nullptr; // Used throughout game - fancy font with icons and stuff


static int numScreenTextures = 0; // how many textures we got loaded rn
static int numScreenRects = 0; // how many rectangles we got
static float introCrawlYPos = 0.0f; // where the scrolly text is at
static int byteSize=0; // needed for the icon rendering stuff
static Music backgroundMusic = {0};
static bool musicLoaded = false;

//Game scenes and related data (Please review above comment block)
// these are for keeping track of where the player is and what theyve done
static std::vector<GameScene> gameScenes; // all the rooms/locations in the game
static std::map<int,bool> battleWon; // which fights have been won (so zombies dont respawn)
static std::vector<std::string> collectedItems; // stuff the player picked up
static bool loadedFromSave = false, savedSucessfully = false; // save/load flags
static int activeEncounterID = -1; // which fight is happening rn (-1 means no fight)
static int currentSceneIndex = TEX_ENTRANCE; // where the player is standing
static int savedPlayerSceneIndex = TEX_ENTRANCE; // where they were when they saved

//======================= RESOURCE CLEANUP FUNCTIONS =======================
/*
    These are a set of functions that delete/deallocate any and all dynamic resources being used in the game.
    Basically we gotta clean up after ourselves or else memory leaks happen and thats bad.
    Each function handles one type of resource so its easier to manage.
    
    The pattern is always: check if pointer exists, clean it up, delete it, set to nullptr
    Setting to nullptr is important cause otherwise you get dangeling pointers and crashes
    i learned this the hard way
*/

/**
 * @brief Safely cleans up all screen textures. This function checks if ScreenTextures is not null, then iterates through each texture, unloading them before deleting the array and setting the pointer to nullptr. Gotta unload textures or else your GPU will fill up with unused data.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupScreenTextures() 
{
    if (ScreenTextures) { // only do stuff if theres actually something to clean
        for (int i = 0; i < numScreenTextures; ++i) {
            UnloadTexture(ScreenTextures[i]); // tell raylib to remove the texture from memory
        }
        delete[] ScreenTextures; // Delete the array of textures itself
        ScreenTextures = nullptr; // Set pointer to nullptr so we dont accidentally use it again
    }
    numScreenTextures = 0; //Reset the count of screen textures back to zero
}

/**
 * @brief Safely cleans up all screen rectangles. This function checks if ScreenRects is not null, then deletes the array and sets the pointer to nullptr. Rectangles dont need special unloading like textures do cause theyre just numbers basically.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupScreenRects() 
{
    if (ScreenRects) { // same pattern as before
        delete[] ScreenRects; // Delete the array of rectangles
        ScreenRects = nullptr; // set to nullptr
    }
    numScreenRects = 0; // Reset the count so we know its empty
}

/**
 * @brief Safely cleans up character cards. This function checks if characterCards is not null, then deletes the array and sets the pointer to nullptr. These are the lil character selection cards that slide around and look cool.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupCharacterCards() 
{
    if (characterCards) {
        delete[] characterCards; // delete character cards
        characterCards = nullptr; // Set pointer to nullptr so we dont get problems with old data
    }
}

/**
 * @brief Safely cleans up character selection stuff. This function checks if CharSelectionStuff is not null, then deletes the array and sets the pointer to nullptr. This is just an int array so nothing fancy needed.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupCharSelectionStuff() 
{
    if (CharSelectionStuff) {
        delete[] CharSelectionStuff; // delete the selection data
        CharSelectionStuff = nullptr; // nullptr it
    }
}

/**
 * @brief Safely cleans up stat lines. This function checks if allStatLines is not null, then clears the stringstream, deletes it, and sets the pointer to nullptr. Stringstreams are weird so we gotta clear them first before deleting.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupStatLines() 
{
    if (allStatLines) {
        allStatLines->clear(); // Clear the stringstream flags (you gotta do this with stringstreams)
        allStatLines->str(""); // Clear the stringstream content (make it empty)
        delete allStatLines; // now we can safely delete it
        allStatLines = nullptr; // and nullptr it
    }
}

/**
 * @brief Safely cleans up the intro crawl text. This function checks if scrollIntroCrawl is not null, then clears the stringstream, deletes it, and sets the pointer to nullptr. Same deal as the stat lines cleanup.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupIntroCrawl() 
{
    if (scrollIntroCrawl) {
        scrollIntroCrawl->clear(); // clear flags first
        scrollIntroCrawl->str(""); // then clear the actual text
        delete scrollIntroCrawl; // Delete it
        scrollIntroCrawl = nullptr; // and nullptr
    }
}

/**
 * @brief Safely cleans up nerd font. This function checks if nerdFont is not null, then unloads the font, deletes the pointer, and sets it to nullptr. Fonts need to be unloaded just like textures or raylib gets confused.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupNerdFont() 
{
    if (nerdFont) {
        UnloadFont(*nerdFont); // tell raylib to unload the font from memory
        delete nerdFont; // Delete the pointer
        nerdFont = nullptr; // Set to nullptr cause were responsible programmers
    }
}

/**
 * @brief Safely cleans up game sounds. This function checks if gameSounds is not null, then iterates through each sound, unloading them before deleting the array and setting the pointer to nullptr. Sound effects gotta be unloaded too or youll have audio memory leaks which is apperently a thing.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupGameSounds() 
{
    if (gameSounds) {
        for (int i = 0; i < TOTAL_SOUNDS; ++i) {
            UnloadSound(gameSounds[i]); // unload each sound one by one
        }
        delete[] gameSounds; // Delete the array of sounds
        gameSounds = nullptr; // nullptr it
    }
}

/**
 * @brief Safely cleans up entities array. This function checks if entities is not null, then deletes each entity and sets pointers to nullptr before deleting the array and setting the pointer to nullptr. This ones more complicated cause we got pointers inside pointers.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupEntities() 
{
    if (entities) 
    {
        //If player character exists in array delete them first
        if (entities[0]) 
        {
            delete entities[0]; // delete player
            entities[0] = nullptr; // nullptr the player slot
        }

        //If enemy character exists in array delete them too
        if (entities[1]) 
        {
            delete entities[1]; // delete enemy
            entities[1] = nullptr; // nullptr the enemy slot
        }
        delete[] entities; // now delete the array itself
        entities = nullptr; // and nullptr the whole thing
    }
}

/**
 * @brief Safely cleans up all screen resources including textures, rects, character cards, selection state, and stat lines. This is like a convienience function that calls all the other cleanup functions so you dont have to remeber all of them.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void CleanupAllScreenResources() 
{
    // just call all the cleanup functions in order
    // this is easier than remembering to call each one seperately
    CleanupScreenTextures();
    CleanupScreenRects();
    CleanupCharacterCards();
    CleanupCharSelectionStuff();
    CleanupStatLines();
}


//======================= GAMESCENE FUNCTION DEFINITIONS =======================
/*
    These functions are used to initialize and manage game scenes. Game scenes are basically
    all the diffrent rooms and locations in the game that the player can walk around in.
    Each scene has its own background image, navigation arrows, items to pick up, and maybe
    an enemy encounter. Its like a point-and-click adventure game sorta.
    
    The way it works is we have a big vector of GameScene structs and each one has all the
    info about that location. Then we just swap between them when the player clicks arrows.
*/

/**
 * @brief Checks if an item has been collected by comparing the item name with the collected items list. This is used to determine if an item should be displayed in the scene or not cause we dont want items to respawn after you already grabbed them.
 * @param itemName The name of the item to check for collection status (like "Key 1" or "Health Potion").
 * @return true if the item has been collected already, false if its still there to grab.
 * @version 1.0
 * @author Edwin Baiden
 */
bool isItemCollected(const std::string& itemName) {
    // loop through all collected items and see if this one is in there
    for (const auto& item : collectedItems) // Iterate through collected items
        if (item == itemName) return true; // found it! player already has this
    return false; // not found so its still available to pick up
}

/**
 * @brief Initializes all game scenes with their respective textures, arrows (and where they lead to), items, and encounters. This function sets up the entire game world structure for exploration mode. Its a big function cause theres alot of scenes to set up. Currently only supports Student character type but we can add more later.
 * @param playerCharacter Pointer to the player character (used for scene initialization per character type [has not been fully implemented due to time constraints]).
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void InitGameScenes(Character* playerCharacter) 
{
    
    // Clear existing scenes if any (start fresh)
    gameScenes.clear();
    
    //Using dynamic_cast to check if the player character is of type Student
    // if its a student we load the student version of the game world
    // other character types would have diffrent layouts but we didnt have time for that
    if (dynamic_cast<Student*>(playerCharacter)) 
    {
        // Initialize scenes for Student character
        // this is gonna be a long one

        // Load screen textures for all the exploration environments
        // theres alot of them cause every room needs a background
        numScreenTextures = TOTAL_EXP_TEX;
        ScreenTextures = new Texture2D[numScreenTextures];

        ChangeDirectory(GetApplicationDirectory()); // Change to application directory so that relative paths works (cause MacOS is picky about file paths)
        
        // Load all the environment textures for different locations in the building
        // these are all the hallway and room backgrounds
        ScreenTextures[TEX_ENTRANCE] = LoadTexture("../assets/images/environments/Building1/Hallway/Entrance.png");
        ScreenTextures[TEX_EXIT] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-4].png");
        ScreenTextures[TEX_FRONT_OFFICE] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-2].png");
        ScreenTextures[TEX_WEST_HALLWAY_TOWARD] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-1].png");
        ScreenTextures[TEX_WEST_HALLWAY_AWAY] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[1-2].png");
        ScreenTextures[TEX_EAST_HALLWAY_TOWARD] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[2-3].png");
        ScreenTextures[TEX_EAST_HALLWAY_AWAY] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[3-1].png");
        ScreenTextures[TEX_CLASSROOM_1] = LoadTexture("../assets/images/environments/Building1/Class-Office/Classroom1.png");
        ScreenTextures[TEX_CLASSROOM_2] = LoadTexture("../assets/images/environments/Building1/Class-Office/Classroom2.png");
        ScreenTextures[TEX_CLASSROOM_3] = LoadTexture("../assets/images/environments/Building1/Class-Office/ClassroomZombies.png"); // spooky classroom with zombies
        ScreenTextures[TEX_IN_OFFICE] = LoadTexture("../assets/images/environments/Building1/Class-Office/Office.png");
        ScreenTextures[TEX_BATH_MEN] = LoadTexture("../assets/images/environments/Building1/Bathrooms/BathroomM.png"); // mens room
        ScreenTextures[TEX_BATH_WOM] = LoadTexture("../assets/images/environments/Building1/Bathrooms/BathroomG.png"); // womens room
        ScreenTextures[TEX_OUTSIDE] = LoadTexture("../assets/images/environments/Building1/Hallway/finalScene[1].png"); // outside area
        
        // Load item textures that can be picked up in the game
        // keys, potions, weapons, the usual RPG stuff
        ScreenTextures[TEX_KEY_1] = LoadTexture("../assets/images/items/Key1.png"); // first key
        ScreenTextures[TEX_KEY_2] = LoadTexture("../assets/images/items/Key2.png"); // second key
        ScreenTextures[TEX_HEALTH_POTION] = LoadTexture("../assets/images/items/HealthPotion.png"); // heals you
        ScreenTextures[TEX_BAT] = LoadTexture("../assets/images/items/BaseballBat.png"); // weapon
        
        // Load UI elements for navigation and minimap
        ScreenTextures[TEX_ARROW] = LoadTexture("../assets/images/UI/explorationArrow.png"); // the clickable arrows
        ScreenTextures[TEX_MINIMAP] = LoadTexture("../assets/images/environments/Building1/NewLayout.png"); // birds eye view of building
        ScreenTextures[TEX_TURTLE] = LoadTexture("../assets/images/UI/turtleIcon.png"); // player icon on minimap

        // Initialize game scenes array to hold all the different locations
        // resize it to fit all our scenes
        gameScenes.resize(TEX_OUTSIDE + 1);

        // ==================== ENTRANCE SCENE ====================
        // This is where the player starts when they begin the game
        // its the front entrance of the building
        GameScene* s = &gameScenes[TEX_ENTRANCE]; // get pointer to this scene for easier access
        s->sceneName = "Entrance"; // what shows up on the minimap
        s->textureIndex = TEX_ENTRANCE; // which background to use
        s->minimapCoords = {0.475f, 0.8f}; // where the turtle goes on the minimap (normalized 0-1 coords)
        s->minimapRotation = 0.0f; // which way the turtle faces
        s->sceneArrows = {
            // these are all the clickable arrows in this scene
            // format: {{x, y, width, height}, direction, where it goes, is it usable, hover text, required key}
            {{550, 500, 150, 150}, LEFT, TEX_WEST_HALLWAY_AWAY, true, "Go West", ""}, // go left
            {{1220, 500, 150, 150}, RIGHT, TEX_EAST_HALLWAY_TOWARD, true, "Go East", ""}, // go right
            {{885, 650, 150, 150}, UP, TEX_FRONT_OFFICE, true, "Go to Office Front", ""}, // go forward
            {{885, 875, 150, 150}, DOWN, TEX_EXIT, true, "Exit Building", ""} // exit but needs key 2 first
        };

        // ==================== EXIT SCENE ====================
        // This is outside the building - the final area basically
        // theres a frat bro zombie here
        s = &gameScenes[TEX_EXIT];
        s->sceneName = "Exit";
        s->textureIndex = TEX_EXIT;
        s->environmentTexture = "../assets/images/environments/Building1/Hallway/Hallway[2-4].png"; // used as combat background when fighting here
        s->minimapCoords = {0.5f, 0.825f};
        s->minimapRotation = 180.0f; // facing the other way
        s->sceneArrows = {{{885, 875, 150, 150}, DOWN, TEX_ENTRANCE, true, "Enter Building", ""},
                          {{885, 650, 150, 150}, UP, TEX_OUTSIDE, true, "Exit Building", ""}}; // can go back inside
        s->hasEncounter = false; // theres a fight here
        s->encounterID = 0; // its encounter number 2 (the frat bro)
        // Combat positioning values - where to draw stuff during the fight
        // these took a while to get right, lots of trial and error
        s->combatBgX = (SCREEN_CENTER_X - (ScreenTextures[TEX_EXIT].width) / 2.0f);
        s->combatBgY = (SCREEN_CENTER_Y - (ScreenTextures[TEX_EXIT].height) / 2.0f- 175.0f);
        s->playerCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_EXIT].width) / 2.0f - 450.0f);
        s->playerCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_EXIT].height) / 2.0f - 700.0f);
        s->enemyCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_EXIT].width) / 2.0f - 675.0f);
        s->enemyCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_EXIT].height) / 2.0f - 750.0f);
        s->playerScale = {600.0f,650.0f}; // how big to draw the player
        s->enemyScale = {400.0f,500.0f}; // how big to draw the enemy

        // ==================== FRONT OFFICE SCENE ====================
        // this is the hallway in front of the main office
        // connects to a bunch of other areas so its like a hub
        s = &gameScenes[TEX_FRONT_OFFICE];
        s->sceneName = "Office Front";
        s->textureIndex = TEX_FRONT_OFFICE;
        s->minimapCoords = {0.45f, 0.475f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {
            {{550, 725, 150, 150}, LEFT, TEX_WEST_HALLWAY_TOWARD, true, "Go West", ""}, // west hallway
            {{1250, 725, 150, 150}, RIGHT, TEX_EAST_HALLWAY_TOWARD, true, "Go East", ""}, // east hallway
            {{885, 875, 150, 150}, DOWN, TEX_EXIT, true, "Exit Building", "Key 2"}, // exit (need key)
            {{885, 650, 150, 150}, UP, TEX_IN_OFFICE, true, "Enter Office", ""} // go into the office
        };

        // ==================== WEST HALLWAY (TOWARD) SCENE ====================
        // west hallway looking toward the end (toward the classrooms)
        // the naming is confusing i know but "toward" means facing that direction
        s = &gameScenes[TEX_WEST_HALLWAY_TOWARD];
        s->sceneName = "West Hallway";
        s->textureIndex = TEX_WEST_HALLWAY_TOWARD;
        s->minimapCoords = {0.25f, 0.475f};
        s->minimapRotation = 270.0f; // facing west
        s->sceneArrows = {
            {{500, 535, 150, 150}, LEFT, TEX_CLASSROOM_1, true, "Enter Classroom 1", ""}, // classroom 1 (has professor zombie)
            {{1250, 535, 150, 150}, RIGHT, TEX_CLASSROOM_2, true, "Enter Classroom 2", "Key 1"}, // classroom 2 (locked, need key 1)
            {{875, 750, 150, 150}, DOWN, TEX_WEST_HALLWAY_AWAY, true, "Return East", ""} // turn around
        };

        // ==================== WEST HALLWAY (AWAY) SCENE ====================
        // west hallway but looking the other way (back toward center)
        // "away" means facing away from that direction
        s = &gameScenes[TEX_WEST_HALLWAY_AWAY];
        s->sceneName = "West Hallway";
        s->textureIndex = TEX_WEST_HALLWAY_AWAY;
        s->minimapCoords = {0.2f, 0.475f};
        s->minimapRotation = 90.0f; // facing east now
        s->sceneArrows = {
            {{855, 850, 150, 150}, DOWN, TEX_WEST_HALLWAY_TOWARD, true, "Return West", ""}, // turn back around
            {{855, 550, 150, 150}, UP, TEX_EAST_HALLWAY_TOWARD, true, "Go East", ""}, // shortcut to east
            {{500, 500, 150, 150}, LEFT, TEX_FRONT_OFFICE, true, "Go to Office Entrance", ""}, // back to hub
            {{1250, 500, 150, 150}, RIGHT, TEX_EXIT, true, "Exit Building", "Key 2"} // exit shortcut
        };

        // ==================== EAST HALLWAY (TOWARD) SCENE ====================
        // east hallway facing toward the end (bathrooms and classroom 3)
        s = &gameScenes[TEX_EAST_HALLWAY_TOWARD];
        s->sceneName = "East Hallway";
        s->textureIndex = TEX_EAST_HALLWAY_TOWARD;
        s->minimapCoords = {0.675f, 0.475f};
        s->minimapRotation = 90.0f;
        s->sceneArrows = {
            {{885, 600, 150, 150}, UP, TEX_CLASSROOM_3, true, "Enter Classroom 3", ""}, // spooky zombie classroom
            {{500, 600, 150, 150}, LEFT, TEX_BATH_MEN, true, "Enter Men's Bathroom", ""}, // mens room
            {{1350, 600, 150, 150}, RIGHT, TEX_BATH_WOM, true, "Enter Women's Bathroom", ""}, // womens room
            {{885, 850, 150, 150}, DOWN, TEX_EAST_HALLWAY_AWAY, true, "Go West", ""} // turn around
        };

        // ==================== EAST HALLWAY (AWAY) SCENE ====================
        // east hallway looking back toward the center
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

        // ==================== CLASSROOM 1 SCENE ====================
        // first classroom - has a professor zombie fight and key 2 after you win
        // gotta beat the prof to get the key to escape
        s = &gameScenes[TEX_CLASSROOM_1];
        s->sceneName = "Classroom 1";
        s->textureIndex = TEX_CLASSROOM_1;
        s->environmentTexture = "../assets/images/environments/Building1/Class-Office/Classroom1.png";
        s->minimapCoords = {0.19f, 0.625f};
        s->minimapRotation = 180.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_WEST_HALLWAY_TOWARD, true, "Exit Classroom", ""}}; // only way out
        // the key only shows up AFTER you beat the zombie (requiresVictory = true)
        // so you cant just grab it and run
        s->sceneItems = {{"Key 2", "Pick up Key 2", {600, 625, 150, 150}, TEX_KEY_2, true}};
        s->hasEncounter = true; // fight time
        s->encounterID = 0; // Professor zombie encounter (hes encounter 0)
        // all the combat positioning stuff again
        s->combatBgX = (SCREEN_CENTER_X - (ScreenTextures[TEX_CLASSROOM_1].width) / 2.0f);
        s->combatBgY = (SCREEN_CENTER_Y - (ScreenTextures[TEX_CLASSROOM_1].height) / 2.0f- 175.0f);
        s->playerCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_CLASSROOM_1].width) / 2.0f - 500.0f);
        s->playerCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_CLASSROOM_1].height) / 2.0f - 790.0f);
        s->enemyCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_CLASSROOM_1].width) / 2.0f - 670.0f);
        s->enemyCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_CLASSROOM_1].height) / 2.0f - 795.0f);
        s->playerScale = {600.0f,700.0f};
        s->enemyScale = {400.0f,400.0f};


        // ==================== CLASSROOM 2 SCENE ====================
        // second classroom - no fight here just a free health potion
        // but you need key 1 to get in (which is in the office)
        s = &gameScenes[TEX_CLASSROOM_2];
        s->sceneName = "Classroom 2";
        s->textureIndex = TEX_CLASSROOM_2;
        s->minimapCoords = {0.15f, 0.325f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_WEST_HALLWAY_TOWARD, true, "Exit Classroom", ""}};
        // health potion is always available (requiresVictory = false) cause no fight here
        s->sceneItems = {{"Health Potion", "Pick up Health Potion", {500, 480, 150, 150}, TEX_HEALTH_POTION, false}};

        // ==================== CLASSROOM 3 SCENE ====================
        // third classroom - just has spooky zombie decorations
        // no items or fights, its just for atmosphere
        s = &gameScenes[TEX_CLASSROOM_3];
        s->sceneName = "Classroom 3";
        s->textureIndex = TEX_CLASSROOM_3;
        s->minimapCoords = {0.15f, 0.65f};
        s->minimapRotation = 90.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_EAST_HALLWAY_TOWARD, true, "Exit Classroom", ""}};
        // nothing here, we could add stuff later if we want

        // ==================== OFFICE SCENE ====================
        // the main office - has a sorority zombie fight
        // also has key 1 and a baseball bat (weapon upgrade)
        // this is probably the most important room besides the exit
        s = &gameScenes[TEX_IN_OFFICE];
        s->sceneName = "Office";
        s->textureIndex = TEX_IN_OFFICE;
        s->environmentTexture = "../assets/images/environments/Building1/Class-Office/Office.png";
        s->minimapCoords = {0.45f, 0.35f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_FRONT_OFFICE, true, "Exit Office", ""}};
        // TWO items here, key 1 and baseball bat, both always available
        s->sceneItems = {
            {"Key 1", "Pick up Key 1", {600, 400, 90, 90}, TEX_KEY_1, false}, // smaller hitbox for key
            {"Baseball Bat", "Pick up Baseball Bat", {800, 500, 300, 150}, TEX_BAT, false} // bigger hitbox for bat
        };
        s->hasEncounter = true;
        s->encounterID = 1; // Sorority zombie is encounter 1
        // more positioning numbers
        s->combatBgX = (SCREEN_CENTER_X - (ScreenTextures[TEX_IN_OFFICE].width) / 2.0f);
        s->combatBgY = (SCREEN_CENTER_Y - (ScreenTextures[TEX_IN_OFFICE].height) / 2.0f- 150.0f);
        s->playerCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_IN_OFFICE].width) / 2.0f - 500.0f);
        s->playerCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_IN_OFFICE].height) / 2.0f - 1075.0f);
        s->enemyCharX = (SCREEN_CENTER_X + (ScreenTextures[TEX_IN_OFFICE].width) / 2.0f - 700.0f);
        s->enemyCharY = (SCREEN_CENTER_Y + (ScreenTextures[TEX_IN_OFFICE].height) / 2.0f - 1295.0f);
        s->playerScale = {700.0f,700.0f};
        s->enemyScale = {300.0f,500.0f};

        // ==================== MEN'S BATHROOM SCENE ====================
        // mens bathroom - nothing here, just exists for realism
        // maybe we could add a secret item here later
        s = &gameScenes[TEX_BATH_MEN];
        s->sceneName = "Men's Bathroom";
        s->textureIndex = TEX_BATH_MEN;
        s->minimapCoords = {0.85f, 0.325f};
        s->minimapRotation = 0.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_EAST_HALLWAY_TOWARD, true, "Exit Bathroom", ""}};

        // ==================== WOMEN'S BATHROOM SCENE ====================
        // womens bathroom - also nothing here
        s = &gameScenes[TEX_BATH_WOM];
        s->sceneName = "Women's Bathroom";
        s->textureIndex = TEX_BATH_WOM;
        s->environmentTexture = "../assets/images/environments/Building1/Class-Office/BathroomG.png";
        s->minimapCoords = {0.8f, 0.6f};
        s->minimapRotation = 180.0f;
        s->sceneArrows = {{{885, 855, 150, 150}, DOWN, TEX_EAST_HALLWAY_TOWARD, true, "Exit Bathroom", ""}};

        // ==================== OUTSIDE SCENE ====================
        // outside area - final scene after you exit the building
        s = &gameScenes[TEX_OUTSIDE];
        s->sceneName = "Outside";
        s->textureIndex = TEX_OUTSIDE;
        s->minimapCoords = {0.5f, 0.9f};
        s->minimapRotation = 180.0f;
        
        
    }
    // if we had time we would add more character types here with different maps
}

//======================= SOUND INITIALIZATION =======================

/**
 * @brief Initializes all game sounds by loading sound files from the assets folder. This function allocates a new array of sounds and loads each sound file into it. Should be called once at game startup otherwise youll have no sounds and the game will be quiet.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void InitGameSounds() 
{
    // create array to hold all our sounds
    gameSounds = new Sound[TOTAL_SOUNDS];
    // load each sound from file
    gameSounds[SND_SELECT] = LoadSound("../assets/sfx/select.wav"); // UI click sound
    gameSounds[SND_HIT] = LoadSound("../assets/sfx/hitHurt.wav"); // sound when someone gets hit
    gameSounds[SND_HEAL] = LoadSound("../assets/sfx/heal.wav"); // healing sound
    gameSounds[SND_ZOM_DEATH] = LoadSound("../assets/sfx/explosion.wav"); // zombie death sound
    gameSounds[SND_ZOM_GROAN] = LoadSound("../assets/sfx/zombieGroan.wav"); // creepy zombie noise
    
}

//======================= GUI STYLE FUNCTIONS =======================
/*
    These functions set the visual styles for different GUI elements using raygui's style system.
    Basically they make the buttons look different on different screens.
    Each screen has its own vibe so the buttons need to match.
    
    The hex codes are colors in RGBA format (like 0xRRGGBBAA)
    figuring out good colors took a while
*/

/**
 * @brief Sets the default GUI styles for buttons and text. This is the baseline style that other styles can override. Uses neutral gray colors for a standard look.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void defaultStyles() {
    // just a bunch of gray colors for default buttons
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x828282FF); // medium gray border
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0xB6B6B6FF); // lighter when hovering
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xDADADAFF); // even lighter when clicked
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xE0E0E0FF); // light gray background
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0xC4C4C4FF); // slightly darker on hover
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0xA8A8A8FF); // even darker when pressed
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0x000000FF); // black text
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0x000000FF); // still black
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0x000000FF); // yep still black
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // reasonable text size
}

/**
 * @brief Sets the GUI styles for the start menu screen. Uses dark semi-transparent buttons with white text to look good over the menu background image. Large text size (56) for menu buttons cause theyre important and should be easy to read.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void startMenuStyles() {
    // dark transparent buttons that look cool over the background
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x646464FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x969696FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xC8C8C8FF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x000000B4); // Semi-transparent black (B4 is the alpha)
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x323232C8); // slightly lighter and more opaque
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x646464DC); // even more visible when pressed
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF); // White text so you can actually read it
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF); // still white
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF); // white
    GuiSetStyle(DEFAULT, TEXT_SIZE, 56); // BIG text for the menu
}

/**
 * @brief Sets the GUI styles for the player/character selection screen. Uses green-themed colors to match the game's aesthetic. Also sets up disabled button styles for characters that cant be selected yet (which is most of them, we only have student working).
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void playerSelectStyles() {
    // green theme cause it looked good
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x006600FF); // Dark green border
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x008800FF); // brighter green on hover
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0x00CC00FF); // bright green when clicked
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000); // completely Transparent background
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x003300C8); // dark green tint on hover
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x006600DC); // darker green when pressed
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF); // white text
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36); // medium-large text
    GuiSetStyle(BUTTON, BORDER_WIDTH, 6); // thick borders
    // Disabled button styles for characters that arent implemented yet
    // they look grayed out so players know they cant click them
    GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED, 0x555555FF);
    GuiSetStyle(BUTTON, BASE_COLOR_DISABLED, 0x222222B4);
    GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED, 0x888888FF); // gray text
}

/**
 * @brief Sets the GUI styles for the gameplay screen and also loads the nerd font for icons. The nerd font is this cool font that has icons built into it like swords and shields and stuff. Without it we wouldnt have the fancy icons in combat. Uses player select styles as base cause they already look pretty good.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void gamePlayStyles() {
    CleanupNerdFont(); // get rid of old font if theres one loaded already
    nerdFont = new Font(); // make a new font pointer
    // these are the codepoints (basically character codes) for the icons we need
    // nerd fonts have alot of icons
    int codepoints[11] = {ICON_SWORD, ICON_BOW_ARROW, ICON_POISON, ICON_FIRE, ICON_ARROW_DOWN, 
                          ICON_ARROW_UP, ICON_PLUS, ICON_SNAIL, ICON_LIGHTNING, 
                          ICON_SHIELD, ICON_PAUSE};
    ChangeDirectory(GetApplicationDirectory()); // gotta change directory again (cause MacOS is picky about file paths)
    *nerdFont = LoadFontEx("../assets/fonts/JetBrainsMonoNLNerdFontMono-Bold.ttf", 32, codepoints, 11);
    SetTextureFilter(nerdFont->texture, TEXTURE_FILTER_BILINEAR); // makes the font look smooth instead of pixely
    playerSelectStyles(); // use the green theme for gameplay too
}


/**
 * @brief Reads the intro crawl text from files and stores it in the provided stringstream. Loads the general intro first, then appends character-specific intro based on the chosen character index. Its like star wars but for our game.
 * @param ss Pointer to the stringstream where the intro text will be stored (we write to this).
 * @param chosenCharacterIdx Index of the chosen character (0=Student, 1=Rat, 2=Professor, 3=Attila). Only student works rn tho.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void getIntroCrawlText(std::stringstream *ss, int chosenCharacterIdx) {
    if (!ss) return; // Safety check - dont do anything if we got a null pointer
    ss->str(""); // Clear existing content (start fresh)
    ss->clear(); // Clear any error flags that might be set

    ChangeDirectory(GetApplicationDirectory()); // directory change
    std::ifstream file("../dat/general_Intro.txt"); // open the general intro file
    if (!file.is_open()) {
        // couldnt open the file
        (*ss) << "Error: Unable to open intro crawl text file.";
        return; // bail out
    }

    // Read general intro text line by line
    // double newlines between lines for that star wars spacing
    std::string line;
    while (std::getline(file, line)) (*ss) << line << "\n\n";
    file.close(); // always close your files
    (*ss) << "\n\n\n"; // Add extra spacing before character-specific intro

    // File paths for each character's specific intro
    // each character has their own backstory
    const char* charFiles[] = {
        "../dat/Student_Intro.txt",   // student intro
        "../dat/Rat_Intro.txt",       // rat intro (not implemented)
        "../dat/Professor_Intro.txt", // professor intro (not implemented)
        "../dat/Attila_Intro.txt"     // attila intro (not implemented)
    };

    // Load character-specific intro if the index is valid
    if (chosenCharacterIdx >= 0 && chosenCharacterIdx < 4) {
        file.open(charFiles[chosenCharacterIdx]);
        if (file.is_open()) {
            while (std::getline(file, line)) (*ss) << line << "\n\n";
            file.close();
        }
        // if file doesnt open we just dont add character specific text
    }
}

/**
 * @brief Draws the status effects panel showing all active buffs and debuffs for an entity. Uses icons from the nerd font to display status effects like poisoned, burning, regenerating etc. Red icons for bad stuff (debuffs), green for good stuff (buffs). Makes combat easier to understand at a glance.
 * @param panel Rectangle defining where to draw the status panel on screen.
 * @param entityStatEff StatusEffects struct containing all the active status effects for the entity (player or enemy).
 * @param fnt Reference to the nerd font used for drawing the fancy icons.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void DrawStatusPanel(const Rectangle &panel, const StatusEffects &entityStatEff, const Font &fnt) {
    // lil struct to hold info about each status we need to draw
    struct StatusType { const char *Effect; std::string Icon; Color GoodOrBadEff; };
    std::vector<StatusType> activeStatEffects; // list of currently active effects

    // Check each possible status effect and add to the list if its active
    // RED = bad stuff happening to you
    if (entityStatEff.isPoisoned) activeStatEffects.push_back({"POISONED", CodepointToUTF8(ICON_POISON, &byteSize), RED});
    if (entityStatEff.isBurning) activeStatEffects.push_back({"BURNING", CodepointToUTF8(ICON_FIRE, &byteSize), RED});
    if (entityStatEff.isWeakened) activeStatEffects.push_back({"WEAKENED", CodepointToUTF8(ICON_ARROW_DOWN, &byteSize), RED});
    if (entityStatEff.isSlowed) activeStatEffects.push_back({"SLOWED", CodepointToUTF8(ICON_SNAIL, &byteSize), RED}); // snail = slow
    // GREEN = good stuff
    if (entityStatEff.isStrengthened) activeStatEffects.push_back({"STRENGTHENED", CodepointToUTF8(ICON_ARROW_UP, &byteSize), GREEN});
    if (entityStatEff.isRegenerating) activeStatEffects.push_back({"REGENERATING", CodepointToUTF8(ICON_PLUS, &byteSize), GREEN});
    if (entityStatEff.isFast) activeStatEffects.push_back({"FAST", CodepointToUTF8(ICON_LIGHTNING, &byteSize), GREEN});
    if (entityStatEff.defending) activeStatEffects.push_back({"DEFENDING", CodepointToUTF8(ICON_SHIELD, &byteSize), GREEN});

    // now draw each active status effect
    for (size_t i = 0; i < activeStatEffects.size(); ++i) {
        // Draw the status effect name text on the left side
        DrawTextEx(GetFontDefault(), activeStatEffects[i].Effect,
                   {panel.x + 8.0f, 
                    panel.y + 8.0f + (i * 28.0f) + ((28.0f - MeasureTextEx(GetFontDefault(), activeStatEffects[i].Effect, 24.0f, 1.0f).y) / 2.0f)}, // centered vertically in the row
                   24.0f, 1.0f, activeStatEffects[i].GoodOrBadEff);
        
        // Draw the icon on the right side of the panel
        DrawTextEx(fnt, activeStatEffects[i].Icon.c_str(),
                   {panel.x + panel.width - 8.0f - MeasureTextEx(fnt, activeStatEffects[i].Icon.c_str(), 44.0f, 1.0f).x, // right aligned
                    panel.y + 8.0f + (i * 28.0f) + ((28.0f - MeasureTextEx(fnt, activeStatEffects[i].Icon.c_str(), 44.0f, 1.0f).y) / 2.0f)},
                   44.0f, 1.0f, activeStatEffects[i].GoodOrBadEff);
    }
}

//=================== SCREENMANAGER CLASS ===================
/*
    The ScreenManager class is the main controller for screen management.
    It handles all the transitions between different screens like main menu, character
    select, intro crawl, and gameplay. Each screen has its own resources that need to
    be loaded and unloaded so we dont waste memory.
    
    It also does this cool thing where it renders to a texture first and then scales
    that texture to fit whatever window size the player has. This means the game looks
    the same whether youre on a tiny laptop or a huge monitor.
*/

/**
 * @brief Constructor for ScreenManager. Initializes the screen manager with an initial screen state and sets default scale and offset values. Nothing fancy just setting up the basics.
 * @param initial The initial ScreenState to start with (usually MAIN_MENU cause thats where games start).
 * @version 1.0
 * @author Edwin Baiden
 */
ScreenManager::ScreenManager(ScreenState initial) : currentScreen(initial), scale(1.0f), offset{0.0f, 0.0f} {}

/**
 * @brief Destructor for ScreenManager. Cleans up all resources including render texture, current screen resources, and persistent game resources like sounds, entities, fonts etc. Gotta clean up after ourselves or memory leaks will happen.
 * @version 1.0
 * @author Edwin Baiden
 */
ScreenManager::~ScreenManager() {
    UnloadRenderTexture(target); // unload the render texture we use for scaling
    exitScreen(currentScreen); // clean up whatever screen were on
    
    // Clean up persistent resources that last the entire game session
    // these are things that exist across multiple screens
    CleanupGameSounds();
    CleanupEntities();
    CleanupStatLines();
    CleanupNerdFont();
    CleanupIntroCrawl();
}

/**
 * @brief Initializes the screen manager by creating the render texture and loading initial resources. This should be called once after creating the ScreenManager. If you forget to call this everything will break.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void ScreenManager::init() {
    ChangeDirectory(GetApplicationDirectory()); // directory stuff (cause MacOS is picky about file paths)
    target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT); // Create render texture for resolution scaling
    InitGameSounds(); // Load all game sounds so we can hear things
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // makes scaling look smooth
    enterScreen(currentScreen); // Enter the initial screen and load its stuff
}

/**
 * @brief Changes the current screen to a new screen state. Handles exiting the old screen and entering the new one, including resource cleanup and loading. This is how you go from like the menu to the game and stuff.
 * @param newScreen The ScreenState to transition to (where we wanna go).
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void ScreenManager::changeScreen(ScreenState newScreen) {
    if (newScreen == currentScreen) return; // already there, no need to do anything
    exitScreen(currentScreen); // Clean up current screen resources
    currentScreen = newScreen; // update what screen were on
    enterScreen(currentScreen); // Setup new screen and load its stuff
}

/**
 * @brief Returns the current screen state. Just a getter function nothing special.
 * @return ScreenState The current screen state enum value (which screen were on rn).
 * @version 1.0
 * @author Edwin Baiden
 */
ScreenState ScreenManager::getCurrentScreen() const {
    return currentScreen; // here you go
}

/**
 * @brief Converts the actual mouse position to virtual (game resolution) coordinates. This is needed because the game renders at a fixed resolution that gets scaled to fit the window. Without this clicks would be in the wrong spot and players would be confused.
 * @return Vector2 The mouse position in virtual/game coordinates (where the mouse ACTUALLY is in game terms).
 * @version 1.0
 * @author Edwin Baiden
 */
Vector2 ScreenManager::GetVirtualMousePosition() {
    Vector2 mouse = GetMousePosition(); // get where the mouse actually is on screen
    // do some math to convert to game coordinates
    return {
        std::clamp((mouse.x - offset.x) / scale, 0.0f, (float)GAME_SCREEN_WIDTH), // clamp so its not outside the game area
        std::clamp((mouse.y - offset.y) / scale, 0.0f, (float)GAME_SCREEN_HEIGHT)
    };
}

/**
 * @brief Updates the current screen logic based on delta time. Handles different update logic for each screen state including character card animations, intro crawl scrolling, and gameplay updates. Delta time is important so the game runs the same speed on fast and slow computers.
 * @param dt Delta time (time since last frame) in seconds. Usually like 0.016 for 60fps.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void ScreenManager::update(float dt) {
    UpdateMusicStream(backgroundMusic); // keep the music playing smoothly
    // Calculate scale and offset for resolution-independent rendering
    // this math figures out how to fit the game in the window
    scale = std::min((float)GetScreenWidth() / GAME_SCREEN_WIDTH, (float)GetScreenHeight() / GAME_SCREEN_HEIGHT);
    offset = {((float)GetScreenWidth() - ((float)GAME_SCREEN_WIDTH * scale)) * 0.5f,
              ((float)GetScreenHeight() - ((float)GAME_SCREEN_HEIGHT * scale)) * 0.5f};

    // do different stuff depending on which screen were on
    switch (currentScreen) {
    case ScreenState::MAIN_MENU:
        // Main menu doesnt really need updating, its all in render
        // just sitting there waiting for clicks
        break;

    case ScreenState::CHARACTER_SELECT: {
        if (!characterCards || !CharSelectionStuff) break; // safety check

        // Update character card textures (make sure they have the right images)
        for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            characterCards[i].texture = ScreenTextures[i + 1]; // +1 cause index 0 is background

        // Initialize character card positions if we havent yet
        if (!CharSelectionStuff[2]) { // [2] is the initialized flag
            for (int i = 0; i < MAX_CHAR_CARDS; ++i) {
                // Calculate where each card goes in the row
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
            CharSelectionStuff[2] = 1; // Mark as initialized so we dont do this again
        }

        // Update where cards should be moving to based on selection
        if (CharSelectionStuff[0] == -1) {
            // nothing selected - all cards stay in the normal row
            for (int i = 0; i < MAX_CHAR_CARDS; i++)
                characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
        } else {
            // something is selected, selected card goes to center
            characterCards[CharSelectionStuff[0]].targetAnimationPos = {
                CENTERED_X(CHARACTER_CARD_WIDTH),
                characterCards[CharSelectionStuff[0]].defaultRow.y,
                CHARACTER_CARD_WIDTH,
                CHARACTER_CARD_HEIGHT
            };

            // other cards slide off to the side (they dock over there)
            for (int i = 0, dockIndex = 0; i < MAX_CHAR_CARDS; ++i) {
                if (i != CharSelectionStuff[0]) { // not the selected one
                    characterCards[i].targetAnimationPos = {
                        CHARACTER_DOCK_X,
                        CHARACTER_DOCK_Y_START + CHARACTER_DOCK_SPACING * dockIndex++,
                        CHARACTER_CARD_WIDTH,
                        CHARACTER_CARD_HEIGHT
                    };
                }
            }
        }

        // Animate the cards moving smoothly to their target positions
        // this uses interpolation with easing for that nice smooth movement
        for (int i = 0; i < MAX_CHAR_CARDS; i++) {
            characterCards[i].currentAnimationPos.x = animation::slopeInt(
                characterCards[i].currentAnimationPos.x,
                characterCards[i].targetAnimationPos.x,
                animation::easeInOutCubic(1.0f - expf(-15.0f * dt))); // fancy easing math
            characterCards[i].currentAnimationPos.y = animation::slopeInt(
                characterCards[i].currentAnimationPos.y,
                characterCards[i].targetAnimationPos.y,
                animation::easeInOutCubic(1.0f - expf(-15.0f * dt)));
        }

        // Update play button position to be below the selected/first card
        ScreenRects[R_PLAY_BTN] = {
            CENTERED_X(PLAY_BTN_WIDTH),
            characterCards[CharSelectionStuff[0] == -1 ? 0 : CharSelectionStuff[0]].currentAnimationPos.y +
                CHARACTER_CARD_HEIGHT + PLAY_BTN_OFFSET_Y,
            PLAY_BTN_WIDTH,
            PLAY_BTN_HEIGHT
        };

        // reset mouse stuff
        SetMouseOffset(0, 0);
        SetMouseScale(1.0f, 1.0f);
        break;
    }

    case ScreenState::INTRO_CRAWL:
        if (!scrollIntroCrawl) break; // no text to scroll? skip
        // Scroll the intro crawl text upward like star wars
        introCrawlYPos -= INTRO_CRAWL_SPEED * dt;
        // when text goes off screen or player presses enter, move to gameplay
        if (introCrawlYPos <= INTRO_CRAWL_END_Y || IsKeyPressed(KEY_ENTER))
            changeScreen(ScreenState::GAMEPLAY);
        break;

    case ScreenState::GAMEPLAY:
        // gameplay has its own manager so just let it do its thing
        gameManager->update(dt);
        break;
    }
}

/**
 * @brief Renders the current screen to the render texture and then draws it scaled to the window. Handles all drawing for each screen state including UI, backgrounds, and game elements. This is where all the pictures actually get drawn to the screen.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void ScreenManager::render() {
    // Calculate scale and offset again (same as update, we need these here too)
    scale = std::min((float)GetScreenWidth() / GAME_SCREEN_WIDTH, (float)GetScreenHeight() / GAME_SCREEN_HEIGHT);
    offset = {((float)GetScreenWidth() - ((float)GAME_SCREEN_WIDTH * scale)) * 0.5f,
              ((float)GetScreenHeight() - ((float)GAME_SCREEN_HEIGHT * scale)) * 0.5f};

    // Set mouse offset and scale so GUI clicks work right
    SetMouseOffset(-offset.x, -offset.y);
    SetMouseScale(1.0f / scale, 1.0f / scale);

    // Begin rendering to the render texture (not directly to screen)
    BeginTextureMode(target);
    ClearBackground(BLACK); // start with black background

    // draw different stuff depending on which screen were on
    switch (currentScreen) {
    case ScreenState::MAIN_MENU:
    {
        // Draw the cool menu background
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);
        // Draw the game title logo thing
        DrawTexture(ScreenTextures[1], CENTERED_X(ScreenTextures[1].width), -150, WHITE);

        // START/RESTART button - if we loaded from save it says RESTART instead
        if (GuiButton(ScreenRects[0], !loadedFromSave ? "START" : "RESTART"))
        {
            changeScreen(ScreenState::CHARACTER_SELECT);
            // Reset all game state for new game (fresh start)
            loadedFromSave = false;
            activeEncounterID = -1;
            currentSceneIndex = TEX_ENTRANCE;
            savedPlayerSceneIndex = TEX_ENTRANCE;
            battleWon.clear(); // forget all won battles
            collectedItems.clear(); // forget all collected items
            // Clean up existing entities if any (delete old characters)
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
        
        // EXIT button - closes the whole game
        if (GuiButton(ScreenRects[2], "EXIT")) 
        {
            exitScreen(currentScreen);
            loadedFromSave = false;
            CloseWindow(); // goodbye
        }
        
        // RELOAD SAVED GAME button - only works if theres actually a save
        int prevStateMM = GuiGetState();
        if (!loadedFromSave) GuiDisable(); // gray it out if no save exists
        if(GuiButton(ScreenRects[1], "RELOAD SAVED GAME"))
        {
            changeScreen(ScreenState::GAMEPLAY); // jump straight to gameplay
        }
        GuiSetState(prevStateMM); // restore button state
        break;
    }
    case ScreenState::CHARACTER_SELECT: {
        // draw background
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);
        CharSelectionStuff[1] = -1; // Reset hover state (nothing hovered yet)

        // Draw all the character cards
        for (int i = 0; i < MAX_CHAR_CARDS; i++) {
            // Draw the card image, dim it if its not selected
            DrawTexturePro(characterCards[i].texture,
                        {0.0f, 0.0f, (float)characterCards[i].texture.width, (float)characterCards[i].texture.height},
                        characterCards[i].currentAnimationPos, 
                        {0.0f, 0.0f}, 
                        0.0f, 
                        CharSelectionStuff[0] == i ? WHITE : Color{100, 100, 100, 200} // selected = bright, others = dim
            );

            // Draw a green border around each card
            DrawRectangleLinesEx(characterCards[i].currentAnimationPos, 4.0f, Color{0, 68, 0, 255});

            // Check if mouse is over this card
            if (CheckCollisionPointRec(GetMousePosition(), characterCards[i].currentAnimationPos))
                CharSelectionStuff[1] = i; // remember which card is hovered

            // Only allow clicking on Student (index 0) cause thats all we have working
            // other characters would go here but we didnt have time
            if (i == 0 && GuiButton(characterCards[i].currentAnimationPos, ""))
            {
                CharSelectionStuff[0] = (CharSelectionStuff[0] == i) ? -1 : i; // Toggle selection (click again to deselect)
                PlaySound(gameSounds[SND_SELECT]); // click noise
            }

            // Draw fancy yellow selection highlight around selected card
            if (CharSelectionStuff[0] == i) {
                // double border for extra fanciness
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

        // Draw info box when hovering over a non-selected character
        // shows their stats so you know what youre getting into
        if (CharSelectionStuff[1] != -1 && CharSelectionStuff[1] != CharSelectionStuff[0]) {
            // figure out where to put the info box (next to the card, but keep it on screen)
            ScreenRects[R_INFO_BOX] = {
                (characterCards[CharSelectionStuff[1]].currentAnimationPos.x + CHARACTER_CARD_WIDTH + 260.0f < (float)GAME_SCREEN_WIDTH) ?
                    (characterCards[CharSelectionStuff[1]].currentAnimationPos.x + CHARACTER_CARD_WIDTH + 5.0f) :
                    (characterCards[CharSelectionStuff[1]].currentAnimationPos.x - 265.0f), // flip to other side if needed
                characterCards[CharSelectionStuff[1]].currentAnimationPos.y - 250.0f,
                260.0f, 240.0f
            };

            // Draw the info box background (dark green, semi transparent)
            DrawRectangleRec(ScreenRects[R_INFO_BOX], Color{0, 40, 0, 200});
            DrawRectangleLinesEx(ScreenRects[R_INFO_BOX], 3.0f, Color{40, 255, 80, 255});

            // Draw character name/type
            const char* charNames[] = {"Student", "Rat", "Professor", "Attila"};
            DrawText(TextFormat("Caste: %s", charNames[CharSelectionStuff[1]]),
                    (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 20), 24, WHITE);

            // only show stats for Student cause thats all we have data for
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
                // other characters just say not available cause we didnt implement them
                DrawText("Not Available", (int)(ScreenRects[R_INFO_BOX].x + 20), (int)(ScreenRects[R_INFO_BOX].y + 50), 20, WHITE);
            }
        }

        // Play Game button - cant click it unless youve selected a character
        int prevState = GuiGetState();
        if (CharSelectionStuff[0] == -1) GuiDisable(); // gray out if nothing selected

        if (GuiButton(ScreenRects[R_PLAY_BTN], "Play Game") && CharSelectionStuff[0] != -1) {
            // Create the player entity with the selected character type
            entities = new Character*[2]{nullptr, nullptr};
            CreateCharacter(entities, allStatLines, "Student", "Steve"); // player is named Steve
            // Setup the intro crawl text
            scrollIntroCrawl = new std::stringstream();
            getIntroCrawlText(scrollIntroCrawl, CharSelectionStuff[0]);
            introCrawlYPos = INTRO_CRAWL_START_Y; // start text at the bottom of screen

            EndTextureMode(); // gotta end this before changing screens
            changeScreen(ScreenState::INTRO_CRAWL); // go to the star wars text
            return; // bail out of this function
        }
        GuiSetState(prevState); // restore button state
        break;
    }

    case ScreenState::INTRO_CRAWL:
        // Draw the scrolling star wars style text
        if (scrollIntroCrawl) {
            scrollIntroCrawl->clear(); // clear error flags
            scrollIntroCrawl->seekg(0, std::ios::beg); // go back to start of text
            std::string line;

            // Draw each line at the right Y position
            for (float y = introCrawlYPos; std::getline(*scrollIntroCrawl, line); y += INTRO_CRAWL_LINE_HEIGHT) {
                if (!line.empty())
                    DrawText(line.c_str(), CENTERED_X(MeasureText(line.c_str(), INTRO_CRAWL_FONT_SIZE)),
                            (int)y, INTRO_CRAWL_FONT_SIZE, GOLD); // gold text like star wars
            }
            // helpful hint at the bottom
            DrawText("Press ENTER to skip", 20, (int)(float)GAME_SCREEN_HEIGHT - 40, 20, GRAY);
        }
        break;

    case ScreenState::GAMEPLAY:
        // gameplay handles its own rendering through the game manager
        gameManager->update(GetFrameTime()); // also update while were at it
        gameManager->render(); // let the game manager do the drawing
        // Check if we need to go back to main menu
        if (gameManager->backToMainMenu) {
            gameManager->backToMainMenu = false;
            delete gameManager;
            gameManager = nullptr;
            changeScreen(ScreenState::MAIN_MENU);
        }
        break;
    }

    EndTextureMode(); // done rendering to texture

    // Now draw the render texture scaled to fit the actual window
    BeginDrawing();
    ClearBackground(BLACK); // black bars on sides if aspect ratio doesnt match
    DrawTexturePro(target.texture,
                   {0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height}, // negative height cause render textures are upside down
                   {offset.x, offset.y, (float)GAME_SCREEN_WIDTH * scale, (float)GAME_SCREEN_HEIGHT * scale},
                   {0.0f, 0.0f}, 0.0f, WHITE);
    SetMouseOffset(0, 0);
    SetMouseScale(1.0f, 1.0f);
    EndDrawing();
}

/**
 * @brief Handles entering a new screen state by loading resources and setting up styles. Each screen needs different textures and stuff loaded so this handles all that. Its like unpacking your bags when you arrive somewhere new.
 * @param s The ScreenState being entered (where we just arrived).
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void ScreenManager::enterScreen(ScreenState s) {
    switch (s) {
    case ScreenState::MAIN_MENU: {
        startMenuStyles(); // set up the menu button styles

        
        // Load menu textures (just 2: background and title)
        numScreenTextures = 2;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png"); // cool background
        ScreenTextures[1] = LoadTexture("../assets/images/UI/gameTitle.png"); // game logo

        // Setup where the menu buttons go
        numScreenRects = 3;
        ScreenRects = new Rectangle[numScreenRects];
        ScreenRects[0] = {CENTERED_X(MAIN_BUTTON_WIDTH), SCREEN_CENTER_Y + MAIN_BUTTON_OFFSET_Y, MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT}; // start button
        ScreenRects[1] = {CENTERED_X(MAIN_BUTTON_WIDTH), SCREEN_CENTER_Y + MAIN_BUTTON_OFFSET_Y + MAIN_BUTTON_SPACING, MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT}; // load button
        ScreenRects[2] = {CENTERED_X(MAIN_BUTTON_WIDTH), SCREEN_CENTER_Y + MAIN_BUTTON_OFFSET_Y + 2 * MAIN_BUTTON_SPACING, MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT}; // exit button
        
        // Load character stats from CSV and try to load any saved game
        allStatLines = storeAllStatLines(openStartingStatsCSV());
        loadedFromSave = LoadProgress(entities, allStatLines, currentSceneIndex, activeEncounterID, savedPlayerSceneIndex, battleWon, collectedItems);

        if (!musicLoaded) 
        {
            backgroundMusic = LoadMusicStream("../assets/sfx/gamePlayMusic.mp3");
            backgroundMusic.looping = true;
            musicLoaded = true; // Set flag to true so we don't load it again
        }

        if (!IsMusicStreamPlaying(backgroundMusic)) 
        {
            PlayMusicStream(backgroundMusic);
        }
        break;
    }

    case ScreenState::CHARACTER_SELECT: {
        playerSelectStyles(); // green button theme
        
        // Allocate space for the character cards
        characterCards = new charCard[MAX_CHAR_CARDS];

        // CharSelectionStuff array: [0]=which is selected, [1]=which is hovered, [2]=initialized yet?
        CharSelectionStuff = new int[3]{-1, -1, 0}; // start with nothing selected

        // Load textures (background + 4 character portraits)
        numScreenTextures = 5;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png"); // same background as menu
        ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/south.png"); // student facing forward
        ScreenTextures[2] = LoadTexture("../assets/images/characters/pc/Rat-Assassin/rotations/south.png"); // rat (not playable yet)
        ScreenTextures[3] = LoadTexture("../assets/images/characters/pc/Professor-Mage/rotations/south.png"); // professor (not playable)
        ScreenTextures[4] = LoadTexture("../assets/images/characters/pc/Attila-Brawler/rotations/south.png"); // attila (also not playable)

        // rectangles will be set up in update()
        numScreenRects = 5;
        ScreenRects = new Rectangle[numScreenRects]{};
        break;
    }

    case ScreenState::INTRO_CRAWL:
        // nothing special to load, text was set up in character select
        break;

    case ScreenState::GAMEPLAY: {
        gamePlayStyles(); // load the nerd font and set styles
        
        // only setup gameplay if we have a player character
        if(entities && entities[0])
        { 
            InitGameScenes(entities[0]); // build the whole game world
            if (loadedFromSave) 
            {
                TraceLog(LOG_INFO, "Loading saved game state."); // debug message
            }
            gameManager = new GameManager; // create the gameplay manager
            // if we were in a fight when we saved, go back to that fight
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

/**
 * @brief Handles exiting a screen state by unloading resources and cleaning up. Gotta clean up before leaving or things get messy.
 * @param s The ScreenState being exited (where were leaving from).
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void ScreenManager::exitScreen(ScreenState s) {
    switch (s) {
    case ScreenState::MAIN_MENU:
    case ScreenState::CHARACTER_SELECT:
    case ScreenState::INTRO_CRAWL: {
        // Clean up intro crawl text if were leaving that screen
        if (scrollIntroCrawl && s == ScreenState::INTRO_CRAWL) {
            CleanupIntroCrawl();
        }
    }
    // NOTE: no break here, we fall through to cleanup textures etc for all screens
    case ScreenState::GAMEPLAY: {
        // Clean up game manager if leaving gameplay
        if (s == ScreenState::GAMEPLAY && gameManager) {
            gameManager->exitGameState(gameManager->getCurrentGameState());
            delete gameManager;
            gameManager = nullptr;
        }

        // Clean up all the screen-specific resources
        CleanupScreenTextures();
        CleanupScreenRects();
        CleanupCharacterCards();
        CleanupCharSelectionStuff();
        //CleanupStatLines(); // Keep stat lines cause we might need them for save/load
        break;
    }
    }
}

//=================== GAMEMANAGER CLASS ===================
/*
    The GameManager class is like a mini ScreenManager but just for gameplay stuff.
    It handles switching between exploration (walking around), combat (fighting zombies),
    and the pause menu. Each of these has its own update and render logic.
    
    This is probably the most complicated class in the project.
    Theres alot going on here with combat turns and exploration navigation and stuff.
*/

/**
 * @brief Constructor for GameManager. Initializes the game manager with an initial game state (usually exploration) and sets the working directory cause MacOS is picky about file paths.
 * @param initial The initial GameState to start with (default is EXPLORATION).
 * @version 1.0
 * @author Edwin Baiden
 */
GameManager::GameManager(GameState initial) : currentGameState(initial), nextGameState(initial), prevGameState(initial) 
{
    ChangeDirectory(GetApplicationDirectory()); // MacOS file path stuff
}

/**
 * @brief Destructor for GameManager. Cleans up resources by exiting the current game state properly.
 * @version 1.0
 * @author Edwin Baiden
 */
GameManager::~GameManager() 
{
    exitGameState(currentGameState);
}

/**
 * @brief Changes the current game state to a new state. Handles exiting the old state and entering the new one. This is how you go from walking around to fighting a zombie and back.
 * @param newState The GameState to transition to.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void GameManager::changeGameState(GameState newState) 
{
    if (newState == currentGameState) return; // already there, nothing to do
    nextGameState = newState; // remember where were going
    exitGameState(currentGameState); // clean up current state
    prevGameState = currentGameState; // remember where we were (for pause menu)
    currentGameState = newState; // update current state
    enterGameState(currentGameState); // setup new state
}

/**
 * @brief Returns the current game state. Simple getter.
 * @return GameState The current game state enum value.
 * @version 1.0
 * @author Edwin Baiden
 */
GameState GameManager::getCurrentGameState() const {
    return currentGameState;
}

/**
 * @brief Handles entering a new game state by loading resources and setting up the state. Different states need different stuff - exploration needs room textures, combat needs enemy sprites and health bars and stuff.
 * @param state The GameState being entered.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void GameManager::enterGameState(GameState state) {
    gamePlayStyles(); // make sure we have the right styles loaded
    
    // If were just coming back from pause menu, skip setup cause everything is still loaded
    // this is a small optimization so the game doesnt stutter when unpausing
    if (prevGameState == GameState::PAUSE_MENU && (state == GameState::COMBAT || state == GameState::EXPLORATION))
        return;

    switch (state) {
    case GameState::EXPLORATION: {
        // Clean up any leftover resources from before
        CleanupScreenTextures();
        CleanupScreenRects();
        
        // Initialize all the game scenes (rooms and stuff)
        if (entities && entities[0]) {
            InitGameScenes(entities[0]);
        }

        // Setup all the rectangles we need for exploration UI
        numScreenRects = 25; // we use alot of rectangles
        ScreenRects = new Rectangle[numScreenRects];
        // Pause button goes in the top-right corner
        ScreenRects[R_EXP_PAUSE_BTN] = {(float)GAME_SCREEN_WIDTH - 75 - 10, 50, 75, 75};
        // Pause menu overlay (dark transparent background)
        ScreenRects[R_EXP_PAUSE_BG_OVERLAY] = {0, 0, (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT};
        // Pause menu panel
        ScreenRects[R_EXP_PAUSE_PANEL] = {PAUSE_PANEL_X, PAUSE_PANEL_Y, PAUSE_PANEL_WIDTH, PAUSE_PANEL_HEIGHT};
        ScreenRects[R_EXP_BTN_RESUME] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_EXP_BTN_SAVE_EXIT] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_EXP_BTN_QUIT_NO_SAVE] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + 2 * (PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING), PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
    
        sceneTransitionTimer = 0.5f; // little delay before you can click again after changing rooms
        break;
    }

    case GameState::COMBAT: {
        // Combat needs alot of setup cause theres alot going on
        
        // Clean up first
        CleanupScreenTextures();
        CleanupScreenRects();
        UnloadMusicStream(backgroundMusic); // stop exploration music
        musicLoaded = false; // Reset flag so we can load new music
        
        
        // Make sure we have the stat lines loaded for creating enemies
        if (!allStatLines) {
            TraceLog(LOG_INFO, "Reloading stat lines for combat");
            allStatLines = storeAllStatLines(openStartingStatsCSV());
        }

        // Make sure we have somewhere to put our entities
        if (!entities) {
            TraceLog(LOG_INFO, "Allocating entities array for combat");
            entities = new Character*[2]{nullptr, nullptr};
        }

        // Load the combat textures (background, player sprite, enemy sprite)
        numScreenTextures = 3;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture(gameScenes[currentSceneIndex].environmentTexture.c_str()); // room background
        ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/north-west.png"); // player fighting pose
        
        // Load the right enemy texture based on which encounter this is
        switch (activeEncounterID) 
        {
            case 0:
                ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/Professor1.png"); // zombie professor
                break;
            case 1:
                ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/Sorority1.png"); // zombie sorority girl
                break;
            case 2:
            default:
                ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/FratBro1.png"); // zombie frat bro
                break;
        }
        TraceLog(LOG_INFO, "Combat screen textures loaded.");

        // Setup ALL the combat UI rectangles (theres alot)
        numScreenRects = 25;
        ScreenRects = new Rectangle[numScreenRects];
        ScreenRects[R_PLAYER_NAME] = {0, 0, 450, 50}; // player name bar at top left
        ScreenRects[R_ENEMY_NAME] = {(float)GAME_SCREEN_WIDTH - 450, 0, 450, 50}; // enemy name bar at top right
        ScreenRects[R_PLAYER_PANEL] = {0, 50, 450, 832}; // big panel for player info
        ScreenRects[R_ENEMY_PANEL] = {(float)GAME_SCREEN_WIDTH - 450, 50, 450, 832}; // big panel for enemy info
        ScreenRects[R_PLAYER_HP_BG] = {20, 150, 410, 30}; // health bar background
        ScreenRects[R_PLAYER_HP_FG] = {20, 150, 410, 30}; // health bar foreground (shrinks when hurt)
        ScreenRects[R_ENEMY_HP_BG] = {(float)GAME_SCREEN_WIDTH - 430, 150, 410, 30};
        ScreenRects[R_ENEMY_HP_FG] = {(float)GAME_SCREEN_WIDTH - 430, 150, 410, 30};
        ScreenRects[R_PLAYER_STATUS] = {20, 250, 410, 500}; // status effects panel
        ScreenRects[R_ENEMY_STATUS] = {(float)GAME_SCREEN_WIDTH - 430, 250, 410, 500};
        ScreenRects[R_BOTTOM_PANEL] = {0, (float)GAME_SCREEN_HEIGHT - 200, (float)GAME_SCREEN_WIDTH, 215}; // action buttons panel
        ScreenRects[R_BTN_ATTACK] = {20, (float)GAME_SCREEN_HEIGHT - 180, 400, 80}; // ATTACK button
        ScreenRects[R_BTN_DEFEND] = {20, (float)GAME_SCREEN_HEIGHT - 80, 400, 80}; // DEFEND button
        ScreenRects[R_BTN_USE_ITEM] = {570, (float)GAME_SCREEN_HEIGHT - 180, 400, 80}; // USE ITEM button
        ScreenRects[R_LOG_BOX] = {(float)GAME_SCREEN_WIDTH - 800, (float)GAME_SCREEN_HEIGHT - 180, 780, 175}; // combat log
        // popup menus start empty, we set them when needed
        ScreenRects[R_ATTACK_MENU] = {0};
        ScreenRects[R_MELEE_BTN] = {0};
        ScreenRects[R_RANGED_BTN] = {0};
        ScreenRects[R_ITEM_MENU] = {0};
        // pause button
        ScreenRects[R_PAUSE_BTN] = {(float)GAME_SCREEN_WIDTH - 450 - 75 - 10, 50 - 37.5f, 75, 75};
        ScreenRects[R_PAUSE_BG_OVERLAY] = {0, 0, (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT};
        ScreenRects[R_PAUSE_PANEL] = {PAUSE_PANEL_X, PAUSE_PANEL_Y, PAUSE_PANEL_WIDTH, PAUSE_PANEL_HEIGHT};
        ScreenRects[R_BTN_RESUME] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_BTN_SAVE_EXIT] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING, PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        ScreenRects[R_BTN_QUIT_NO_SAVE] = {PAUSE_BTN_X, PAUSE_PANEL_Y + 60.0f + 2 * (PAUSE_BTN_HEIGHT + PAUSE_BTN_SPACING), PAUSE_BTN_WIDTH, PAUSE_BTN_HEIGHT};
        TraceLog(LOG_INFO, "Combat screen rectangles initialized.");

        // Debug logging to help figure out whats going on
        TraceLog(LOG_INFO, "Active encounter ID: %d", activeEncounterID);
        if (!entities[0]) {
            TraceLog(LOG_ERROR, "Player entity is null in COMBAT"); // this is bad
        } else {
            TraceLog(LOG_INFO," Player: %s", entities[0]->getName().c_str());
        }
        
        // Create the enemy based on which encounter this is
        // (unless we loaded from save, then the enemy already exists)
        if (activeEncounterID == 0 && !loadedFromSave) 
        {
            // encounter 0 = professor zombie
            if (entities && entities[1]) { delete entities[1]; entities[1] = nullptr; } // delete old enemy if any
            if (allStatLines) {
                CreateCharacter(entities, allStatLines, "Zombie_Prof", "Professor");
                TraceLog(LOG_INFO, "Created enemy: Professor");
            } else {
                TraceLog(LOG_ERROR, "Cannot create enemy: allStatLines is null");
            }
        } else if (activeEncounterID == 1 && !loadedFromSave) 
        {
            // encounter 1 = sorority zombie
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
            // encounter 2 (or anything else) = frat bro zombie
            if (entities && entities[1]) { delete entities[1]; entities[1] = nullptr; }
            if (allStatLines) {
                CreateCharacter(entities, allStatLines, "Zombie_Standard", "Frat Bro");
                TraceLog(LOG_INFO, "Created enemy: Frat Bro");
            } else {
                TraceLog(LOG_ERROR, "Cannot create enemy: allStatLines is null");
            }
        } else 
        {
            // Enemy was loaded from save file so we dont need to create one
            TraceLog(LOG_INFO, "Loaded enemy from save: %s", entities[1] ? entities[1]->getName().c_str() : "NULL");
            loadedFromSave = false; // clear the flag
        }
        
        // Make sure enemy actually got created
        if (!entities || !entities[1]) {
            TraceLog(LOG_ERROR, "Enemy creation failed; aborting COMBAT setup");
            // Fall back to exploration cause we cant fight nothing
            nextGameState = GameState::EXPLORATION;
            enterGameState(currentGameState);
            break;
        }

        // Initialize the combat handler (manages turns and stuff)
        combatHandler = new CombatHandler;
        // Figure out who goes first based on initiative stat
        if (!entities[0]) {
            combatHandler->playerTurn = true; // default to player if something went wrong
            TraceLog(LOG_WARNING, "Player entity null; defaulting playerTurn=true");
        } else if (!entities[1]) {
            combatHandler->playerTurn = true;
            TraceLog(LOG_WARNING, "Enemy entity null at initiative check; defaulting playerTurn=true");
        } else {
            if (!combatHandler) {
                TraceLog(LOG_ERROR, "CombatHandler is null!"); // this shouldnt happen
            }
            // higher initiative goes first
            combatHandler->playerTurn = entities[0]->cbt.initiative >= entities[1]->cbt.initiative;
        }
        // Initialize all the combat state variables
        combatHandler->playerIsDefending = false;
        combatHandler->enemyIsDefending = false;
        combatHandler->playerHitFlashTimer = 0.0f; // for the red flash when hit
        combatHandler->enemyHitFlashTimer = 0.0f;
        combatHandler->gameOverState = false; // player dead?
        combatHandler->victoryState = false; // enemy dead?
        combatHandler->gameOverTimer = 0.0f; // delay before leaving combat
        combatHandler->showAttackMenu = false; // popup menus hidden by default
        combatHandler->showItemMenu = false;
        combatHandler->log.clear(); // clear combat log
        combatHandler->logScrollOffset = 0.0f;
        AddNewLogEntry(combatHandler->log, "A wild " + entities[1]->getName() + " appears!");
        combatHandler->enemyActionDelay = 1.0f; // enemy waits a sec before attacking (so player can see whats happening)

        // Load and start playing combat music
        if (!musicLoaded) 
        {
            backgroundMusic = LoadMusicStream("../assets/sfx/battleMusicLoop.mp3");
            backgroundMusic.looping = true;
            musicLoaded = true; // Set flag to true so we don't load it again
        }
        if (!IsMusicStreamPlaying(backgroundMusic)) 
        {
            PlayMusicStream(backgroundMusic);
        }
        break;
    }

    case GameState::PAUSE_MENU:
        // Pause menu doesnt need special setup, uses rects from previous state
        break;
    }
}

/**
 * @brief Handles exiting a game state by cleaning up resources. We dont cleanup when going to pause menu cause we need to keep everything loaded to come back.
 * @param state The GameState being exited.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void GameManager::exitGameState(GameState state) {
    switch (state) {
    case GameState::EXPLORATION:
        // Clean up exploration textures when leaving (but not if just pausing)
        if (nextGameState != GameState::PAUSE_MENU) {
            CleanupScreenTextures();
            CleanupScreenRects();
        }
        break;

    case GameState::COMBAT:
        if (nextGameState != GameState::PAUSE_MENU) {

            UnloadMusicStream(backgroundMusic); // stop combat music
            backgroundMusic = Music{LoadMusicStream("../assets/sfx/gamePlayMusic.mp3")}; // load exploration music
            PlayMusicStream(backgroundMusic);
            backgroundMusic.looping = true;

            // Clean up combat handler
            if (combatHandler) {
                delete combatHandler;
                combatHandler = nullptr;
            }
            // Delete the enemy (player survives between fights)
            if (entities[1]) {
                delete entities[1];
                entities[1] = nullptr;
            }
            CleanupScreenTextures();
            CleanupScreenRects();
        }
        break;

    case GameState::PAUSE_MENU:
        // Nothing to cleanup when leaving pause, everything is still loaded
        break;
    }
}

/**
 * @brief Renders the current game state. This is a big function cause it draws everything for exploration, combat, and pause menu. Theres alot of DrawRectangle and DrawText calls in here.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void GameManager::render() {
    switch (currentGameState) {
    case GameState::EXPLORATION: {
        // make sure we have stuff to render
        if (gameScenes.empty() || !ScreenTextures) break;

        // Draw the current room background stretched to fill screen
        DrawTexturePro(ScreenTextures[gameScenes[currentSceneIndex].textureIndex],
                      {0.0f, 0.0f, (float)ScreenTextures[gameScenes[currentSceneIndex].textureIndex].width,
                       (float)ScreenTextures[gameScenes[currentSceneIndex].textureIndex].height},
                      {0.0f, 0.0f, (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT}, {0.0f, 0.0f}, 0.0f, WHITE);

        // Draw the pause button in the corner
        DrawRectangleRec(ScreenRects[R_EXP_PAUSE_BTN], COL_BUTTON);
        DrawRectangleLinesEx(ScreenRects[R_EXP_PAUSE_BTN], 3.0f, BLACK);
        if (GuiButton(ScreenRects[R_EXP_PAUSE_BTN], "")) {
            changeGameState(GameState::PAUSE_MENU);
        }
        // Draw the pause icon (two vertical bars)
        DrawTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize),
                   {ScreenRects[R_EXP_PAUSE_BTN].x + (ScreenRects[R_EXP_PAUSE_BTN].width - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).x) / 2.0f,
                    ScreenRects[R_EXP_PAUSE_BTN].y + (ScreenRects[R_EXP_PAUSE_BTN].height - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).y) / 2.0f},
                   FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

        // Draw any items in this room that havent been picked up yet
        for (const auto &item : gameScenes[currentSceneIndex].sceneItems) {
            // only draw if: not collected yet AND (doesnt require victory OR victory achieved)
            if (!isItemCollected(item.itemName) &&
                (!item.requiresVictory || (gameScenes[currentSceneIndex].hasEncounter && battleWon[gameScenes[currentSceneIndex].encounterID]))) {
                DrawTexturePro(ScreenTextures[item.textureIndex],
                              {0, 0, (float)ScreenTextures[item.textureIndex].width, (float)ScreenTextures[item.textureIndex].height},
                              item.clickArea, {0, 0}, 0.0f, WHITE);
            }
        }

        // Draw the navigation arrows with a cool pulsing animation
        for (const auto &arrow : gameScenes[currentSceneIndex].sceneArrows) {
            // skip arrows that are disabled or need a key the player doesnt have
            if (!arrow.isEnabled || (!arrow.requiredKeyName.empty() && !isItemCollected(arrow.requiredKeyName)))
                continue;
            // calculate pulsing size (makes them bob up and down kinda)
            float scaledWidth = arrow.clickArea.width+ arrow.clickArea.width * animation::sinPulse(0.2f, PI, animation::easeInOutCubic(fmodf(GetTime(), 1.0f)));
            float scaledHeight = arrow.clickArea.height + arrow.clickArea.height * animation::sinPulse(0.2f, PI, animation::easeInOutCubic(fmodf(GetTime(), 1.0f)));
            // draw the arrow rotated based on which direction it points
            DrawTexturePro(ScreenTextures[TEX_ARROW],
                          {0.0f, 0.0f, (float)ScreenTextures[TEX_ARROW].width, (float)ScreenTextures[TEX_ARROW].height},
                          {arrow.clickArea.x + arrow.clickArea.width / 2.0f, arrow.clickArea.y + arrow.clickArea.height / 2.0f,
                           scaledWidth, scaledHeight},
                          {scaledWidth/2.0f, scaledHeight/2.0f}, // rotate around center
                          ARROW_ROTATION(arrow.dir), WHITE);
        }

        // Draw the minimap in the corner so players dont get lost
        DrawRectangleLinesEx({MINIMAP_X, MINIMAP_Y, MINIMAP_SIZE, MINIMAP_SIZE}, MINIMAP_BORDER, BLACK);
        DrawTexturePro(ScreenTextures[TEX_MINIMAP],
                      {0, 0, (float)ScreenTextures[TEX_MINIMAP].width, (float)ScreenTextures[TEX_MINIMAP].height},
                      {MINIMAP_X, MINIMAP_Y, MINIMAP_SIZE, MINIMAP_SIZE}, {0, 0}, 0.0f, WHITE);

        // Draw the player position on the minimap (its a turtle icon)
        DrawTexturePro(ScreenTextures[TEX_TURTLE],
                      {0, 0, (float)ScreenTextures[TEX_TURTLE].width, (float)ScreenTextures[TEX_TURTLE].height},
                      {MINIMAP_X + gameScenes[currentSceneIndex].minimapCoords.x * MINIMAP_SIZE - 16,
                       MINIMAP_Y + gameScenes[currentSceneIndex].minimapCoords.y * MINIMAP_SIZE - 16, 32, 32},
                      {16, 16}, gameScenes[currentSceneIndex].minimapRotation, WHITE);

        // Draw the room name above the minimap
        DrawText(gameScenes[currentSceneIndex].sceneName.c_str(), MINIMAP_X, MINIMAP_Y - 30, 30, WHITE);
        DrawRectangleLinesEx({MINIMAP_X, MINIMAP_Y, MINIMAP_SIZE, MINIMAP_SIZE}, MINIMAP_BORDER, BLACK);
        
        // Black bar at top for info text
        DrawRectangle(0, 0, (float)GAME_SCREEN_WIDTH, 40, BLACK);

        // Figure out what info text to show based on what the mouse is over
        std::string infoText;
        // check if hovering over an item
        for (const auto &item : gameScenes[currentSceneIndex].sceneItems) {
            if (!isItemCollected(item.itemName) &&
                (!item.requiresVictory || (gameScenes[currentSceneIndex].hasEncounter && battleWon[gameScenes[currentSceneIndex].encounterID])) &&
                CheckCollisionPointRec(GetMousePosition(), item.clickArea)) {
                infoText = item.hoverText;
                break;
            }
        }

        // if not hovering an item, check arrows
        if (infoText.empty()) {
            for (const auto &arrow : gameScenes[currentSceneIndex].sceneArrows) {
                if (arrow.isEnabled && (arrow.requiredKeyName.empty() || isItemCollected(arrow.requiredKeyName)) &&
                    CheckCollisionPointRec(GetMousePosition(), arrow.clickArea)) {
                    infoText = arrow.hoverText;
                    break;
                }
            }
        }

        // figure out whats visible in the room for default text
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

        // set default text if not hovering anything specific
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

        // actually draw the info text
        DrawText(infoText.c_str(), 20,ScreenRects[R_LOG_BOX].y + 5, 30, WHITE);
        break;
    }

    case GameState::COMBAT: {
        // safety checks cause we need alot of stuff for combat
        if (!combatHandler || !entities[0] || !entities[1] || !ScreenTextures || !ScreenRects ||!nerdFont) break;

        // Draw the room as combat background
        DrawTexture(ScreenTextures[0], gameScenes[currentSceneIndex].combatBgX, gameScenes[currentSceneIndex].combatBgY, WHITE);

        // Draw the player sprite (flashes red when taking damage)
        DrawTexturePro(ScreenTextures[1],
                      {0.0f, 0.0f, (float)ScreenTextures[1].width, (float)ScreenTextures[1].height},
                      {gameScenes[currentSceneIndex].playerCharX, gameScenes[currentSceneIndex].playerCharY, gameScenes[currentSceneIndex].playerScale.x, gameScenes[currentSceneIndex].playerScale.y}, {0.0f, 0.0f}, 0.0f,
                      combatHandler->playerHitFlashTimer > 0.0f ? RED : WHITE); // red tint when hit
        
        // Draw the enemy sprite (also flashes red when hurt)
        DrawTexturePro(ScreenTextures[2],
                     
                      {0.0f, 0.0f, (float)ScreenTextures[2].width, (float)ScreenTextures[2].height},
                      {gameScenes[currentSceneIndex].enemyCharX, gameScenes[currentSceneIndex].enemyCharY, gameScenes[currentSceneIndex].enemyScale.x, gameScenes[currentSceneIndex].enemyScale.y}, {0.0f, 0.0f}, 0.0f,
                      combatHandler->enemyHitFlashTimer > 0.0f ? RED : WHITE);

        // Draw all the UI panels
        DrawRectangleRec(ScreenRects[R_PLAYER_NAME], COL_NAME_BAR);
        DrawRectangleRec(ScreenRects[R_ENEMY_NAME], COL_NAME_BAR);
        DrawRectangleRec(ScreenRects[R_BOTTOM_PANEL], COL_BOTTOM_PANEL);
        DrawRectangleRec(ScreenRects[R_PLAYER_PANEL], COL_STATUS_PANEL);
        DrawRectangleRec(ScreenRects[R_ENEMY_PANEL], COL_STATUS_PANEL);
        DrawRectangleRec(ScreenRects[R_PLAYER_HP_BG], COL_HP_BG); // health bar backgrounds (red)
        DrawRectangleRec(ScreenRects[R_PLAYER_HP_FG], COL_HP_FG); // health bar foregrounds (green)
        DrawRectangleRec(ScreenRects[R_ENEMY_HP_BG], COL_HP_BG);
        DrawRectangleRec(ScreenRects[R_ENEMY_HP_FG], COL_HP_FG);
        DrawRectangleRec(ScreenRects[R_PLAYER_STATUS], COL_STATUS_INNER);
        DrawRectangleRec(ScreenRects[R_ENEMY_STATUS], COL_STATUS_INNER);
        DrawRectangleRec(ScreenRects[R_LOG_BOX], COL_LOG_BOX);
        DrawRectangleRec(ScreenRects[R_BTN_ATTACK], COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_BTN_DEFEND], COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_BTN_USE_ITEM], COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_PAUSE_BTN], COL_BUTTON);

        // Pause button
        if (GuiButton(ScreenRects[R_PAUSE_BTN], "")) {
            gameManager->changeGameState(GameState::PAUSE_MENU);
        }
        
        // error check for pause button (shouldnt happen but safety check)
        if (ScreenRects[R_PAUSE_BTN].height == 0 || ScreenRects[R_PAUSE_BTN].width == 0)
        {
            TraceLog(LOG_ERROR, "ScreenRects[R_PAUSE_BTN] is null!");
        }
        // draw pause icon
        DrawTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize),
                   {ScreenRects[R_PAUSE_BTN].x + (ScreenRects[R_PAUSE_BTN].width - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).x) / 2.0f,
                    ScreenRects[R_PAUSE_BTN].y + (ScreenRects[R_PAUSE_BTN].height - MeasureTextEx(*nerdFont, CodepointToUTF8(ICON_PAUSE, &byteSize), FONT_SIZE_BTN + 20, 1.0f).y) / 2.0f},
                   FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

        // Draw borders around all the panels (but not the health bar foregrounds cause they look weird with borders)
        for (int i = 0; i < 15; ++i)
            if (i != R_PLAYER_HP_FG && i != R_ENEMY_HP_FG)
                DrawRectangleLinesEx(ScreenRects[i], 3.0f, BLACK);

        // Draw character names
        DrawText(("Player: " + entities[0]->getName()).c_str(),
                (int)(ScreenRects[R_PLAYER_NAME].x + 20), (int)(ScreenRects[R_PLAYER_NAME].y + 10), FONT_SIZE_NAME, WHITE);
        DrawText(("Enemy: " + entities[1]->getName()).c_str(),
                (int)(ScreenRects[R_ENEMY_NAME].x + 20), (int)(ScreenRects[R_ENEMY_NAME].y + 10), FONT_SIZE_NAME, WHITE);
        
        // Draw health values (current / max)
        DrawText(TextFormat("HP: %d / %d", entities[0]->vit.health, entities[0]->vit.maxHealth),
                (int)(ScreenRects[R_PLAYER_PANEL].x + 30), (int)(ScreenRects[R_PLAYER_PANEL].y + 130), FONT_SIZE_HP, WHITE);
        DrawText(TextFormat("HP: %d / %d", entities[1]->vit.health, entities[1]->vit.maxHealth),
                (int)(ScreenRects[R_ENEMY_PANEL].x + 30), (int)(ScreenRects[R_ENEMY_PANEL].y + 130), FONT_SIZE_HP, WHITE);

        // ok this is where it gets complicated - player turn vs enemy turn
        if (combatHandler->playerTurn) {
            // PLAYER TURN - show all the action buttons
            
            // ATTACK button
            if (GuiButton(ScreenRects[R_BTN_ATTACK], "ATTACK")) {
                combatHandler->playerIsDefending = false; // stop defending if you were
                entities[0]->endDefense();
                combatHandler->showAttackMenu = !combatHandler->showAttackMenu; // toggle the attack submenu
                combatHandler->showItemMenu = false; // close item menu if open
                AddNewLogEntry(combatHandler->log, combatHandler->showAttackMenu ? "Choose your attack." : "Attack cancelled.");
                combatHandler->logScrollOffset = 1000.0f; // scroll to bottom of log
            }

            // attack submenu popup (melee vs ranged)
            if (combatHandler->showAttackMenu) {
                // position the menu next to the attack button
                ScreenRects[R_ATTACK_MENU] = {ScreenRects[R_BTN_ATTACK].x + ScreenRects[R_BTN_ATTACK].width + 10,
                                              ScreenRects[R_BTN_ATTACK].y - 150, 300.0f, 140.0f};
                ScreenRects[R_MELEE_BTN] = {ScreenRects[R_ATTACK_MENU].x + 10, ScreenRects[R_ATTACK_MENU].y + 10,
                                           ScreenRects[R_ATTACK_MENU].width - 20, 50.0f};
                ScreenRects[R_RANGED_BTN] = {ScreenRects[R_ATTACK_MENU].x + 10, ScreenRects[R_ATTACK_MENU].y + 75,
                                            ScreenRects[R_ATTACK_MENU].width - 20, 50.0f};

                DrawRectangleRec(ScreenRects[R_ATTACK_MENU], COL_BOTTOM_PANEL);
                DrawRectangleLinesEx(ScreenRects[R_ATTACK_MENU], 3.0f, BLACK);

                // MELEE attack option
                if (GuiButton(ScreenRects[R_MELEE_BTN], "")) {
                    combatHandler->showAttackMenu = false;
                    combatHandler->playerIsDefending = false;
                    // do the attack and check if it hit
                    combatHandler->enemyHitFlashTimer = resolve_melee(*entities[0], *entities[1], combatHandler->enemyIsDefending, combatHandler->log) ? 0.2f : 0.0f;
                    if (combatHandler->enemyHitFlashTimer > 0.0f) PlaySound(gameSounds[SND_HIT]); // hit sound
                    combatHandler->logScrollOffset = 1000.0f;
                    combatHandler->playerTurn = false; // end player turn
                    combatHandler->enemyActionDelay = 0.6f; // enemy will act after short delay
                }

                // draw melee button label and sword icon
                DrawText("MELEE", (int)(ScreenRects[R_MELEE_BTN].x + 20), (int)(ScreenRects[R_MELEE_BTN].y + 10),
                        FONT_SIZE_BTN, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));
                DrawTextEx(*nerdFont, CodepointToUTF8(ICON_SWORD, &byteSize),
                          {ScreenRects[R_MELEE_BTN].x + ScreenRects[R_MELEE_BTN].width - 50, ScreenRects[R_MELEE_BTN].y + 2},
                          FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

                // RANGED attack option
                if (GuiButton(ScreenRects[R_RANGED_BTN], "")) {
                    combatHandler->showAttackMenu = false;
                    combatHandler->playerIsDefending = false;
                    combatHandler->enemyHitFlashTimer = resolve_ranged(*entities[0], *entities[1], combatHandler->enemyIsDefending, combatHandler->log) ? 0.2f : 0.0f;
                    combatHandler->logScrollOffset = 1000.0f;
                    combatHandler->playerTurn = false;
                    combatHandler->enemyActionDelay = 0.6f;
                }

                // draw ranged button label and bow icon
                DrawText("RANGED", (int)(ScreenRects[R_RANGED_BTN].x + 20), (int)(ScreenRects[R_RANGED_BTN].y + 10),
                        FONT_SIZE_BTN, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));
                DrawTextEx(*nerdFont, CodepointToUTF8(ICON_BOW_ARROW, &byteSize),
                          {ScreenRects[R_RANGED_BTN].x + ScreenRects[R_RANGED_BTN].width - 50, ScreenRects[R_RANGED_BTN].y + 2},
                          FONT_SIZE_BTN + 20, 1.0f, GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));

                // check if enemy died from the attack
                if (!entities[1]->isAlive()) {
                    AddNewLogEntry(combatHandler->log, "You have defeated " + entities[1]->getName() + "!");
                    combatHandler->gameOverTimer = 2.0f; // wait 2 secs before leaving combat
                    combatHandler->victoryState = true; // we won
                    return;
                }
            } else {
                // attack menu is closed, clear the rects
                ScreenRects[R_ATTACK_MENU] = {0};
                ScreenRects[R_MELEE_BTN] = {0};
                ScreenRects[R_RANGED_BTN] = {0};
            }

            // DEFEND button
            if (GuiButton(ScreenRects[R_BTN_DEFEND], "DEFEND")) {
                combatHandler->showAttackMenu = false;
                combatHandler->playerIsDefending = true;
                entities[0]->startDefense(); // activate defense buff
                combatHandler->showItemMenu = false;
                AddNewLogEntry(combatHandler->log, entities[0]->getName() + " is defending!");
                combatHandler->logScrollOffset = 1000.0f;
                combatHandler->playerTurn = false;
                combatHandler->enemyActionDelay = 0.6f;
            }

            // USE ITEM button
            if (GuiButton(ScreenRects[R_BTN_USE_ITEM], "USE ITEM")) {
                combatHandler->playerIsDefending = false;
                entities[0]->endDefense();
                combatHandler->showItemMenu = !combatHandler->showItemMenu; // toggle item menu
                combatHandler->showAttackMenu = false;
                // check if player has any items
                if (dynamic_cast<PlayerCharacter*>(entities[0])->inv.getItems().empty()) {
                    AddNewLogEntry(combatHandler->log, "No items in inventory.");
                    combatHandler->showItemMenu = false;
                } else if (combatHandler->showItemMenu) {
                    AddNewLogEntry(combatHandler->log, "Choose an item to use.");
                }
                combatHandler->logScrollOffset = 1000.0f;
            }

            // item menu popup
            if (combatHandler->showItemMenu) {
                auto &items = dynamic_cast<PlayerCharacter*>(entities[0])->inv.getItems();
                // size the menu based on how many items we have
                ScreenRects[R_ITEM_MENU] = {ScreenRects[R_BTN_USE_ITEM].x + ScreenRects[R_BTN_USE_ITEM].width + 10,
                                           ScreenRects[R_BTN_USE_ITEM].y - (55.0f * items.size()) - 20.0f,
                                           ScreenRects[R_BTN_USE_ITEM].width, (55.0f * items.size()) + 20.0f};

                DrawRectangleRec(ScreenRects[R_ITEM_MENU], COL_BOTTOM_PANEL);
                DrawRectangleLinesEx(ScreenRects[R_ITEM_MENU], 3.0f, BLACK);

                // draw a button for each item
                for (size_t i = 0; i < items.size(); i++) {
                    if (GuiButton({ScreenRects[R_ITEM_MENU].x + 10.0f, ScreenRects[R_ITEM_MENU].y + 10.0f + (i * 55.0f),
                                  ScreenRects[R_ITEM_MENU].width - 20.0f, 50.0f}, "")) {
                        // handle healing items
                        if (items[i].healAmount > 0) {
                            // cant heal if already full hp
                            if (entities[0]->vit.health == entities[0]->vit.maxHealth) {
                                AddNewLogEntry(combatHandler->log, entities[0]->getName() + "'s health is already full!");
                                combatHandler->logScrollOffset = 1000.0f;
                                combatHandler->showItemMenu = false;
                                continue; // skip to next item
                            }
                            // do the healing
                            int beforeHeal = entities[0]->vit.health;
                            dynamic_cast<PlayerCharacter*>(entities[0])->heal(items[i].healAmount);
                            AddNewLogEntry(combatHandler->log, entities[0]->getName() + " used " + items[i].name + " and healed " +
                                          std::to_string(entities[0]->vit.health - beforeHeal) + " HP!");
                            PlaySound(gameSounds[SND_HEAL]); // healing sound
                            dynamic_cast<PlayerCharacter*>(entities[0])->inv.removeitem(items[i].name, 1); // use up the item
                            combatHandler->logScrollOffset = 1000.0f;
                            combatHandler->playerTurn = false;
                            combatHandler->enemyActionDelay = 0.6f;
                            combatHandler->showItemMenu = false;
                        }
                    }

                    // format the item label (NAME (xQUANTITY))
                    std::string itemLabel = items[i].name;
                    for (char &c : itemLabel) c = toupper(c); // make it uppercase
                    itemLabel += " (x" + std::to_string(items[i].quantity) + ")";
                    DrawText(itemLabel.c_str(), (int)(ScreenRects[R_ITEM_MENU].x + 20 + (i * 0)),
                            (int)(ScreenRects[R_ITEM_MENU].y + 20 + (i * 55.0f)), FONT_SIZE_BTN,
                            GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL)));
                }
            }
        } else {
            // ENEMY TURN - disable all the buttons so player cant do anything
            int prevState = GuiGetState();
            GuiDisable(); // gray out all buttons
            GuiButton(ScreenRects[R_BTN_ATTACK], "ATTACK");
            GuiButton(ScreenRects[R_BTN_DEFEND], "DEFEND");
            GuiButton(ScreenRects[R_BTN_USE_ITEM], "USE ITEM");
            GuiSetState(prevState);
        }

        // Draw the combat log (shows what happened in the fight)
        // use scissor mode so text doesnt draw outside the box
        BeginScissorMode((int)ScreenRects[R_LOG_BOX].x + 1, (int)ScreenRects[R_LOG_BOX].y + 1, (int)ScreenRects[R_LOG_BOX].width - 2, (int)ScreenRects[R_LOG_BOX].height - 2);
        float logY = ScreenRects[R_LOG_BOX].y + 5.0f - combatHandler->logScrollOffset;

        // draw each log entry
        for (size_t i = 0; i < combatHandler->log.size(); ++i)
        {
            if (i != combatHandler->log.size() - 1)
            {
                // old entries are gray
                DrawText(TextFormat(". %s", combatHandler->log[i].c_str()), (int)(ScreenRects[R_LOG_BOX].x + 10), (int)logY, FONT_SIZE_LOG, GRAY);
            }
            else
            {
                // newest entry is black (more visible)
                DrawText(TextFormat("> %s", combatHandler->log[i].c_str()), (int)(ScreenRects[R_LOG_BOX].x + 10), (int)logY, FONT_SIZE_LOG, BLACK);
            }
            logY += LOG_LINE_HEIGHT;
        }

        EndScissorMode();
        
        // Draw status effects for both characters
        DrawStatusPanel(ScreenRects[R_PLAYER_STATUS], entities[0]->statEff, *nerdFont);
        DrawStatusPanel(ScreenRects[R_ENEMY_STATUS], entities[1]->statEff,*nerdFont);
        break;
    }

    case GameState::PAUSE_MENU: {
        // for pause menu, draw the previous state behind it (but grayed out)
        GameState tempState = currentGameState;
        currentGameState = prevGameState;
        GuiDisable(); // disable interactions with background
        render(); // recursive call to draw whatever we paused from
        GuiEnable();
        currentGameState = tempState;

        // draw the dark overlay so you can tell its paused
        DrawRectangleRec(ScreenRects[R_PAUSE_BG_OVERLAY], Fade(BLACK, 0.7f));
        // draw the pause panel
        DrawRectangleRec(ScreenRects[R_PAUSE_PANEL], COL_BOTTOM_PANEL);
        DrawRectangleLinesEx(ScreenRects[R_PAUSE_PANEL], 3.0f, BLACK);
        DrawText("PAUSED", CENTERED_X(MeasureText("PAUSED", 40)), (int)(ScreenRects[R_PAUSE_PANEL].y + 10), 40, WHITE);

        // Resume button - goes back to game
        DrawRectangleRec(ScreenRects[R_BTN_RESUME], COL_BUTTON);
        if (GuiButton(ScreenRects[R_BTN_RESUME], "Resume"))
            currentGameState = prevGameState; // unpause

        // Save & Exit button - saves and goes to main menu
        DrawRectangleRec(ScreenRects[R_BTN_SAVE_EXIT], COL_BUTTON);
        if (GuiButton(ScreenRects[R_BTN_SAVE_EXIT], "Save & Exit")) {
            savedSucessfully = saveProgress(entities, currentSceneIndex, activeEncounterID, savedPlayerSceneIndex, battleWon, collectedItems);
            backToMainMenu = true;
        }

        // Exit without saving button
        DrawRectangleRec(ScreenRects[R_BTN_QUIT_NO_SAVE], COL_BUTTON);
        if (GuiButton(ScreenRects[R_BTN_QUIT_NO_SAVE], "Exit (No Save)")) {
            backToMainMenu = true;
        }
        break;
    }
    }
}

/**
 * @brief Updates the current game state logic based on delta time. Handles exploration navigation (clicking arrows and items), combat turn system (attacks, defense, items), and pause menu logic. Delta time makes sure everything runs at the same speed regardless of frame rate.
 * @param dt Delta time (time since last frame) in seconds. Should be like 0.016 for 60fps.
 * @return void
 * @version 1.0
 * @author Edwin Baiden
 */
void GameManager::update(float dt) {
    // Calculate where the mouse is in game coordinates
    float scale = std::min((float)GetScreenWidth() / GAME_SCREEN_WIDTH, (float)GetScreenHeight() / GAME_SCREEN_HEIGHT);
    Vector2 virtualMouse = {
        ((GetMousePosition().x - (((float)GetScreenWidth() - ((float)GAME_SCREEN_WIDTH * scale)) * 0.5f)) / scale),
        ((GetMousePosition().y - (((float)GetScreenHeight() - ((float)GAME_SCREEN_HEIGHT * scale)) * 0.5f)) / scale)
    };

    switch (currentGameState) {
    case GameState::EXPLORATION: {
        if (gameScenes.empty()) break;

        // scene transition timer prevents instant clicks after changing rooms
        if (sceneTransitionTimer > 0.0f) {
            sceneTransitionTimer -= dt;
            if (sceneTransitionTimer < 0.0f) sceneTransitionTimer = 0.0f;
        }

        // handle mouse clicks for navigation and item pickup
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // first check if player clicked on an item
            for (auto &item : gameScenes[currentSceneIndex].sceneItems) {
                if (!isItemCollected(item.itemName) &&
                    (!item.requiresVictory || (gameScenes[currentSceneIndex].hasEncounter && battleWon[gameScenes[currentSceneIndex].encounterID])) &&
                    CheckCollisionPointRec(virtualMouse, item.clickArea)) {
                    // picked up the item, add to collected list
                    collectedItems.push_back(item.itemName);

                    // handle different items differently
                    if (item.itemName == "Health Potion") 
                    {
                            // add potion to inventory
                            HealthPotion hpotion;
                            dynamic_cast<PlayerCharacter*>(entities[0])->inv.additem(hpotion);
                    }

                    if (item.itemName == "Baseball Bat") 
                    {
                        // baseball bat boosts your weapon stats
                        // gotta check which character type cause they have different weapon structs
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

                    // keys just set flags on the player
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

            // then check if player clicked on a navigation arrow
            for (const auto &arrow : gameScenes[currentSceneIndex].sceneArrows) {
                // skip disabled arrows and locked arrows
                if (!arrow.isEnabled || (!arrow.requiredKeyName.empty() && !isItemCollected(arrow.requiredKeyName)))
                    continue;

                if (CheckCollisionPointRec(virtualMouse, arrow.clickArea)) {
                    currentSceneIndex = arrow.targetSceneIndex; // go to new room
                    sceneTransitionTimer = 0.25f; // short delay before can click again

                    // if new room has an undefeated enemy, start combat
                    if (gameScenes[currentSceneIndex].hasEncounter && !battleWon[gameScenes[currentSceneIndex].encounterID]) {
                        savedPlayerSceneIndex = currentSceneIndex; // remember where we are for saves
                        activeEncounterID = gameScenes[currentSceneIndex].encounterID;
                        changeGameState(GameState::COMBAT); // fight
                    }
                    break; // only process one arrow click
                }
            }
        }
        break;
    }

    case GameState::COMBAT: {
        if (!combatHandler || !entities[0] || !entities[1]) break;

        // Update health bar widths based on current health (so they shrink as you take damage)
        ScreenRects[R_PLAYER_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_PLAYER_HP_BG], entities[0]->vit.health, entities[0]->vit.maxHealth);
        ScreenRects[R_ENEMY_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_ENEMY_HP_BG], entities[1]->vit.health, entities[1]->vit.maxHealth);
        
        // count down hit flash timers (for the red tint effect)
        combatHandler->playerHitFlashTimer = std::max(0.0f, combatHandler->playerHitFlashTimer - dt);
        combatHandler->enemyHitFlashTimer = std::max(0.0f, combatHandler->enemyHitFlashTimer - dt);

        // handle scrolling the combat log with mouse wheel
        if (CheckCollisionPointRec(virtualMouse, ScreenRects[R_LOG_BOX])) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f)
                combatHandler->logScrollOffset += wheel * -25.0f; // scroll up or down
        }

        // keep log scroll in bounds
        if (combatHandler->logScrollOffset < 0.0f) combatHandler->logScrollOffset = 0.0f;
        if (combatHandler->logScrollOffset > std::max(0.0f, (float)(combatHandler->log.size() * LOG_LINE_HEIGHT) - (ScreenRects[R_LOG_BOX].height - 10.0f))) 
            combatHandler->logScrollOffset = std::max(0.0f, (float)(combatHandler->log.size() * LOG_LINE_HEIGHT) - (ScreenRects[R_LOG_BOX].height - 10.0f));

        // handle game over or victory (waiting before leaving combat)
        if (combatHandler->gameOverState || combatHandler->victoryState) {
            combatHandler->gameOverTimer -= dt;
            if (combatHandler->gameOverTimer <= 0.0f) {
                PlaySound(gameSounds[SND_ZOM_DEATH]); // death sound
                // if we won, mark the encounter as defeated
                if (combatHandler->victoryState && activeEncounterID >= 0) {
                    battleWon[activeEncounterID] = true;
                    // set the defeat flag on player based on which zombie we beat
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
                    activeEncounterID = -1; // no more active encounter
                }
                changeGameState(GameState::EXPLORATION); // back to exploring
            }
            break; // dont do other combat stuff while waiting
        }

        // Enemy's turn logic
        if (!combatHandler->playerTurn) {
            combatHandler->enemyActionDelay -= dt; // count down delay
            if (combatHandler->enemyActionDelay <= 0.0f) {
                // its enemys turn to do something
                
                // end enemy defense if they were defending last turn
                if (combatHandler->enemyIsDefending)
                    entities[1]->endDefense();
                combatHandler->enemyIsDefending = false;

                // AI decides what to do
                Action enemyAction = ai_choose(*(dynamic_cast<NonPlayerCharacter*>(entities[1])), *(dynamic_cast<PlayerCharacter*>(entities[0])));

                if (enemyAction.type == ActionType::Attack) {
                    // enemy attacks
                    combatHandler->playerHitFlashTimer = resolve_melee(*entities[1], *entities[0], combatHandler->playerIsDefending, combatHandler->log) ? 0.2f : 0.0f;
                    if (combatHandler->playerHitFlashTimer > 0.0f) PlaySound(gameSounds[SND_HIT]);
                    combatHandler->logScrollOffset = 1000.0f;
                    // check if player died
                    if (!entities[0]->isAlive()) {
                        AddNewLogEntry(combatHandler->log, "You died.");
                        combatHandler->gameOverTimer = 2.0f;
                        combatHandler->gameOverState = true;
                        return;
                    }
                } else if (enemyAction.type == ActionType::Defend) {
                    // enemy defends
                    combatHandler->enemyIsDefending = true;
                    entities[1]->startDefense();
                    AddNewLogEntry(combatHandler->log, entities[1]->getName() + " is defending!");
                    combatHandler->logScrollOffset = 1000.0f;
                }
                combatHandler->playerTurn = true; // back to player turn
            }
        }
        break;
    }

    case GameState::PAUSE_MENU:
        // pause menu doesnt need update logic, its just waiting for button clicks
        break;
    }
}

