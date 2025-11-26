/* =================================== screenManager.cpp ====================================
    Project: TTRPG Game ??? Idk what this game is anymore lol
    Subsystem: Screen Manager
    Primary Author: Edwin Baiden
    Description: This file implements the ScreenManager class and GameManager class and their functions from screenManager.h  to manage
                 different game screens using raylib and raygui for rendering and GUI
                 elements.

                 Screen States:
                    - MAIN_MENU: The main menu screen where players can start or exit the game.

                    - CHARACTER_SELECT: The character selection screen where players choose
                    their character. You can only select student for now.

                    - GAMEPLAY: The main gameplay screen (not fully implemented here, however
                    combat is being worked on). Values from this screen will be used to
                    determine player actions in combat.

                    - SAVE_QUIT: The save and quit screen (not fully implemented here).

                Each screen has its own styles(managed byfunctuons) and GUI resources
                (textures and rectangles, which are dynamically allocated and deallocated
                when entering and exiting screens).

                Game States:
                    - EXPLORATION: Exploration mode (not implemented here).

                    - COMBAT: Combat mode where players and enemies take turns attacking
                    each other.

                    - DIALOGUE: Dialogue mode for conversations (not implemented here).

                    - PAUSE_MENU: Pause menu mode (not implemented here).

                Dynamically allocated resources(more on these in enterScreen and exitScreen):
                    - Textures (ScreenTextures + numScreenTextures): Array of textures used
                    in the current screen.

                    - Rectangles (ScreenRects + numScreenRects): Array of rectangles defining GUI
                    element positions and sizes.

                    - Character Cards (characterCards): Array of character card structures used
                    in the character selection screen.

                    - Character Selection State (CharSelectionStuff): Array of integers used to
                    manage character selection state in the character selection screen.

                    - All Stat Lines (allStatLines): String stream holding all character stats
                    from CSV file(function from characters.h/characters.cpp).

                    - Game Manager (gameManager): Pointer to the GameManager instance managing game states (Defined in screenManager.h).

                    - Characters (Steve, Chad): Pointers to the player character and NPC instances (Classes are in characters.h).

                Screen Styles:
                    - defaultStyles(): Sets default GUI styles for buttons and text.
                    - startMenuStyles(): Sets GUI styles for the main menu screen.
                    - playerSelectStyles(): Sets GUI styles for the character selection screen.
                    - gamePlayStyles(): Not done yet, will set GUI styles for the gameplay screen.
                    - pauseMenuStyles(): Not done yet, will set GUI styles for the pause menu screen.

                Screen Manager Functions: uses a state machine approach
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
                      resources and setting styles. (This is where most of the dynamic memory allocation happens).

                    - void ScreenManager::exitScreen(ScreenState screen): Handle exiting a screen by unloading
                      resources and cleaning up. (This is where most of the dynamic memory deallocation happens).

                Game Manager Functions: not fully implemented yet (only COMBAT state is somewhat test functional) uses a state machine approach
                    - GameManager::GameManager(GameState initial): Constructor to initialize the
                      GameManager with an initial game state.

                    - GameManager::~GameManager(): Destructor to clean up remainingresources when the
                      GameManager is destroyed.

                    - void GameManager::changeGameState(GameState newState): Request a game state change.

                    - GameState GameManager::getCurrentGameState() const: Get the current game state.

                    - void GameManager::update(float dt): Update the current game state with delta time.

                    - void GameManager::render(): Render the current game state.

                    - void GameManager::enterGameState(GameState state): Handle entering a new game
                      state by loading resources.

                    - void GameManager::exitGameState(GameState state): Handle exiting a game state
                      by unloading resources.

                Macros for constants(used to save runtime memory):
                    - MAX_CHAR_CARDS: Number of character cards shown on the CHARACTER_SELECT
                    screen.

                    - MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT: Size of main menu buttons ("Start Game",
                    "Exit Game").

                    - MAIN_BUTTON_OFFSET_Y, MAIN_BUTTON_SPACING:  Vertical offset of the first main
                    menu button from screen center, and vertical spacing between main menu
                    buttons.

                    - CHARACTER_CARD_WIDTH, CHARACTER_CARD_HEIGHT: Size of each character selection card.

                    - CHARACTER_CARD_SPACING: Horizontal spacing between character cards in the default
                    row.

                    - CHARACTER_DOCK_SPACING:  Vertical spacing between docked (i.e nonselected) character
                    cards when one card is centered.

                    PLAY_BTN_WIDTH, PLAY_BTN_HEIGHT: Size of the "Play Game" button on
                    the CHARACTER_SELECT screen.

                    PLAY_BTN_OFFSET_Y: Vertical offset of the "Play Game" button from the character card
                    its under.

                    - R_<NAME>: Indices for rectangles in the combat UI (e.g., R_PLAYER_NAME, R_ENEMY_NAME...) Used to improve readability
                    when accessing ScreenRects array.

*/


#define RAYGUI_IMPLEMENTATION // Ensures raygui implementation is included here (needs to be defined in one cpp file)
#include "screenManager.h"
#include <algorithm> // For std::min, std::max, std::clamp

//================= MACROS TO IMPROVE READABILITY WHILE SAVING RUNTIME MEMORY ===================
// UPDATED: Lock to virtual resolution
#define SCREEN_WIDTH (float)GAME_SCREEN_WIDTH
#define SCREEN_HEIGHT (float)GAME_SCREEN_HEIGHT

// Character select
#define MAX_CHAR_CARDS 4 // Maximum number of character cards shown (for now, we only have 4 characters)
#define MAIN_BUTTON_WIDTH 600.0f // Width of main menu buttons
#define MAIN_BUTTON_HEIGHT 70.0f // Height of main menu buttons
#define MAIN_BUTTON_OFFSET_Y 100.0f // Vertical offset of the first main menu button from screen center
#define MAIN_BUTTON_SPACING 100.0f // Vertical spacing between main menu buttons

#define CHARACTER_CARD_WIDTH 300.0f // Width of each character selection card
#define CHARACTER_CARD_HEIGHT 400.0f // Height of each character selection card
#define CHARACTER_CARD_SPACING 50.0f // Horizontal spacing between character cards in the default row
#define CHARACTER_DOCK_SPACING 90.0f // Vertical spacing between docked (i.e nonselected) character cards when one card is centered

#define PLAY_BTN_WIDTH 400.0f // Width of the "Play Game" button on the CHARACTER_SELECT screen
#define PLAY_BTN_HEIGHT 60.0f // Height of the "Play Game" button on the CHARACTER_SELECT screen
#define PLAY_BTN_OFFSET_Y 36.0f // Vertical offset of the "Play Game" button from the character card its under

// Intro crawl
#define INTRO_CRAWL_SPEED 30.0f // Speed of the intro crawl text (pixels per second)
#define INTRO_CRAWL_START_Y (float)GAME_SCREEN_HEIGHT // Starting Y position of the intro crawl text (just off the bottom of the screen)
#define INTRO_CRAWL_END_Y -1400 // Ending Y position of the intro crawl text (just off the top of the screen)
#define INTRO_CRAWL_FONT_SIZE 28// Font size for the intro crawl text
#define INTRO_CRAWL_LINE_HEIGHT 34 // Line spacing for the intro crawl text

// Combat UI rectangle indices will be used in conjunction with ScreenRects array to improve readability
#define R_PLAYER_NAME 0 // Index for player name rectangle
#define R_ENEMY_NAME 1 // Index for enemy name rectangle
#define R_PLAYER_PANEL 2 // Index for player panel rectangle
#define R_ENEMY_PANEL 3 // Index for enemy panel rectangle
#define R_PLAYER_HP_BG 4 // Index for player HP background rectangle
#define R_PLAYER_HP_FG 5 // Index for player HP foreground rectangle
#define R_ENEMY_HP_BG 6 // Index for enemy HP background rectangle
#define R_ENEMY_HP_FG 7 // Index for enemy HP foreground rectangle
#define R_PLAYER_STATUS 8 // Index for player status rectangle
#define R_ENEMY_STATUS 9 // Index for enemy status rectangle
#define R_BOTTOM_PANEL 10 // Index for bottom panel rectangle
#define R_BTN_ATTACK 11 // Index for attack button rectangle
#define R_BTN_DEFEND 12 // Index for defend button rectangle
#define R_BTN_USE_ITEM 13 // Index for use item button rectangle
#define R_LOG_BOX 14 // Index for log box rectangle
#define R_ATTACK_MENU 15 // Index for attack menu rectangle
#define R_MELEE_BTN 16 // Index for melee attack button rectangle
#define R_RANGED_BTN 17 // Index for ranged attack button rectangle

// Text sizes
#define FONT_SIZE_NAME 30 // Font size for names
#define FONT_SIZE_HP 20 // Font size for health points
#define FONT_SIZE_BTN 30 // Font size for buttons
#define FONT_SIZE_LOG 20 // Font size for log
#define LOG_LINE_HEIGHT 24 // Line height for log text

// Centered text helpers (for readability while saving runtime memory)
// used this as reference: https://stackoverflow.com/questions/163365/how-do-i-make-a-c-macro-behave-like-a-function
#define CENTER_TEXT_X(rect, txt, size) \
    (int)((rect).x + (rect).width / 2.0f - MeasureText((txt), (size)) / 2.0f)

#define CENTER_TEXT_Y(rect, size) \
    (int)((rect).y + (rect).height / 2.0f - (size) / 2.0f)

// Health bar width helper (for readability while saving runtime memory)
#define HEALTH_BAR_WIDTH(rectBg, cur, max) \
    ((float)(rectBg).width * ((float)(cur) / (float)(max)))



// Combat UI colors
#define COL_NAME_BAR Color{8,8,12,255} // Dark color for name bars
#define COL_BOTTOM_PANEL Color{112,120,128,255} // Gray color for bottom panel
#define COL_STATUS_PANEL Color{55,61,57,220} // Dark translucent color for status panels
#define COL_STATUS_INNER Color{91,94,92,255} // Lighter color for inner status panel
#define COL_LOG_BOX Color{167,171,170,255} // Color for log box
#define COL_BUTTON Color{68,74,72,255} // Color for buttons (at least when creating them they will change after they turn into GuiButtons)
#define COL_HP_BG Color{60,15,20,255} // Color for health bar background
#define COL_HP_FG Color{190,50,60,255} // Color for health bar foreground

//======================= GLOBAL STATIC POINTERS + VARIABLES =======================
//I had an issue where the program was using too much runtime memory so i decided to make shared items across screens static pointers
// This way they are only allocated when needed and deallocated when not needed to save memory and the variables can be reused across screens and functions
//When not used the pointers are only using 8 bytes of memory each (on a 64 bit system)

std::istringstream *allStatLines = nullptr; // Holds all character stats from CSV file used in character select screen and creating characters (CreateCharacter(String))

static Texture2D *ScreenTextures = nullptr; // Array of textures used in the current screen
static int numScreenTextures = 0; // Number of textures in the current screen

static Rectangle *ScreenRects = nullptr; // Array of rectangles defining GUI element positions and sizes
static int numScreenRects = 0; // Number of rectangles in the current screen

static charCard *characterCards = nullptr; // Array of character card structures used in the character selection screen

static std::stringstream *scrollIntroCrawl = nullptr; // Holds the intro crawl text used in the intro crawl screen
static float introCrawlYPos = 0.0f; // Y position of the intro crawl text

// Only used in character selection screen to manage character selection state
// [0] = selection index (-1 = none)
// [1] = hovered index   (-1 = none)
// [2] = layoutInitFlag  (0 = false, 1 = true)
static int *CharSelectionStuff = nullptr;

static Character** entities = nullptr; // Array of character pointers (players and NPCs)

static GameManager *gameManager = nullptr; // Pointer to the GameManager instance managing game states

static Font* nerdFont; // Nerd font for UI text

//======================= GUI BUTTON AND TEXT STYLE FUNCTIONS =======================
//Setting styles for buttons and text to improve GUI appearance and user experience

//@brief: Sets the default styles for buttons and text (not yet called anywhere)
//@version: 1.0
//@author: Edwin Baiden
void defaultStyles()
{
    /* Keyword defs:
         - GuiSetStyle(controlType, property, value); From raygui.h
            - controlType: The type of GUI control (e.g., BUTTON, LABEL, etc.)
            - property: The specific style property to set (e.g., BORDER_COLOR_NORMAL, TEXT_SIZE, etc.)
            - value: The value to assign to the specified property.
        -BORDER_COLOR_NORMAL: Color of the border when the button is in its normal state when not interacted with.
        -BORDER_COLOR_FOCUSED: Color of the border when the button is focused (hovered over).
        -BORDER_COLOR_PRESSED: Color of the border when the button is pressed (clicked).
        -BASE_COLOR_NORMAL: Base color of the button in its normal state.
        -BASE_COLOR_FOCUSED: Base color of the button when focused (hovered over).
        -BASE_COLOR_PRESSED: Base color of the button when pressed (clicked).
        -TEXT_COLOR_NORMAL: Color of the button text in its normal state.
        -TEXT_COLOR_FOCUSED: Color of the button text when focused (hovered over).
        -TEXT_COLOR_PRESSED: Color of the button text when pressed (clicked).
        -TEXT_SIZE: Size of the text displayed on the button.
    */

    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x828282FF); // Medium gray border for normal state
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0xB6B6B6FF); //Neutral gray border for focused state
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xDADADAFF); // Light gray border for pressed state
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xE0E0E0FF); // Light gray base for normal state
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0xC4C4C4FF); // Light-medium gray base for focused state
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0xA8A8A8FF); // Medium gray base for pressed state
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0x000000FF); // Black text for normal state
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0x000000FF); // Black text for focused state
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0x000000FF); // Black text for pressed state
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Default text size
}

//@brief: Sets the styles for buttons and text in the main menu screen
//@version: 1.0
//@author: Edwin Baiden
void startMenuStyles()
{
    //Note for Team: Review defaultStyles() for keyword definitions
    //TODO: Need to add disabled styles once we have the progress JSON file working, so we can add a "CONTIUE" button.
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x646464FF); // Dark gray border for normal state
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x969696FF); // Medium gray border for focused state
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xC8C8C8FF); // Light gray border for pressed state
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x000000B4); // Dark blue base for normal state
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x323232C8); // Dark gray base for focused state
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x646464DC); // Medium gray base for pressed state
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF); // White text for normal state
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF); // White text for focused state
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF); // White text for pressed state
    GuiSetStyle(DEFAULT, TEXT_SIZE, 56); // Large text size
}

//@brief: Sets the styles for buttons and text in the character selection screen
//@version: 1.0
//@author: Edwin Baiden

void playerSelectStyles()
{
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x006600FF); // Dark green border for normal state
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x008800FF); // Medium green border for focused state
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0x00CC00FF); // Light green border for pressed state
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000); // Transparent base for normal state
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x003300C8); // Dark blue base for focused state
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x006600DC); // Medium green base for pressed state
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF); // White text for normal state (text is not used on character cards)
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF); // White text for focused state (text is not used on character cards)
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF); // White text for pressed state (text is not used on character cards)
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36); // Medium-large text size
    GuiSetStyle(BUTTON, BORDER_WIDTH, 6); // Thicker border for better visibility

    //Disacbled styles: Used for Play Button when no character is selected
    GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED,  0x555555FF); // Dark gray border for disabled state
    GuiSetStyle(BUTTON, BASE_COLOR_DISABLED,    0x222222B4); // Dark gray base for disabled state
    GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED,    0x888888FF); // Light gray text for disabled state
}

void gamePlayStyles()
{
    nerdFont = new Font();
    int codepoints[2] = {0xF04E5, 0xF1841}; // Codepoints for Nerd Font icons we will use

    ChangeDirectory(GetApplicationDirectory()); // Ensure working directory is set to application directory (Cause MacOS)
    *nerdFont = LoadFontEx("../assets/fonts/JetBrainsMonoNLNerdFontMono-Bold.ttf", 32, codepoints, 2); // Load custom font used across screens
    SetTextureFilter(nerdFont->texture, TEXTURE_FILTER_BILINEAR); // Set texture filter for better appearance

    playerSelectStyles(); // Reuse character selection styles for gameplay for now
    
}

//=============== CHARACTER CREATION FUNCTION FOR CHARACTER SELECT SCREEN ===============

//@brief: Creates a character based on the provided ID by reading stats from the ../dat/Character_Starting_Stats.csv file (made possible by the allStatLines pointer variable)
//@param ID - The character ID used to look up stats in the CSV file (e.g., "Student", "Zombie_Standard").
//@version: 1.0
//@author: Edwin Baiden
void CreateCharacter(std::string ID, std::string name) // This function will probably move later to characters.cpp/h
{
    Attributes     CharAttrs; // Structs to hold character attributes {Strength, Dexterity, 
                              // Constitution, Wisdom, Charisma, Intelligence} (defined in characters.h)
                        
    DefenseStats   CharDef;   // Struct to hold character defense stats {Armor, Magic Resist} (defined in characters.h)
    CombatStats    CharCbt;   // Struct to hold character combat stats {Attack, Defense, Initiative} (defined in characters.h)
    VitalStats     CharVit;   // Struct to hold character vital stats {HP, Max HP} (defined in characters.h)
    StatusEffects  CharStatus; // Struct to hold character status effects (defined in characters.h)

    // Populate the structs by reading from the CSV file using the getStatForCharacterID function
    CharAttrs = {
        getStatForCharacterID(allStatLines, ID, CSVStats::STR),
        getStatForCharacterID(allStatLines, ID, CSVStats::DEX),
        getStatForCharacterID(allStatLines, ID, CSVStats::CON),
        getStatForCharacterID(allStatLines, ID, CSVStats::WIS),
        getStatForCharacterID(allStatLines, ID, CSVStats::CHA),
        getStatForCharacterID(allStatLines, ID, CSVStats::INT)
    };

    CharDef = {
        getStatForCharacterID(allStatLines, ID, CSVStats::ARMOR),
        0 // magic resist unused for now
    };

    CharCbt = {
        0, // This is just a placeholder for now (melee damage)
        0, // This is just a placeholder for now (ranged damage)
        getStatForCharacterID(allStatLines, ID, CSVStats::INITIATIVE)
    };

    CharVit = {
        getStatForCharacterID(allStatLines, ID, CSVStats::MAX_HEALTH),
        getStatForCharacterID(allStatLines, ID, CSVStats::MAX_HEALTH)
    };

    CharStatus = {}; // All status effects default to false

    // Create the character based on the ID
    // For now only student and standard zombie are implemented (Milestone requirement)
    if (ID == "Student") //If the ID is "Student", create a Student character
    {
        //(Name, Attributes, DefenseStats, CombatStats, VitalStats, StatusEffects)
        entities[0] = new Student(name, CharAttrs, CharDef, CharCbt, CharVit, CharStatus);
    }
    else if (ID == "Rat")
    {
        //(Name, Attributes, DefenseStats, CombatStats, VitalStats, StatusEffects)
        entities[0] = new Rat(name, CharAttrs, CharDef, CharCbt, CharVit, CharStatus);
    }
    else if (ID == "Professor")
    {
        entities[0] = new Professor(name, CharAttrs, CharDef, CharCbt, CharVit, CharStatus);
    }
    else if (ID == "Attila")
    {
        entities[0] = new Atilla(name, CharAttrs, CharDef, CharCbt, CharVit, CharStatus);
    }
    
    else
    {
        //(Name, Attributes, DefenseStats, CombatStats, VitalStats, StatusEffects)
        entities[1] = new Zombie(CharAttrs, CharDef, CharCbt, CharVit, CharStatus);
    }
}

void getIntroCrawlText(std::stringstream *ss, int chosenCharacterIdx)
{
    if(ss == nullptr) return; // Safety check
    ss->str(""); // Clear the stringstream content
    ss->clear(); // Clear any error flags

    ChangeDirectory(GetApplicationDirectory()); // Ensure working directory is set to application directory (Cause MacOS)
    std::ifstream file("../dat/general_Intro.txt"); // Open the intro crawl text file
    if (!file.is_open())
    {
        (*ss) << "Error: Unable to open intro crawl text file."; // Error handling if file cannot be opened
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        (*ss) << line << "\n\n"; // Read each line from the file and append it to the stringstream
    }
    file.close(); // Close the file after reading
    (*ss) << "\n\n\n"; // Add some spacing before character-specific intro

    // Append character-specific intro text based on the chosen character index
    switch (chosenCharacterIdx)
    {
        case 0: //Student
        {
            file.open("../dat/Student_Intro.txt"); // Open the student intro text file
            break;
        }
        case 1: //Rat
        {
            file.open("../dat/Rat_Intro.txt"); // Open the rat intro text file
            break;
        }
        case 2: //Professor
        {
            file.open("../dat/Professor_Intro.txt"); // Open the professor intro text file
            break;
        }
        case 3: //Attila
        {
            file.open("../dat/Attila_Intro.txt"); // Open the Attila intro text file
            break;
        }
        default:
            return; // No character-specific intro text   
    }
    if (!file.is_open())
    {
        (*ss) << "Error: Unable to open character-specific intro text file."; // Error handling if file cannot be opened
        return;
    }
    while (std::getline(file, line))
    {
        (*ss) << line << "\n\n"; // Read each line from the file and append it to the stringstream
    }
    file.close(); // Close the file after reading

}

// =================== SCREENMANAGER CLASS FUNCTION DEFINITIONS ===================

//@brief: Constructor to initialize the ScreenManager with an initial screen state
//@param initial - The initial screen state to set (default is MAIN_MENU)
//@version: 1.0
//@author: Edwin Baiden
ScreenManager::ScreenManager(ScreenState initial) //initial = ScreenState::MAIN_MENU
{
    currentScreen = initial; // Set the current screen state to the provided initial state
    scale = 1.0f;
    offset = { 0.0f, 0.0f };
}

// @brief: Destructor to clean up resources (on any screen) when the ScreenManager is destroyed in case its not already done
//@version: 1.0
//@author: Edwin Baiden
ScreenManager::~ScreenManager()
{
    UnloadRenderTexture(target); // Clean up V-Res texture
    exitScreen(currentScreen); // Ensure resources for the current screen are cleaned up when the ScreenManager is destroyed
}

//@brief: Initialize the screen manager by entering the initial screen (could have used constructor or just called enterScreen directly, but this is cleaner, i think)
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::init()
{
    // Initialize Virtual Resolution Render Texture
    target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Smooth scaling

    enterScreen(currentScreen); // Enter the initial screen to set up resources
}

//@brief: Request a screen change to a new screen state
//@param newScreen - The new screen state to change to
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::changeScreen(ScreenState newScreen)
{
    if (newScreen == currentScreen) return; // No change needed if the new screen is the same as the current screen
    exitScreen(currentScreen); // Clean up resources for the current screen
    currentScreen = newScreen; // Update the current screen state to the new screen
    enterScreen(currentScreen); // Set up resources for the new screen
}

//@brief: Get the current screen state
//@return: The current screen state
//@version: 1.0
//@author: Edwin Baiden
ScreenState ScreenManager::getCurrentScreen() const //used const to make sure it doesn't modify any member variables
{
    return currentScreen; 
}

// Helper to get mouse position in virtual coordinates
Vector2 ScreenManager::GetVirtualMousePosition()
{
    Vector2 mouse = GetMousePosition();
    Vector2 virtualMouse = { 0 };
    
    // Reverse the scaling and offset applied in render()
    virtualMouse.x = (mouse.x - offset.x) / scale;
    virtualMouse.y = (mouse.y - offset.y) / scale;

    // Clamp to game screen to avoid out of bounds
    if (virtualMouse.x < 0) virtualMouse.x = 0;
    if (virtualMouse.y < 0) virtualMouse.y = 0;
    if (virtualMouse.x > GAME_SCREEN_WIDTH) virtualMouse.x = GAME_SCREEN_WIDTH;
    if (virtualMouse.y > GAME_SCREEN_HEIGHT) virtualMouse.y = GAME_SCREEN_HEIGHT;

    return virtualMouse;
}

//@brief: Update the current screen with delta time
//@param dt - The delta time since the last update (called every frame with GetFrameTime())
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::update(float dt)
{
    // Recalculate scale/offset each frame for input calculations
    // (Also done in render, but we need it here for GetVirtualMousePosition)
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    scale = std::min(screenWidth / GAME_SCREEN_WIDTH, screenHeight / GAME_SCREEN_HEIGHT);
    offset.x = (screenWidth - (GAME_SCREEN_WIDTH * scale)) * 0.5f;
    offset.y = (screenHeight - (GAME_SCREEN_HEIGHT * scale)) * 0.5f;

    // Update logic based on the current screen 
    switch (currentScreen)
    {
    case ScreenState::MAIN_MENU: // Main menu screen
        // No dynamic elements to update in main menu for now
        break;

    case ScreenState::CHARACTER_SELECT: // Character selection screen
    {
        if (!characterCards || !CharSelectionStuff) break; // If character resources are not initialized, exit early (safety check)

        // Calculating the belend factor for easing based on delta time (exponential decay)
        // Using easeInQuad function from animation namespace (defined in screenManager.h)
        float t = animation::easeInQuad(1.0f - expf(-10.0f * dt));

        // Loop through each character card and applying the appropriate texture to it
        for (int i = 0; i < MAX_CHAR_CARDS; ++i)
        {
            characterCards[i].texture = ScreenTextures[i + 1]; // +1 to skip background texture at index 0
        }

        // CharacterSelectionStuff[2] is a flag to check if layout has been initialized (wish there was a better way to use the dynamic approach; maybe use macros to improve readability?)
        if (!CharSelectionStuff[2])
        {
            // Starting off by centering the cards horizontally with equal spacing (this x value is the x position of the first card)
            // Its basically just calculating the difference between screen width and total width of all cards + spacing, then dividing by 2 to get starting X position
            float startX = (SCREEN_WIDTH - (MAX_CHAR_CARDS * CHARACTER_CARD_WIDTH + (MAX_CHAR_CARDS - 1) * CHARACTER_CARD_SPACING)) / 2.0f;
            
            // Verical Centering
            // Basically subtracting card height from screen height and dividing by 2 to get Y position
            float targetY = (SCREEN_HEIGHT - CHARACTER_CARD_HEIGHT) / 2.0f;

            //Placing ech card down in a row with equal spacing
            for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            {
                // Calculating the target X position for each card based on its index
                float targetX = startX + i * (CHARACTER_CARD_WIDTH + CHARACTER_CARD_SPACING);
                characterCards[i].defaultRow.x = targetX;
                characterCards[i].defaultRow.y = targetY;
                characterCards[i].defaultRow.width  = CHARACTER_CARD_WIDTH; // Set width of the card defined in macro
                characterCards[i].defaultRow.height = CHARACTER_CARD_HEIGHT; // Set height of the card defined in macro

                //These values will be used for the animation slopeing/interpolation
                characterCards[i].currentAnimationPos = characterCards[i].defaultRow; // Initialize current position to default row position
                characterCards[i].targetAnimationPos  = characterCards[i].defaultRow; // Initialize target position to default row position
            }
            CharSelectionStuff[2] = 1; // Set layout initialized flag to true so we don't redo this
        }

        // CharSelectionStuff[0] is the index of the currently selected character (-1 if none selected) (Once again, wish there was a better way to do this with the dynamic approach for readability)
        // If CharSelectionStuff[0] == -1, then no character is selected
        // If CharSelectionStuff[0] == 0, the student character is selected (only one implemented for now)
        // If CharSelectionStuff[0] == 1, the rat is selected (not fully implemented for now)
        // If CharSelectionStuff[0] == 2, the professor is selected (not fully implemented for now)
        // If CharSelectionStuff[0] == 3, the atilla is selected (not fully implemented for now)
        if (CharSelectionStuff[0] == -1) 
        {
            // No character selected, so all cards go back to default row positions
            for (int i = 0; i < MAX_CHAR_CARDS; i++)
            {
                characterCards[i].targetAnimationPos = characterCards[i].defaultRow; // Reset taget position to default row
            }
        }
        else // A character is selected so we center that card and dock the others
        {
            characterCards[CharSelectionStuff[0]].targetAnimationPos.x = (SCREEN_WIDTH - CHARACTER_CARD_WIDTH) / 2.0f; // Getting centered X position (similar to how startX was calculated earlier but without spacing)
            characterCards[CharSelectionStuff[0]].targetAnimationPos.y = characterCards[CharSelectionStuff[0]].defaultRow.y; // Keep Y position the same as default row
            characterCards[CharSelectionStuff[0]].targetAnimationPos.width = CHARACTER_CARD_WIDTH; // width stays the same
            characterCards[CharSelectionStuff[0]].targetAnimationPos.height = CHARACTER_CARD_HEIGHT; // height stays the same

            float dockX = SCREEN_WIDTH - CHARACTER_CARD_WIDTH - 40.0f; // Docked X position on the right side of the screen
            float dockY = SCREEN_HEIGHT - CHARACTER_CARD_HEIGHT - 300.0f; // Starting Y position for docking (from bottom of screen upwards)

            int dockIndex = 0; // Index to space out docked cards vertically (did not use i because when i tested it caused spacing issues)
            for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            {
                if (i != CharSelectionStuff[0])
                {
                    characterCards[i].targetAnimationPos.x = dockX; // Docked X position (consistant for all docked cards)
                    characterCards[i].targetAnimationPos.y= dockY + CHARACTER_DOCK_SPACING * dockIndex++; // Staggered Y position for each docked card (using dockIndex to space them out)
                    characterCards[i].targetAnimationPos.width = CHARACTER_CARD_WIDTH; // width stays the same
                    characterCards[i].targetAnimationPos.height = CHARACTER_CARD_HEIGHT; // height stays the same
                }
            }
        }

        // This is where the animation is applied to get the smooth movement effect
        for (int i = 0; i < MAX_CHAR_CARDS; i++)
        {
            // Interpolating current position towards target position using slopeInt function from animation namespace (defined in screenManager.h)
            // The t value calculated earlier based on delta time is used for smoothing
            // animation::slopeInt(start, end, t); // Overloaded for colors (to apply smooth color transitions) and Vector2 as well
            characterCards[i].currentAnimationPos.x =animation::slopeInt(characterCards[i].currentAnimationPos.x, characterCards[i].targetAnimationPos.x, t);
            characterCards[i].currentAnimationPos.y = animation::slopeInt(characterCards[i].currentAnimationPos.y, characterCards[i].targetAnimationPos.y, t);
        }

        // SreenRects[0] is the rectangle for the "Play Game" button (once again, wish there was a better way to do this with the dynamic approach for readability)
        ScreenRects[0].x      = (SCREEN_WIDTH - PLAY_BTN_WIDTH) / 2.0f; // Centered X position
        ScreenRects[0].width  = PLAY_BTN_WIDTH; // Width from macro
        ScreenRects[0].height = PLAY_BTN_HEIGHT; // Height from macro

        if (CharSelectionStuff[0] == -1) // No character selected
        {
            // Using the first character card (student) as reference for Y position(could have used any card since they are all in default row)
            //PLAY_BTN_OFFSET_Y is just a little extra spacing below the card
            ScreenRects[0].y = characterCards[0].currentAnimationPos.y + characterCards[0].currentAnimationPos.height + PLAY_BTN_OFFSET_Y;
        }
        else
        {
            // Using the selected character card for Y position (in this case it has to be the selected one since others are docked)
            ScreenRects[0].y = characterCards[CharSelectionStuff[0]].currentAnimationPos.y + characterCards[CharSelectionStuff[0]].currentAnimationPos.height +PLAY_BTN_OFFSET_Y;
        }

        SetMouseOffset(0, 0);
        SetMouseScale(1.0f, 1.0f);

        break;
    }
    case ScreenState::INTRO_CRAWL:
    {
        if (!scrollIntroCrawl)  break; // If intro crawl text is not initialized, exit early (safety check)
        introCrawlYPos -= INTRO_CRAWL_SPEED * dt; // Move the intro crawl text upwards based on speed and delta time
        if (introCrawlYPos <= INTRO_CRAWL_END_Y || IsKeyPressed(KEY_ENTER)) // If the intro crawl text has moved off the top of the screen
        {
            changeScreen(ScreenState::GAMEPLAY); // Transition to gameplay screen after intro crawl ends
        }
        break;
    }

    case ScreenState::GAMEPLAY:
        // TODO: Implement gameplay screen update logic (if needed)
        // Gameplay screen update logic would go here (not fully implemented yet)
        // right now all of that stuff is being handled by a GameManager class
        gameManager->update(dt);
        break;

    case ScreenState::SAVE_QUIT:
        // TODO: Implement save and quit screen update logic (if needed)
        // Save and quit screen update logic would go here (not fully implemented yet)
        break;
    }
}

//@brief: Render the current screen
//@version: 1.0
//author: Eswin Baiden
void ScreenManager::render()
{
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    scale = std::min(screenWidth / GAME_SCREEN_WIDTH, screenHeight / GAME_SCREEN_HEIGHT);
    offset.x = (screenWidth - (GAME_SCREEN_WIDTH * scale)) * 0.5f;
    offset.y = (screenHeight - (GAME_SCREEN_HEIGHT * scale)) * 0.5f;

    SetMouseOffset(-offset.x, -offset.y);
    SetMouseScale(1.0f / scale, 1.0f / scale);


    BeginTextureMode(target);
    ClearBackground(BLACK); // Clear the background to black before rendering any screen

    switch (currentScreen)
    {
        case ScreenState::MAIN_MENU:
        {
            //TODO: Add the extra button later once we have the progress JSON file working ("CONTINUE" button)
            /*
                -ScreenTextures[0]: Background texture for the main menu screen
                -ScreenTextures[1]: Logo texture for the main menu screen
            */
            DrawTexture(ScreenTextures[0], 0, 0, WHITE); // Draw the background texture (index 0)
            DrawTexture(ScreenTextures[1], (SCREEN_WIDTH - ScreenTextures[1].width) / 2, -150, WHITE); // Draw the logo texture (index 1) centered at the top with a Y offset

            

            /*
                -ScreenRects[0]: Rectangle for the "Start Game" button
                -ScreenRects[1]: Rectangle for the "Exit Game" button
            */
            // Setting up the start button and what happens when its clicked (change to character select screen)
            if (GuiButton(ScreenRects[0], "Start Game")) changeScreen(ScreenState::CHARACTER_SELECT);

            //  Setting up the exit button and what happens when its clicked (close the window)
            if (GuiButton(ScreenRects[1], "Exit Game"))
            {
                exitScreen(currentScreen); // Clean up resources before exiting
                CloseWindow();
                
            }
            break;
            
        }

        case ScreenState::CHARACTER_SELECT:
        {
            /*
                -ScreenTextures[0]: Background texture for the character selection screen
                -ScreenTextures[1-4]: Character card textures for the character selection screen
                -characterCards: Array of character card structures holding texture and animation positions
                -CharSelectionStuff[0]: Index of the currently selected character (-1 if none selected)
                -CharSelectionStuff[1]: Index of the currently hovered character (-1 if none hovered)
            */
            DrawTexture(ScreenTextures[0], 0, 0, WHITE); // Draw the background texture (index 0)
            

            CharSelectionStuff[1] = -1; // Reset hovered index eery frame

            for (int i = 0; i < MAX_CHAR_CARDS; i++)
            {
                // Draw each character card with its current animation position
                /*
                    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, tint);
                    - texture: The texture to draw (characterCards[i].texture).
                    - sourceRec: The source rectangle from the texture to draw (full texture in this case so from 0,0 to the texture's width and height).
                    - destRec: The destination rectangle on the screen where the texture will be drawn (characterCards[i].currentAnimationPos).
                    - origin: The origin point for rotation and scaling (0,0 means top-left corner).
                    - rotation: The rotation angle in degrees (0.0f means no rotation).
                    - tint: The color tint to apply to the texture (WHITE means no tint).

                    DrawRectangleLinesEx(rec, lineThick, color);
                    - rec: The rectangle to draw the outline for (characterCards[i].currentAnimationPos).
                    - lineThick: The thickness of the outline (4.0f pixels).
                    - color: The color of the outline (dark green in this case).
                */
                DrawTexturePro(characterCards[i].texture,{0.0f, 0.0f,(float)characterCards[i].texture.width,(float)characterCards[i].texture.height},characterCards[i].currentAnimationPos,{0.0f, 0.0f},0.0f,WHITE);
                DrawRectangleLinesEx(characterCards[i].currentAnimationPos, 4.0f, Color{0, 68, 0, 255});

                //Checking to see if the mouse is hovering over this card (if so, update hovered index)
                // FIX: Use GetVirtualMousePosition() for manual collision checks
                if (CheckCollisionPointRec(GetMousePosition(),characterCards[i].currentAnimationPos)) CharSelectionStuff[1] = i;

                // Check for clicks on the character card (only the first card is selectable for now)
                // used GuiButton with empty text so i can use raygui's button click detection
                if (i==0 && GuiButton(characterCards[i].currentAnimationPos, ""))
                {
                    if (CharSelectionStuff[0] == i) CharSelectionStuff[0] = -1; // Deselect if already selected
                    else CharSelectionStuff[0] = i; // Select this character
                }

                /*
                    - ScreenRects[2]: Rectangle for the inner yellow outline when a card is selected
                    - ScreenRects[3]: Rectangle for the outer yellow outline when a card is selected
                */
                // If this card is the selected one, draw yellow outlines around it for emphasis (just for more visual feedback)
                if (CharSelectionStuff[0] == i)
                {
                    ScreenRects[2].x = characterCards[i].currentAnimationPos.x - 6.0f; // Inner outline slightly offset from card position
                    ScreenRects[2].y = characterCards[i].currentAnimationPos.y - 6.0f;
                    ScreenRects[2].width = characterCards[i].currentAnimationPos.width + 12.0f; // Inner outline slightly larger than card size
                    ScreenRects[2].height = characterCards[i].currentAnimationPos.height + 12.0f;

                    ScreenRects[3].x = characterCards[i].currentAnimationPos.x - 12.0f; // Outer outline further offset from card position
                    ScreenRects[3].y = characterCards[i].currentAnimationPos.y - 12.0f;
                    ScreenRects[3].width = characterCards[i].currentAnimationPos.width + 24.0f; // Outer outline further larger than card size
                    ScreenRects[3].height = characterCards[i].currentAnimationPos.height + 24.0f;

                    DrawRectangleLinesEx(ScreenRects[2], 4, YELLOW); // Draw inner yellow outline
                    DrawRectangleLinesEx(ScreenRects[3], 2, YELLOW); // Draw outer yellow outline

                }
            }

            if (CharSelectionStuff[1] != -1 && CharSelectionStuff[1] != CharSelectionStuff[0]) // If a card is hovered and it's not the selected one
            {
                //ScreenRects[4] is the rectangle for the info box position
                ScreenRects[4] = characterCards[CharSelectionStuff[1]].currentAnimationPos; //Initialize it to the hovered card's current position 

                bool placeRight = (ScreenRects[4].x + CHARACTER_CARD_WIDTH + 260.0f) < SCREEN_WIDTH; // Checking if there is enough space to the right to place the info box
                // If not enough space on the right, it will be placed to the left of the card
                ScreenRects[1].x = placeRight ? (ScreenRects[4].x + CHARACTER_CARD_WIDTH + 5.0f): (ScreenRects[4].x - 5.0f - 260.0f);
                ScreenRects[1].y = ScreenRects[4].y - 250.0f; // Positioning the info box above the card
                ScreenRects[1].width  = 260.0f; // Fixed width for info box
                ScreenRects[1].height = 240.0f; // Fixed height for info box

                DrawRectangleRec(ScreenRects[1],Color{0, 40, 0, 200}); // Draw the info box background (dark green with some transparency)
                DrawRectangleLinesEx(ScreenRects[1], 3.0f,Color{40, 255, 80, 255}); // Draw the info box outline (bright green)

                switch (CharSelectionStuff[1]) // Switch based on the hovered character's ID (0 = Student, 1 = Rat, 2 = Professor, 3 = Atilla)
                {
                    case 0:
                        // Display stats for the Student character (only one implemented for now)
                        // Need to cast the stat values to string using std::to_string since DrawText requires a cstrings
                        // Using allStatLines pointer and getStatForCharacterID function to retrieve stats from the CSV file (defined in characters.cpp)
                        DrawText("Caste: Student", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                        DrawText(TextFormat("Health: %d", getStatForCharacterID(allStatLines,"Student",CSVStats::MAX_HEALTH)), (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 50), 20, WHITE);
                        DrawText(TextFormat("Armor: %d", getStatForCharacterID(allStatLines,"Student",CSVStats::ARMOR)), (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 80), 20, WHITE);
                        DrawText(TextFormat("Dexterity: %d", getStatForCharacterID(allStatLines,"Student",CSVStats::DEX)), (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 110), 20, WHITE);
                        DrawText(TextFormat("Constitution: %d", getStatForCharacterID(allStatLines,"Student",CSVStats::CON)), (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 140), 20, WHITE);
                        DrawText(TextFormat("Initiative: %d", getStatForCharacterID(allStatLines,"Student",CSVStats::INITIATIVE)), (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 170), 20, WHITE);
                        break;

                    case 1:
                        // Display stats for the Rat character (not implemented)
                        //TODO: Fill in actual stats when implemented
                        DrawText("Caste: Rat", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                        DrawText("Not Available", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 50), 20, WHITE);
                        break;

                    case 2:
                        // Display stats for the Professor character (not implemented)
                        //TODO: Fill in actual stats when implemented
                        DrawText("Caste: Professor", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                        DrawText("Not Available", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 50), 20, WHITE);
                        break;

                    case 3:
                        // Display stats for the Atilla character (not implemented)
                        //TODO: Fill in actual stats when implemented
                        DrawText("Caste: Atilla", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                        DrawText("Not Available", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 50), 20, WHITE);
                        break;
                }
            }

            // Setting up the "Play Game" button
            // GuiDisable() is used to disable the button if no character is selected (CharSelectionStuff[0] == -1)

            // Not sure if there a better way to disable just one button using raygui
            // so what i am currently doing is in each frame create and show all the character card buttons normally
            // but before rendering the "Play Game" button i check if a character is selected
            // if not selected i disable the gui (which disables all buttons)
            int prevState = GuiGetState(); // Store previous GUI state
            if (CharSelectionStuff[0] == -1) GuiDisable(); // Disable GUI if no character selected

            if (GuiButton(ScreenRects[0], "Play Game") && CharSelectionStuff[0] != -1) // If "Play Game" button is clicked and a character is selected
            {
                entities = new Character*[2] { nullptr, nullptr }; // Dynamically allocate array for player and NPC characters
                CreateCharacter("Student","Steve"); // Create the player character (only student implemented for now)
                CreateCharacter("Zombie_Standard","Chad"); // Create the NPC character (only zombie implemented for now)
                scrollIntroCrawl = new std::stringstream(); // Dynamically allocate stringstream for intro crawl text
                getIntroCrawlText(scrollIntroCrawl, 1); // Populate the intro crawl text based on selected character
                introCrawlYPos = INTRO_CRAWL_START_Y;

                // MUST End texture mode before returning if we are exiting the function early
                EndTextureMode();

                changeScreen(ScreenState::INTRO_CRAWL); // Change to gameplay screen
                return; // Exit render function early to avoid drawing the rest of the character select screen
            }
            GuiSetState(prevState); //If a character is not selected re-enable the GUI to restore previous state


        break;
        }
        case ScreenState::INTRO_CRAWL:
        {
            if (!scrollIntroCrawl) break;

            scrollIntroCrawl->clear(); // Clear any error flags
            scrollIntroCrawl->seekg(0, std::ios::beg); // Reset stringstream to beginning
            std::string line;

            float y = introCrawlYPos;

            while (std::getline(*scrollIntroCrawl, line))
            {
                if (!line.empty())
                {
                    int textWidth = MeasureText(line.c_str(), INTRO_CRAWL_FONT_SIZE);
                    float x = (SCREEN_WIDTH - (float)textWidth) / 2.0f;

                    DrawText(line.c_str(), (int)x, (int)y,INTRO_CRAWL_FONT_SIZE, GOLD);
                }

                y += INTRO_CRAWL_LINE_HEIGHT;
            }

            DrawText("Press ENTER to skip", 20, (int)SCREEN_HEIGHT - 40, 20, GRAY);

            break;
        }       
        
        case ScreenState::GAMEPLAY:
        {
            //Not much to render here since all of that is being handled by the GameManager class
            //That may change later as we add more UI elements specific to gameplay
            gameManager->update(GetFrameTime()); // Update the game manager with delta time
            gameManager->render(); // Render the game manager
            break;
        }

        case ScreenState::SAVE_QUIT:
        {
            // TODO: Implement save and quit screen rendering (not fully implemented yet)
            break;
        }
    }

    EndTextureMode(); // Stop drawing to virtual screen

    // 4. Draw Virtual Texture to Real Screen
    BeginDrawing();
        ClearBackground(BLACK); // Black bars for letterboxing

        // Source rect is negative height to flip it correctly for OpenGL coordinates
        Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        Rectangle destRec = { offset.x, offset.y, (float)GAME_SCREEN_WIDTH * scale, (float)GAME_SCREEN_HEIGHT * scale };
        
        DrawTexturePro(target.texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
    SetMouseOffset(0, 0);
    SetMouseScale(1.0f, 1.0f);
    EndDrawing();
}

//@brief: Enter a specific screen state and set up resources
//@param s - The screen state to enter
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::enterScreen(ScreenState s)
{
    switch (s)
    {
        case ScreenState::MAIN_MENU:
        {
            startMenuStyles(); // Apply main menu styles to buttons
            // Load resources for the main menu screen
            /*
                -ScreenTextures[0]: Background texture for the main menu screen
                -ScreenTextures[1]: Logo texture for the main menu screen
                -ScreenRects[0]: Rectangle for the "Start Game" button
                -ScreenRects[1]: Rectangle for the "Exit Game" button
            */
            numScreenTextures = 2;
            ScreenTextures = new Texture2D[numScreenTextures]; // Dynamically allocate array for screen textures
            ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png"); // Background texture
            ScreenTextures[1] = LoadTexture("../assets/images/UI/gameTitle.png"); // Logo texture

            numScreenRects = 2;
            ScreenRects = new Rectangle[numScreenRects];  // Dynamically allocate array for screen rectangles
            ScreenRects[0].x = (SCREEN_WIDTH - MAIN_BUTTON_WIDTH) / 2.0f; //Calculating x position for "Start Game" button
            ScreenRects[0].y = (SCREEN_HEIGHT - MAIN_BUTTON_HEIGHT) / 2.0f + MAIN_BUTTON_OFFSET_Y; // Y position with offset for "Start Game" button
            ScreenRects[0].width = MAIN_BUTTON_WIDTH; // Width for "Start Game" button
            ScreenRects[0].height = MAIN_BUTTON_HEIGHT; // Height for "Start Game" button

            ScreenRects[1].x = (SCREEN_WIDTH - MAIN_BUTTON_WIDTH) / 2.0f; // Calculating x position for "Exit Game" button
            ScreenRects[1].y = (SCREEN_HEIGHT - MAIN_BUTTON_HEIGHT) / 2.0f + MAIN_BUTTON_OFFSET_Y + MAIN_BUTTON_SPACING; // Y position with offset and spacing for "Exit Game" button
            ScreenRects[1].width = MAIN_BUTTON_WIDTH; // Width for "Exit Game" button
            ScreenRects[1].height = MAIN_BUTTON_HEIGHT; // Height for "Exit Game" button
            break;
        }
        case ScreenState::CHARACTER_SELECT:
        {
            playerSelectStyles(); // Apply character selection styles to buttons
            allStatLines = storeAllStatLines(openStartingStatsCSV()); // Load and store all character stats from the CSV file(defined in characters.cpp and it handles auto closing and fstream pointer cleanup)
            characterCards = new charCard[MAX_CHAR_CARDS]; // Dynamically allocate array for character cards

            /*
                -ScreenTextures[0]: Background texture for the character selection screen
                -ScreenTextures[1-4]: Character card textures for the character selection screen
                -characterCards: Array of character card structures holding texture and animation positions
                -CharSelectionStuff[0]: Index of the currently selected character (-1 if none selected)
                -CharSelectionStuff[1]: Index of the currently hovered character (-1 if none hovered)
                -CharSelectionStuff[2]: Flag to indicate if layout has been initialized (0 = not initialized, 1 = initialized)
            */
            CharSelectionStuff = new int[3]; // Dynamically allocate array for character selection state variables
            CharSelectionStuff[0] = -1; // No character selected initially
            CharSelectionStuff[1] = -1; // No character hovered initially
            CharSelectionStuff[2] = 0; // Layout not initialized yet

            numScreenTextures = 5;
            ScreenTextures = new Texture2D[numScreenTextures]; // Dynamically allocate array for screen textures
            ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png"); // Background texture
            ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/south.png"); // Character card texture
            ScreenTextures[2] = LoadTexture("../assets/images/characters/pc/Rat-Assassin/rotations/south.png"); // Character card texture
            ScreenTextures[3] = LoadTexture("../assets/images/characters/pc/Professor-Mage/rotations/south.png"); // Character card texture
            ScreenTextures[4] = LoadTexture("../assets/images/characters/pc/Attila-Brawler/rotations/south.png"); // Character card texture
        
            /*
                -ScreenRects[0]: Rectangle for the "Play Game" button
                -ScreenRects[1]: Rectangle for the hovered character info box
                -ScreenRects[2]: Rectangle for the inner yellow outline when a card is selected
                -ScreenRects[3]: Rectangle for the outer yellow outline when a card is selected
                -ScreenRects[4]: Rectangle for the info box position
            */
            numScreenRects = 5;
            ScreenRects = new Rectangle[numScreenRects]; // Dynamically allocate array for screen rectangles
            for (int i = 0; i < numScreenRects; ++i)
            {
                ScreenRects[i] = {0, 0, 0, 0}; // Initialize rectangles to zero
            }
        
            break;
        }
        case ScreenState::INTRO_CRAWL:
        {
            break;
        }
        case ScreenState::GAMEPLAY:
        {
            gamePlayStyles(); // Apply gameplay styles to buttons
            gameManager = new GameManager; // Create a new GameManager object
            gameManager->enterGameState(gameManager->getCurrentGameState()); // Enter the initial game state
            break;
        }
        case ScreenState::SAVE_QUIT:
        {
            // Handle save and quit logic here (not fully implemented yet)
            //TODO: Implement save and quit screen setup
            break;
        }
    }
}

//@brief: Exit a specific screen state and clean up resources(since we are really only using dynamic memory allocation for resources, this is mainly just deleting those pointers)
//@param s - The screen state to exit
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::exitScreen(ScreenState s)
{
    switch (s) //Most of the stuff is reused so we just write the cleanup code once in the SAVE_QUIT case
    {
        case ScreenState::MAIN_MENU:
        case ScreenState::CHARACTER_SELECT:
        case ScreenState::INTRO_CRAWL:
        {
            if(scrollIntroCrawl && s == ScreenState::INTRO_CRAWL)
            {
                delete scrollIntroCrawl; // Delete the dynamically allocated stringstream for intro crawl text
                scrollIntroCrawl = nullptr; // Set pointer to nullptr to avoid dangling pointer
            }
        }
        case ScreenState::GAMEPLAY:
        {
            // The GAMPLAY case needs to clean up the gameManager object specifically before falling through to the SAVE_QUIT case
            if (s == ScreenState::GAMEPLAY && gameManager)
            {
                gameManager->exitGameState(gameManager->getCurrentGameState()); // Exit the current game state (clean up game state resources)
                delete gameManager; // Delete the game manager object
                gameManager = nullptr; // Set pointer to nullptr to avoid dangling pointer
            }
        }
        case ScreenState::SAVE_QUIT:
        {
            // Handling cleanup of dynamically allocated resources for all screen states
            //Textures
            for (int i = 0; i < numScreenTextures; ++i)
            {
                UnloadTexture(ScreenTextures[i]); // Unload each texture to free GPU memory
                ScreenTextures[i] = {}; // Reset texture to default state
            }
            delete[] ScreenTextures; // Delete the dynamically allocated array of textures
            ScreenTextures = nullptr; // Set pointer to nullptr to avoid dangling pointer
            numScreenTextures = 0; // Reset texture count

            //Rectangles 
            for (int i = 0; i < numScreenRects; ++i)
            {
                ScreenRects[i] = {}; // Reset rectangle to default state
            }
            delete[] ScreenRects; // Delete the dynamically allocated array of rectangles
            ScreenRects = nullptr; // Set pointer to nullptr to avoid dangling pointer
            numScreenRects = 0; // Reset rectangle count

            if (characterCards) // Clean up character cards if they were allocated
            {
                delete[] characterCards; // Delete the dynamically allocated array of character cards
                characterCards = nullptr; // Set pointer to nullptr to avoid dangling pointer
            }

            if (CharSelectionStuff) // Clean up character selection state variables if they were allocated
            {
                delete[] CharSelectionStuff; // Delete the dynamically allocated array of character selection state variables
                CharSelectionStuff = nullptr; // Set pointer to nullptr to avoid dangling pointer
            }

            if (allStatLines) // Clean up character stats if they were allocated
            {
                allStatLines->clear(); //clear the string streams
                allStatLines->str(""); //set to empty
                delete allStatLines; // Delete the dynamically allocated vector of character stats
                allStatLines = nullptr; // Set pointer to nullptr to avoid dangling pointer
            }
            break;
        }
    }
}

// =================== GAMEMANAGER CLASS FUNCTION DEFINITIONS ===================
// Very similar to ScreenManager but handles different but uses a different enum for game states

//@brief: Constructor for the GameManager class
//@param initial - The initial game state to start with (default is COMBAT for testing and milestone purposes)
//@version: 1.0
//@author: Edwin Baiden
GameManager::GameManager(GameState initial)
{
    ChangeDirectory(GetApplicationDirectory()); // Ensure working directory is set to application directory (Cause MacOS)
    currentGameState = initial;
}

//@brief: Destructor for the GameManager class
//@version: 1.0
//@author: Edwin Baiden
GameManager::~GameManager()
{
    // Clean up any resources used by the game manager if not already done
    exitGameState(currentGameState);
}

//@brief: Change the current game state to a new state
//@param newState - The new game state to change to
//@version: 1.0
//@author: Edwin Baiden
void GameManager::changeGameState(GameState newState)
{
    if (newState == currentGameState) return; //Checking to see if we are already in the desired state (if so, do nothing)
    exitGameState(currentGameState); // Clean up current state resources
    currentGameState = newState; // Update to the new state
    enterGameState(currentGameState); // Set up new state resources
}

//@brief: Get the current game state
//@version: 1.0
//@author: Edwin Baiden
GameState GameManager::getCurrentGameState() const // Const function since it does not modify any member variables
{
    return currentGameState; // Return the current game state
}

//@brief: Enter a specific game state and set up resources
//@param state - The game state to enter
//@version: 1.0
//@author: Edwin Baiden
void GameManager::enterGameState(GameState state)
{
    switch (state)
    {
        case GameState::EXPLORATION:
        {
            //TODO: Implement exploration state setup
            break;
        }

        case GameState::COMBAT:
        {
            // Load resources for the combat state
            /*
                -ScreenTextures[0]: Background texture for the combat state
                -ScreenTextures[1]: Player character texture for the combat state (For now  just using the student fighter)
                -ScreenTextures[2]: Enemy character texture for the combat state (For now just using a standard frat bro enemy)
                -ScreenRects: Array of rectangles defining UI element positions and sizes for the combat state (using macro definitions for readibility , may be implemented for the other stuff later depending on time)
            */

            numScreenTextures = 3;
            ScreenTextures = new Texture2D[numScreenTextures];
            ScreenTextures[0] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[1-2].png");
            ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/north-west.png");
            ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/FratBro1.png");

            numScreenRects = 18;
            ScreenRects = new Rectangle[numScreenRects]; // Dynamically allocate array for screen rectangles

            ScreenRects[R_PLAYER_NAME] = {0,0, 450, 50};
            ScreenRects[R_ENEMY_NAME] = {SCREEN_WIDTH - ScreenRects[R_PLAYER_NAME].width, 0, ScreenRects[R_PLAYER_NAME].width, ScreenRects[R_PLAYER_NAME].height};
            ScreenRects[R_PLAYER_PANEL]= {0, ScreenRects[R_PLAYER_NAME].height, ScreenRects[R_PLAYER_NAME].width, 832};
            ScreenRects[R_ENEMY_PANEL] = {ScreenRects[R_ENEMY_NAME].x,ScreenRects[R_ENEMY_NAME].y + ScreenRects[R_ENEMY_NAME].height, ScreenRects[R_ENEMY_NAME].width, 832};
            ScreenRects[R_PLAYER_HP_BG] = {ScreenRects[R_PLAYER_PANEL].x + 20, ScreenRects[R_PLAYER_PANEL].y + 100, ScreenRects[R_PLAYER_PANEL].width - 40, 30};
            ScreenRects[R_PLAYER_HP_FG] = ScreenRects[R_PLAYER_HP_BG];
            ScreenRects[R_ENEMY_HP_BG] = {ScreenRects[R_ENEMY_PANEL].x + 20, ScreenRects[R_ENEMY_PANEL].y + 100,ScreenRects[R_ENEMY_PANEL].width - 40,30};
            ScreenRects[R_ENEMY_HP_FG] = ScreenRects[R_ENEMY_HP_BG];
            ScreenRects[R_PLAYER_STATUS] = {ScreenRects[R_PLAYER_PANEL].x + 20,ScreenRects[R_PLAYER_PANEL].y + 200, ScreenRects[R_PLAYER_PANEL].width - 40,500};
            ScreenRects[R_ENEMY_STATUS] = {ScreenRects[R_ENEMY_PANEL].x + 20,ScreenRects[R_ENEMY_PANEL].y + 200,ScreenRects[R_ENEMY_PANEL].width - 40,500};
            ScreenRects[R_BOTTOM_PANEL] = {0,SCREEN_HEIGHT - 200,SCREEN_WIDTH,215};
            ScreenRects[R_BTN_ATTACK] = {ScreenRects[R_BOTTOM_PANEL].x + 20,ScreenRects[R_BOTTOM_PANEL].y + 20,400,80};
            ScreenRects[R_BTN_DEFEND] = {ScreenRects[R_BOTTOM_PANEL].x + 20,ScreenRects[R_BTN_ATTACK].y + 100,400,80};
            ScreenRects[R_BTN_USE_ITEM] = {ScreenRects[R_BTN_ATTACK].x + ScreenRects[R_BTN_ATTACK].width + 150,ScreenRects[R_BTN_ATTACK].y,400,80};
            ScreenRects[R_LOG_BOX] = {SCREEN_WIDTH - 800, ScreenRects[R_BTN_ATTACK].y, 780, 175};
            ScreenRects[R_ATTACK_MENU] = {0};
            ScreenRects[R_MELEE_BTN] = {0};
            ScreenRects[R_RANGED_BTN] = {0};


            // Dynamically allocate combat handler and initialize combat state variables
            combatHandler = new CombatHandler;
            combatHandler->playerTurn = entities[0]->cbt.initiative >= entities[1]->cbt.initiative; // Determine who goes first based on initiative (simple comparison for now)
            combatHandler->playerIsDefending = false;
            combatHandler->enemyIsDefending = false;
            combatHandler->playerHitFlashTimer = 0.0f;
            combatHandler->enemyHitFlashTimer = 0.0f;
            combatHandler->gameOverState = false;
            combatHandler->victoryState = false;
            combatHandler->gameOverTimer = 0.0f;
            combatHandler->showAttackMenu = false;

            combatHandler->log.clear();
            combatHandler->logScrollOffset = 0.0f;
            AddNewLogEntry(combatHandler->log, "A wild " + entities[1]->getName() + " appears!"); // Initial log entry

            combatHandler->enemyActionDelay = 1.0f; // Reset enemy action delay timer
            break;
        }

        case GameState::DIALOGUE:
        {
            //TODO: Implement dialogue state setup
            break;
        }

        case GameState::PAUSE_MENU:
        {
            //TODO: Implement pause menu state setup
            break;
        }
    }
}

//@brief: Exit a specific game state and clean up resources
//@param state - The game state to exit
//@version: 1.0
//@author: Edwin Baiden
void GameManager::exitGameState(GameState state) //Not needed currently since we are only using the combat state(screenManager takes care of that) but may be useful later
{
    switch (state)
    {
        case GameState::EXPLORATION:
        {
            //TODO: Implement exploration state cleanup
            break;
        }

        case GameState::COMBAT:
        {
            //TODO: Implement combat state cleanup
            // Clean up combat handler
            if (combatHandler)
            {
                delete combatHandler;
                combatHandler = nullptr;
            }
            break;
        }

        case GameState::DIALOGUE:
        {
            //TODO: Implement dialogue state cleanup
            break;
        }

        case GameState::PAUSE_MENU:
        {
            //TODO: Implement pause menu state cleanup
            break;
        }
    }
}

//@brief: Update the current game state
//@param dt - Delta time since the last update
//@version: 1.0
//@author: Edwin Baiden
void GameManager::update(float dt) //Currently only updating the health bars in combat state but will be expanded later
{
    // FIX: Calculate virtual mouse specifically for manual collision checks inside GameManager
    // Since GameManager isn't ScreenManager, we duplicate the scale math locally to avoid changing class headers too much
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float scale = std::min(sw / GAME_SCREEN_WIDTH, sh / GAME_SCREEN_HEIGHT);
    Vector2 offset = { (sw - (GAME_SCREEN_WIDTH * scale)) * 0.5f, (sh - (GAME_SCREEN_HEIGHT * scale)) * 0.5f };
    Vector2 mouse = GetMousePosition();
    Vector2 virtualMouse = { (mouse.x - offset.x) / scale, (mouse.y - offset.y) / scale };

    switch (currentGameState)
    {
        case GameState::EXPLORATION:
        {
            //TODO: Implement exploration state update
            break;
        }

        case GameState::COMBAT:
        {
            if(!combatHandler) break; // Safety check to ensure combat handler is valid
            if(!entities[0] || !entities[1]) break; // Safety check to ensure characters are valid
            //TODO: Implement combat state update logic (for now just updating health bars)
            // Update health bar widths based on current health
            ScreenRects[R_PLAYER_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_PLAYER_HP_BG],entities[0]->vit.health, entities[0]->vit.maxHealth);
            ScreenRects[R_ENEMY_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_ENEMY_HP_BG], entities[1]->vit.health, entities[1]->vit.maxHealth);
            combatHandler->playerHitFlashTimer = std::max(0.0f, combatHandler->playerHitFlashTimer - dt);
            combatHandler->enemyHitFlashTimer  = std::max(0.0f, combatHandler->enemyHitFlashTimer  - dt);

            // FIX: Use virtualMouse for the log box scroll check
            if (CheckCollisionPointRec(virtualMouse, ScreenRects[R_LOG_BOX]))
            {
                float wheel = GetMouseWheelMove();
                if (wheel != 0.0f)
                {
                    combatHandler->logScrollOffset += wheel * -25.0f;
                }
            }

            // Handle game over states
            if (combatHandler->gameOverState || combatHandler->victoryState)
            {
                combatHandler->gameOverTimer -= dt;
                if (combatHandler->gameOverTimer <= 0.0f)
                {
                    // Return to main menu instead of unimplemented exploration
                    // This prevents the window from closing unexpectedly
                    if (combatHandler->gameOverState)
                    {
                        gameManager->changeGameState(GameState::EXPLORATION); // Refresh combat state to handle game over
                    }
                    else if (combatHandler->victoryState)
                    {
                        gameManager->changeGameState(GameState::EXPLORATION); // Refresh combat state to handle victory
                    }
                }
                break; // Don't process normal combat logic during game over
            }

            if(!combatHandler->playerTurn)
            {
                combatHandler->enemyActionDelay -= dt;
                if (combatHandler->enemyActionDelay <= 0.0f)
                {
                    Action enemyAction = ai_choose(*(dynamic_cast<NonPlayerCharacter*>(entities[1])), *(dynamic_cast<PlayerCharacter*>(entities[0]))); // AI chooses an action

                    if (enemyAction.type == ActionType::Attack)
                    {
                        combatHandler->playerHitFlashTimer = resolve_melee(*entities[1], *entities[0], combatHandler->playerIsDefending, combatHandler->log) ? 0.2f : 0.0f;
                        combatHandler->logScrollOffset = 1000.0f; // Auto scroll to bottom on new log entry
                        if (!entities[0]->isAlive())
                        {
                            AddNewLogEntry(combatHandler->log, "You died.");
                            combatHandler->gameOverTimer = 2.0f; // Set a timer instead of blocking
                            combatHandler->gameOverState = true;
                            return;
                        }
                    }
                    else if (enemyAction.type == ActionType::Defend)
                    {
                        combatHandler->enemyIsDefending = true;
                        AddNewLogEntry(combatHandler->log, entities[1]->getName() + " is defending!");
                        combatHandler->logScrollOffset = 1000.0f; // Auto scroll to bottom on new log entry
                    }
                    combatHandler->playerTurn = true;
                    combatHandler->playerIsDefending  = false;
                    
                }
            }
            break;
        }

        case GameState::DIALOGUE:
        {
            //TODO: Implement dialogue state update
            break;
        }

        case GameState::PAUSE_MENU:
        {
            //TODO: Implement pause menu state update
            //Probably not needed since nothing changes in pause menu
            break;
        }
    }
}

//@brief: Render the current game state
//@version: 1.0
//@author: Edwin Baiden
void GameManager::render()
{
    switch (currentGameState)
    {
        case GameState::EXPLORATION:
        {
            //TODO: Implement exploration state rendering
            break;
        }

        case GameState::COMBAT:
        {
            //TODO: Make rendering for combatmore dynamic later on but for now just hardcoding stuff for testing/milestone purposes

            //This pportion is mostly hardcoded for testing purposes. Its definitely going to become more dynamic and have structs for positioning later on
            // Drawing the Combat background, horizontally centered (SCREEN_WIDTH - ScreenTextures[0].width)/2, making it slightly shifted up to make room for UI elements at the bottom
            if(!combatHandler) break; // Safety check to ensure combat handler is valid
            if(!entities[0] || !entities[1]) break; // Safety check to ensure characters are valid
            if(!ScreenTextures || numScreenTextures < 3) break; // Safety check for textures
            if(!ScreenRects || numScreenRects < 15) break; // Safety check for rectangles
            
            DrawTexture(ScreenTextures[0],(int)(SCREEN_WIDTH / 2.0f - ScreenTextures[0].width  / 2.0f), (int)(SCREEN_HEIGHT / 2.0f - ScreenTextures[0].height / 2.0f - 175.f), WHITE);

            // Drawing the Player character texture, positioned to the right side of the screen within the combat background (this is where a struct for character positions will come in handy later)
            if (combatHandler->playerHitFlashTimer > 0.0f) DrawTexturePro(ScreenTextures[1], {0.0f, 0.0f,(float)ScreenTextures[1].width, (float)ScreenTextures[1].height}, {SCREEN_WIDTH / 2.0f + ScreenTextures[0].width / 2.0f - 500.f, SCREEN_HEIGHT / 2.0f + ScreenTextures[0].height / 2.0f - 650.f, 500.0f, 500.0f}, {0.0f, 0.0f}, 0.0f, RED);
            else DrawTexturePro(ScreenTextures[1], {0.0f, 0.0f,(float)ScreenTextures[1].width, (float)ScreenTextures[1].height}, {SCREEN_WIDTH / 2.0f + ScreenTextures[0].width / 2.0f - 500.f, SCREEN_HEIGHT / 2.0f + ScreenTextures[0].height / 2.0f - 650.f, 500.0f, 500.0f}, {0.0f, 0.0f}, 0.0f, WHITE);

            // Drawing the Enemy character texture, positioned to the left side of the screen within the combat background (this is where a struct for character positions will come in handy later)
            if (combatHandler->enemyHitFlashTimer > 0.0f) DrawTexturePro(ScreenTextures[2], {0.0f, 0.0f, (float)ScreenTextures[2].width, (float)ScreenTextures[2].height}, {SCREEN_WIDTH / 2.0f + ScreenTextures[0].width / 2.0f - 600.f, SCREEN_HEIGHT / 2.0f + ScreenTextures[0].height / 2.0f - 725.f, 200.f, 300.f}, {0.0f, 0.0f}, 0.0f, RED);
            else DrawTexturePro(ScreenTextures[2], {0.0f, 0.0f, (float)ScreenTextures[2].width, (float)ScreenTextures[2].height}, {SCREEN_WIDTH / 2.0f + ScreenTextures[0].width / 2.0f - 600.f, SCREEN_HEIGHT / 2.0f + ScreenTextures[0].height / 2.0f - 725.f, 200.f, 300.f}, {0.0f, 0.0f}, 0.0f, WHITE);

            // Drawing UI elements using predefined rectangles(defined in the enterGameState for combat) and colors
            DrawRectangleRec(ScreenRects[R_PLAYER_NAME],COL_NAME_BAR);
            DrawRectangleRec(ScreenRects[R_ENEMY_NAME], COL_NAME_BAR);
            DrawRectangleRec(ScreenRects[R_BOTTOM_PANEL], COL_BOTTOM_PANEL);
            DrawRectangleRec(ScreenRects[R_PLAYER_PANEL], COL_STATUS_PANEL);
            DrawRectangleRec(ScreenRects[R_ENEMY_PANEL], COL_STATUS_PANEL);

            // Drawing the HP bars (background and foreground)
            DrawRectangleRec(ScreenRects[R_PLAYER_HP_BG],COL_HP_BG);
            DrawRectangleRec(ScreenRects[R_PLAYER_HP_FG],COL_HP_FG);
            DrawRectangleRec(ScreenRects[R_ENEMY_HP_BG], COL_HP_BG);
            DrawRectangleRec(ScreenRects[R_ENEMY_HP_FG],COL_HP_FG);

            // Status boxes
            DrawRectangleRec(ScreenRects[R_PLAYER_STATUS],COL_STATUS_INNER);
            DrawRectangleRec(ScreenRects[R_ENEMY_STATUS],COL_STATUS_INNER);

            // Log box make this scrollable
            DrawRectangleRec(ScreenRects[R_LOG_BOX],COL_LOG_BOX);
            const float totalHeight   = LOG_LINE_HEIGHT * (float)combatHandler->log.size();
            const float visibleHeight = ScreenRects[R_LOG_BOX].height - 10.0f;
            float minScroll = 0.0f;
            if (totalHeight > visibleHeight)
            {
                minScroll = totalHeight - visibleHeight;
                combatHandler->logScrollOffset = std::clamp(combatHandler->logScrollOffset, 0.0f, minScroll);
            }
            else combatHandler->logScrollOffset = 0.0f; // No scrolling needed if all log entries fit

            
    


            // Buttons
            DrawRectangleRec(ScreenRects[R_BTN_ATTACK],COL_BUTTON);
            DrawRectangleRec(ScreenRects[R_BTN_DEFEND], COL_BUTTON);
            DrawRectangleRec(ScreenRects[R_BTN_USE_ITEM], COL_BUTTON);

            // Drawing the outlines for each rectangle except for the HP foreground bars (to make the hp bars look like they are depleating)
            for (int i = 0; i < 15; ++i)
            {
                if (i == R_PLAYER_HP_FG || i == R_ENEMY_HP_FG) continue; // Skip HP foreground bars
                DrawRectangleLinesEx(ScreenRects[i], 3.0f, BLACK); // Draw outline with thickness 3 and black color
            }

            // Drawing text information (names, HP values)
            // Once again need to convert numbers to strings using std::to_string since DrawText requires cstrings
            DrawText(("Player: " + entities[0]->getName()).c_str(), (int)(ScreenRects[R_PLAYER_NAME].x + 20),(int)(ScreenRects[R_PLAYER_NAME].y + 10), FONT_SIZE_NAME, WHITE);

            DrawText(("Enemy: " + entities[1]->getName()).c_str(), (int)(ScreenRects[R_ENEMY_NAME].x + 20), (int)(ScreenRects[R_ENEMY_NAME].y + 10), FONT_SIZE_NAME, WHITE);

            DrawText(TextFormat("HP: %d / %d", entities[0]->vit.health, entities[0]->vit.maxHealth), (int)(ScreenRects[R_PLAYER_PANEL].x + 30),(int)(ScreenRects[R_PLAYER_PANEL].y + 130),FONT_SIZE_HP,WHITE);
            DrawText(TextFormat("HP: %d / %d", entities[1]->vit.health, entities[1]->vit.maxHealth), (int)(ScreenRects[R_ENEMY_PANEL].x + 30),(int)(ScreenRects[R_ENEMY_PANEL].y + 130),FONT_SIZE_HP, WHITE);

            if (combatHandler->playerTurn)
{
    // 1. ATTACK BUTTON (Only toggles menu)
    if (GuiButton(ScreenRects[R_BTN_ATTACK], "ATTACK"))
    {
        combatHandler->showAttackMenu = !combatHandler->showAttackMenu;
        
        if (combatHandler->showAttackMenu) 
            AddNewLogEntry(combatHandler->log, "Choose your attack.");
        else 
            AddNewLogEntry(combatHandler->log, "Attack cancelled.");
            
        combatHandler->logScrollOffset = 1000.0f;
    }

    // 2. ATTACK MENU (Where the actual attacks happen)
    if (combatHandler->showAttackMenu)
    {
        // Define the pop-up menu area
        //{x, y, width, height}
        ScreenRects[R_ATTACK_MENU] = {ScreenRects[R_BTN_ATTACK].x + ScreenRects[R_BTN_ATTACK].width + 10, ScreenRects[R_BTN_ATTACK].y - 150, 300.0f, 140.0f};
        DrawRectangleRec(ScreenRects[R_ATTACK_MENU], COL_BOTTOM_PANEL);
        DrawRectangleLinesEx(ScreenRects[R_ATTACK_MENU], 3.0f, BLACK);
        
        ScreenRects[R_MELEE_BTN] = {ScreenRects[R_ATTACK_MENU].x + 10, ScreenRects[R_ATTACK_MENU].y + 10, ScreenRects[R_ATTACK_MENU].width - 20, 50.0f};
        ScreenRects[R_RANGED_BTN] = {ScreenRects[R_ATTACK_MENU].x + 10, ScreenRects[R_ATTACK_MENU].y + 75, ScreenRects[R_ATTACK_MENU].width - 20, 50.0f};

        // --- MELEE BUTTON ---
        if (GuiButton(ScreenRects[R_MELEE_BTN], "Melee Attack"))
        {
            combatHandler->showAttackMenu = false; // Close menu
            combatHandler->playerIsDefending = false;
            
            // Resolve Melee Damage
            bool hit = resolve_melee(*entities[0], *entities[1], combatHandler->enemyIsDefending, combatHandler->log);
            combatHandler->enemyHitFlashTimer = hit ? 0.2f : 0.0f;
            
            // End Turn
            combatHandler->logScrollOffset = 1000.0f;
            combatHandler->playerTurn = false;
            combatHandler->enemyActionDelay = 0.6f;
        }

        // --- RANGED BUTTON ---
        if (GuiButton(ScreenRects[R_RANGED_BTN], "Ranged Attack"))
        {
            combatHandler->showAttackMenu = false; // Close menu
            combatHandler->playerIsDefending = false;
            
            bool hit = resolve_ranged(*entities[0], *entities[1], combatHandler->enemyIsDefending, combatHandler->log);
            combatHandler->enemyHitFlashTimer = hit ? 0.2f : 0.0f;
            
            // End Turn
            combatHandler->logScrollOffset = 1000.0f;
            combatHandler->playerTurn = false;
            combatHandler->enemyActionDelay = 0.6f;
        }

        if (!entities[1]->isAlive())
        {
            AddNewLogEntry(combatHandler->log, "You have defeated " + entities[1]->getName() + "!");
            combatHandler->gameOverTimer = 2.0f;
            combatHandler->victoryState = true;
            return; // Exit render
        }
    } else
    {
        ScreenRects[R_ATTACK_MENU] = {0}; // Reset attack menu rectangle when not shown
        ScreenRects[R_MELEE_BTN] = {0}; // Reset melee button rectangle when not
        ScreenRects[R_RANGED_BTN] = {0}; // Reset ranged button rectangle when not shown
    }

    // 3. DEFEND BUTTON (Outside menu logic)
    if (GuiButton(ScreenRects[R_BTN_DEFEND], "DEFEND"))
    {
        combatHandler->showAttackMenu = false; // Close menu if they switch to defend
        combatHandler->playerIsDefending = true;
        AddNewLogEntry(combatHandler->log, entities[0]->getName() + " is defending!");
        combatHandler->logScrollOffset = 1000.0f;
        combatHandler->playerTurn = false;
        combatHandler->enemyActionDelay = 0.6f;
    }
    
    // 4. ITEM BUTTON
    if (GuiButton(ScreenRects[R_BTN_USE_ITEM], "USE ITEM"))
    {
        combatHandler->showAttackMenu = false;
        // TODO: Item Logic
    }
    
    combatHandler->enemyIsDefending = false;
}
            else
            {
                int prevState = GuiGetState();
                GuiDisable();
                GuiButton(ScreenRects[R_BTN_ATTACK], "Attack");
                GuiButton(ScreenRects[R_BTN_DEFEND], "Defend");
                GuiButton(ScreenRects[R_BTN_USE_ITEM], "Use Item");
                GuiSetState(prevState);
            }

            BeginScissorMode((int)ScreenRects[R_LOG_BOX].x + 1, (int)ScreenRects[R_LOG_BOX].y + 1, (int)ScreenRects[R_LOG_BOX].width - 2, (int)ScreenRects[R_LOG_BOX].height - 2);
            float logY = ScreenRects[R_LOG_BOX].y + 5.0f - combatHandler->logScrollOffset;

            for (int i = 0; i < combatHandler->log.size(); ++i) {
                if (i != combatHandler->log.size() - 1) {
                    // For all but the last log entry, use gray color
                    DrawText(TextFormat(". %s", combatHandler->log[i].c_str()), (int)(ScreenRects[R_LOG_BOX].x + 10), (int)logY, FONT_SIZE_LOG, GRAY);
                }else 
                {
                    DrawText(TextFormat("> %s", combatHandler->log[i].c_str()), (int)(ScreenRects[R_LOG_BOX].x + 10), (int)logY, FONT_SIZE_LOG, BLACK);
                }
                logY += LOG_LINE_HEIGHT;
            }
                
            EndScissorMode();
            break;
        }
        
        case GameState::DIALOGUE:
        {
            //TODO: Add dialogue rendering
            break;
        }
        
        case GameState::PAUSE_MENU:
        {
            //TODO: Add pause menu rendering
            break;
        }
    }
}
