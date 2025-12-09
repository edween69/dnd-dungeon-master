/*===================================== screenManager.h ======================================
    Project: TTRPG Game ?
    Subsystem: Screen Manager
    Primary Author: Edwin Baiden
    Description: This file declares the ScreenManager class and GameManager class to manage different game screens using raylib and raygui for rendering and GUI
                 elements. It also declares enums for screen states and game states as well as namespace functions for simple animations

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
                      GameManager with an initial game state.

                    - GameManager::~GameManager(): Destructor to clean up remaining resources when the
                      GameManager is destroyed

                    - void GameManager::changeGameState(GameState newState): Request a game state change.

                    - GameState GameManager::getCurrentGameState() const: Get the current game state

                    - void GameManager::update(float dt): Update the current game state with delta time.

                    - void GameManager::render(): Render the current game state.

                    - void GameManager::enterGameState(GameState state): Handle entering a new game
                      state by loading resources.

                    - void GameManager::exitGameState(GameState state): Handle exiting a game state
                      by unloading resources
                
                Animation Namespace: Holds simple animation interpolation functions. Used for GUI animations.

                    - float saturate(float blendFactor): Clamp blend factor between 0 and 1.

                    - float slopeInt(float start, float end, float blendFactor): Linear interpolation
                      between two float values.

                    - Vector2 slopeInt(const Vector2& start, const Vector2& end, float blendFactor):
                      Linear interpolation between two Vector2 values.

                    - Color slopeInt(const Color& start, const Color& end, float blendFactor):
                      Linear interpolation between two Color values.

                    - float easeInQuad(float blendFactor): Quadratic ease-in function

                    - float easeInOutCubic(float blendFactor): Cubic ease-in-out function

                    SlopeInt functions are used to smoothly interpolate properties like position, color, and size over time.

                    EaseInQuad and EaseInOutCubic: used to control the acceleration or deceleration of a property over time,
                    creating smooth transitions for GUI elements.
                    EaseInQuad starts slowly and accelerates, while EaseInOutCubic starts and ends slowly with a faster middle phase.
                    Found info about these functions and how to implement them here: https://easings.net/
*/

//======================= STANDARD LIBRARY INCLUDES =======================
#include <cmath>       // for std::exp, fmodf
#include <map>         // for battleWon map
#include <algorithm>   // for std::clamp, std::max, std::min

//======================= PROJECT INCLUDES =======================
#include "raylib.h"    // used for screen rendering 
#include "characters.h"// for Character class and related definitions
#include "combat.h"    // to manage combat state and perform actions
#include "raygui.h"    // for GUI elements


//=============== HEADER GUARD ===============
#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

//======================== SCREEN AND GUI CONSTANTS & MACROS ========================

// Tricking the game into thinking its running at 1920x1080 no matter what the actual window size is, for scaling purposes
#define GAME_SCREEN_WIDTH 1920
#define GAME_SCREEN_HEIGHT 1080

//Gets the center of the screen
#define SCREEN_CENTER_X ((float)GAME_SCREEN_WIDTH / 2.0f)
#define SCREEN_CENTER_Y ((float)GAME_SCREEN_HEIGHT / 2.0f)

//Macro to the starting X position of text if it were to be centered in a rectangle
//Used macro instead of function so stuff is done in line and no extra overhead is used during runtime
#define CENTER_TEXT_X(rect, txt, size) \
        (int)((rect).x + (rect).width / 2.0f - MeasureText((txt), (size)) / 2.0f)

//Macro to the starting Y position of text if it were to be centered in a rectangle
//Used macro instead of function so stuff is done in line and no extra overhead is used during runtime
#define CENTER_TEXT_Y(rect, size) \
        (int)((rect).y + (rect).height / 2.0f - (size) / 2.0f)


// Macro to get the centered X position of an element w respect to the screen width
#define CENTERED_X(width) (((float)GAME_SCREEN_WIDTH - (width)) / 2.0f)

//Macro to get the centered Y position of an element w respect to the screen height
#define CENTERED_Y(height) (((float)GAME_SCREEN_HEIGHT - (height)) / 2.0f)



//====================== SOUND INDICES ======================
#define SND_SELECT 0       //Select sound when navigating character select menu
#define SND_HIT 1          //Sound when player or enemy hits during combat
#define SND_HEAL 2         //Sound when player heals during combat
#define SND_ZOM_DEATH 3    //Sound when enemy/player dies during combat
#define SND_ZOM_GROAN 4    //Sound when player walks in on a zombie encounter
#define TOTAL_SOUNDS 5

//================= NERD FONT ICON CODEPOINTS ===================
// Got these values from nerdfonts.com/cheat-sheet
#define ICON_SWORD 0xF04E5
#define ICON_BOW_ARROW 0xF1841
#define ICON_POISON 0xF0BC7
#define ICON_FIRE 0xF0238
#define ICON_ARROW_DOWN 0xF063
#define ICON_ARROW_UP 0xF062
#define ICON_PLUS 0xF0415
#define ICON_SNAIL 0xF1677
#define ICON_LIGHTNING 0xF140B
#define ICON_SHIELD 0xF0498
#define ICON_PAUSE 0xF03E4

//======================== MAIN MENU SCREEN CONSTANTS & MACROS =========================
#define MAIN_BUTTON_WIDTH 600.0f 
#define MAIN_BUTTON_HEIGHT 70.0f
#define MAIN_BUTTON_OFFSET_Y 100.0f // Offset form vertical center
#define MAIN_BUTTON_SPACING 100.0f  // Spacing between buttons


//========================= CHARACTER SELECTION SCREEN CONSTANTS & MACROS =========================
#define MAX_CHAR_CARDS 4 // Number of character cards available (Student, Rat, Professor, Atilla)

// Character card dimensions
#define CHARACTER_CARD_WIDTH 300.0f 
#define CHARACTER_CARD_HEIGHT 400.0f

//Spacing between character cards in both docked and default positions
#define CHARACTER_CARD_SPACING 50.0f
#define CHARACTER_DOCK_SPACING 90.0f

// Starting position for docked character cards
#define CHARACTER_DOCK_X ((float)GAME_SCREEN_WIDTH - CHARACTER_CARD_WIDTH - 40.0f)
#define CHARACTER_DOCK_Y_START ((float)GAME_SCREEN_HEIGHT - CHARACTER_CARD_HEIGHT - 300.0f)

// Play button dimensions and position
#define PLAY_BTN_WIDTH 400.0f
#define PLAY_BTN_HEIGHT 60.0f
#define PLAY_BTN_OFFSET_Y 36.0f // Offset from bottom of screen

// Character select screen rects (indices)
#define R_PLAY_BTN 0             // Play button rectangle index
#define R_INFO_BOX 1             // Info box rectangle index
#define R_SELECT_INNER_OUTLINE 2 // Inner outline rectangle index (when a character is selected)
#define R_SELECT_OUTER_OUTLINE 3 // Outer outline rectangle index (when a character is selected)
#define R_HOVER_INFO_POS 4       // Hover info position rectangle index (when hovering over a character card)

//======================== INTRO CRAWL SCREEN CONSTANTS & MACROS =========================
#define INTRO_CRAWL_SPEED 30.0f        // Speed of the intro crawl
#define INTRO_CRAWL_START_Y (float)GAME_SCREEN_HEIGHT // Starting Y position for the intro crawl (bottom of the screen)
#define INTRO_CRAWL_END_Y -1400        // Ending Y position for the intro crawl (off the top of the screen)
#define INTRO_CRAWL_FONT_SIZE 28       // Font size for the intro crawl text
#define INTRO_CRAWL_LINE_HEIGHT 34     // spacing between lines in the intro crawl

//========================= GAMEPLAY SCREEN CONSTANTS & MACROS =========================

// Text Sizes
#define FONT_SIZE_NAME 30
#define FONT_SIZE_HP 20
#define FONT_SIZE_BTN 30
#define FONT_SIZE_LOG 20
#define LOG_LINE_HEIGHT 24

// ================== Exploration constants and macros ==================

// Background texture indices in the exploration screen
#define TEX_ENTRANCE 0
#define TEX_EXIT 1
#define TEX_FRONT_OFFICE 2
#define TEX_EAST_HALLWAY_TOWARD 3
#define TEX_EAST_HALLWAY_AWAY 4
#define TEX_WEST_HALLWAY_TOWARD 5
#define TEX_WEST_HALLWAY_AWAY 6
#define TEX_CLASSROOM_1 7
#define TEX_CLASSROOM_2 8
#define TEX_CLASSROOM_3 9
#define TEX_IN_OFFICE 10
#define TEX_BATH_MEN 11
#define TEX_BATH_WOM 12
#define TEX_OUTSIDE 13

//Item texture indices in the exploration screen
#define TEX_KEY_1 14
#define TEX_KEY_2 15
#define TEX_HEALTH_POTION 16
#define TEX_BAT 17

//Other texture indices in the exploration screen
#define TEX_ARROW 18
#define TEX_MINIMAP 19
#define TEX_TURTLE 20
#define TOTAL_EXP_TEX 21

//Minimap specific macros
#define MINIMAP_SIZE 300.0f
#define MINIMAP_MARGIN 20.0f
#define MINIMAP_BORDER 4.0f
#define MINIMAP_X ((float)GAME_SCREEN_WIDTH - MINIMAP_SIZE - MINIMAP_MARGIN)   // X position of the minimap
#define MINIMAP_Y ((float)GAME_SCREEN_HEIGHT - MINIMAP_SIZE - MINIMAP_MARGIN)  // Y position of the minimap

//Arrow rotation macro function (used this instead of an actual function because macros are inline and avoid call overhead)
#define ARROW_ROTATION(dir) \
        ((dir) == DOWN ? 180.0f : (dir) == LEFT ? -90.0f : (dir) == RIGHT ? 90.0f : 0.0f)


//Exploration Pause rectangles (indices; used in pause menu, reused same indices as combat pause menu)
#define R_EXP_PAUSE_BTN 19
#define R_EXP_PAUSE_BG_OVERLAY 20
#define R_EXP_PAUSE_PANEL 21
#define R_EXP_BTN_RESUME 22
#define R_EXP_BTN_SAVE_EXIT 23
#define R_EXP_BTN_QUIT_NO_SAVE 24

// ================== Combat constants and macros ==================

// Combat screen rectangles (indices)
#define R_PLAYER_NAME 0          // Player name box rectangle index
#define R_ENEMY_NAME 1           // Enemy name box rectangle index
#define R_PLAYER_PANEL 2         // Player panel(Big) rectangle index
#define R_ENEMY_PANEL 3          // Enemy panel(Big) rectangle index
#define R_PLAYER_HP_BG 4         // Player HP background rectangle index
#define R_PLAYER_HP_FG 5         // Player HP foreground rectangle index
#define R_ENEMY_HP_BG 6          // Enemy HP background rectangle index
#define R_ENEMY_HP_FG 7          // Enemy HP foreground rectangle index
#define R_PLAYER_STATUS 8        // Player status rectangle index
#define R_ENEMY_STATUS 9         // Enemy status rectangle index
#define R_BOTTOM_PANEL 10        // Bottom panel rectangle index
#define R_BTN_ATTACK 11          // Attack button rectangle index
#define R_BTN_DEFEND 12          // Defend button rectangle index
#define R_BTN_USE_ITEM 13        // Use item button rectangle index
#define R_LOG_BOX 14             // Log box rectangle index
#define R_ATTACK_MENU 15         // Attack menu rectangle index
#define R_MELEE_BTN 16           // Melee button rectangle index
#define R_RANGED_BTN 17          // Ranged button rectangle index
#define R_ITEM_MENU 18           // Item menu rectangle index
#define R_PAUSE_BTN 19           // Pause button rectangle index (Same as exploration pause button index)
#define R_PAUSE_BG_OVERLAY 20    // Pause background overlay rectangle index (Same as exploration pause bg overlay index)
#define R_PAUSE_PANEL 21         // Pause panel rectangle index (Same as exploration pause panel index)
#define R_BTN_RESUME 22          // Resume button rectangle index (Same as exploration resume button rectangle index)
#define R_BTN_SAVE_EXIT 23       // Save and exit button rectangle index (Same as exploration save and exit button rectangle index)
#define R_BTN_QUIT_NO_SAVE 24    // Quit without saving button rectangle index (Same as exploration quit without saving button rectangle index)

//Health Bar Macro Function(once again used macro for speed)
#define HEALTH_BAR_WIDTH(rectBg, cur, max) \
        ((float)(rectBg).width * ((float)(cur) / (float)(max))) // Health bar width scaled as (current health / max health) * background width


// Colors for UI in Combat Screen
#define COL_NAME_BAR      Color{8, 8, 12, 255} 
#define COL_BOTTOM_PANEL  Color{112, 120, 128, 255}
#define COL_STATUS_PANEL  Color{55, 61, 57, 220}
#define COL_STATUS_INNER  Color{91, 94, 92, 255}
#define COL_LOG_BOX       Color{167, 171, 170, 255}
#define COL_BUTTON        Color{68, 74, 72, 255}
#define COL_HP_BG         Color{60, 15, 20, 255}
#define COL_HP_FG         Color{190, 50, 60, 255}

// =========================== Pause Menu Constants and Macros ===========================

//Pause menu main rectangle dimensions
#define PAUSE_PANEL_WIDTH 400.0f 
#define PAUSE_PANEL_HEIGHT 300.0f 

// Sizing and Spacing for buttons
#define PAUSE_BTN_WIDTH 300.0f
#define PAUSE_BTN_HEIGHT 60.0f
#define PAUSE_BTN_SPACING 20.0f
#define PAUSE_PANEL_X (((float)GAME_SCREEN_WIDTH - PAUSE_PANEL_WIDTH) / 2.0f) 
#define PAUSE_PANEL_Y (((float)GAME_SCREEN_HEIGHT - PAUSE_PANEL_HEIGHT) / 2.0f)
#define PAUSE_BTN_X (PAUSE_PANEL_X + (PAUSE_PANEL_WIDTH - PAUSE_BTN_WIDTH) / 2.0f)



// ======================== GAME AND SCREEN STATE ENUMS ========================

//@author: Edwin Baiden
//@brief: Enum representing different screen states (main menu, character select, gameplay, save & quit).
//@version: 1.0
enum class ScreenState { MAIN_MENU, CHARACTER_SELECT, INTRO_CRAWL, GAMEPLAY };

//author: Edwin Baiden
//@brief: Enum representing different game states (exploration, combat, dialogue, pause menu).
//@version: 1.0
enum class GameState { EXPLORATION, COMBAT, PAUSE_MENU };

//@author: Edwin Baiden
//@brief: Class to manage screen states and transitions
//@version: 1.0

//======================= SCREEN MANAGER CLASS DEFINITION =======================
class ScreenManager 
{
private:
    ScreenState currentScreen; // Current active screen state
    
    // VIRTUAL RESOLUTION VARIABLES
    RenderTexture2D target; // The texture we render the game onto
    float scale; // The scale factor to fit the window
    Vector2 offset;// The offset to center the game in the window

    void enterScreen(ScreenState screen); // Handle entering a new screen loading resources
    void exitScreen(ScreenState screen);  // Handle exiting a screen unloading resources

public:
    explicit ScreenManager(ScreenState initial = ScreenState::MAIN_MENU); // Constructor with default initial screen
    ~ScreenManager(); // Destructor
    void init(); // Initialize the screen manager
    void changeScreen(ScreenState newScreen); // Request a screen change
    /*
        - [[nodiscard]]: Makes sure that the returned is actually used by the caller(helps catch bugs where return value is ignored)
        - const: Ensures that the function is a read-only operation
    */
    [[nodiscard]] ScreenState getCurrentScreen() const; // Get the current screen state used 
    void update(float deltaTime); // Update the current screen with delta time
    void render(); // Render the current screen

    // Helper to convert real mouse coordinates to virtual game coordinates
    Vector2 GetVirtualMousePosition();
};

//======================= GAME MANAGER CLASS DEFINITION =======================
//@author: Edwin Baiden
//@brief: Class to manage game states and transitions. Helper for ScreenManager once gameplay is started. Literally the same as ScreenManager but for game states
//@version: 1.0
class GameManager {
private:
    GameState currentGameState; // Current active game state
    GameState nextGameState = GameState::EXPLORATION; // Next game state to transition to
    GameState prevGameState = GameState::EXPLORATION; // Previous game state before transition
    CombatHandler* combatHandler = nullptr; // Combat handler to manage combat state
    float sceneTransitionTimer = 0.0f; // Timer for scene transitions

public:
    explicit GameManager(GameState initial = GameState::EXPLORATION);
    ~GameManager(); // Destructor
    void changeGameState(GameState newState); // Request a game state change
    [[nodiscard]] GameState getCurrentGameState() const; // Get the current game state (used [[nodiscard]] to ensure return value is used by caller; used const to make it read-only)
    void update(float deltaTime); // Update the current game state with delta time
    void render(); // Render the current game state
    void enterGameState(GameState state); // Handle entering a new game state loading resources
    void exitGameState(GameState state); // Handle exiting a game state unloading resources
    bool backToMainMenu = false; // Flag to indicate returning to main menu
};

//@author: Edwin Baiden
//@brief: Simple enum for direction. mostly used for arrows in exploration
//@version: 1.0
enum ArrowDirection { NONE = -1, UP, DOWN, LEFT, RIGHT };

// Items on in the area (Keys, Potions)
//@author: Edwin Baiden
//@brief: Struct defining an item that sits on the floor in exploration mode. basicly stuff u can pick up
//@version: 1.0
struct SceneItem {
    std::string itemName; // Name to match inventory string (e.g. "Key 1")
    std::string hoverText; // Text to display on mouseover (tells player what it is)
    Rectangle clickArea; // Click zone on screen (hitbox)
    int textureIndex; // Index in Global ScreenTextures array (dont mess this up)
    bool requiresVictory; // True if item only appears after room battle is won (loot drop)
};

// Navigation points (Doors, Hallway Arrows)
//@author: Edwin Baiden
//@brief: Struct for navigation arrows. Tells the game where to go next.
//@version: 1.0
struct SceneArrow {
    Rectangle clickArea; // clickable area on screen
    ArrowDirection dir; // which way is it pointing
    int targetSceneIndex; // where does this arrow take u
    bool isEnabled; // is the arrow clickable?
    std::string hoverText; // text when mouse hovers
    std::string requiredKeyName;// If not empty, checks player inventory for this string (locked doors)
};

// The Room Container
//@author: Edwin Baiden
//@brief: Class that holds all data for a specific room or "scene" in the game. basicly a container for all the stuff in a room
//@version: 1.0
class GameScene 
{
public:
    std::string sceneName;               // Name displayed on minimap
    int textureIndex;                    // Background texture index for the room
    std::string environmentTexture = ""; // Environment texture file path (for combat background loading)
    
    Vector2 minimapCoords;  // 0.0-1.0 Position on map image (normalized coords)
    float minimapRotation;  // Rotation of the turtle icon on minimap
    
    std::vector<SceneArrow> sceneArrows; // List of nav arrows
    std::vector<SceneItem> sceneItems;   // List of items in room
        
    // Combat Trigger stuff
    bool hasEncounter = false; // Does this room have a fight?
    int encounterID = -1;      // Which enemy is it?
        
    // Coordinates for drawing combat elements (took a while to calibrate these)
    float combatBgX = 0.0f;
    float combatBgY = 0.0f;

    float playerCharX = 0.0f;
    float playerCharY = 0.0f;
    
    float enemyCharX = 0.0f;
    float enemyCharY = 0.0f;

    Vector2 playerScale = {0.0f, 0.0f}; // size of player in this room
    Vector2 enemyScale = {0.0f, 0.0f};  // size of enemy in this room
};

void InitGameScenes(Character* playerCharacter); // Initialize all game scenes

// ========================= ANIMATION NAMESPACE DEFINITION =========================

//@brief: Namespace containing simple animation interpolation functions for GUI elements
//@version: 1.0
//@author: Edwin Baiden
// I like using the namespace as way way to organize related functions together without creating a class for them
namespace animation {

    // Inline: Since this header file is used in both screenManager.cpp and main.cpp
    // making these functions inline prevents multiple definition errors during linking.

    //@brief: Clamp blend factor between 0 and 1
    //@param blendFactor - The input blend factor
    //@return: The clamped blend factor
    //@version: 1.0
    //@author: Edwin Baiden
    inline float saturate (float blendFactor) 
    {
        if (blendFactor < 0.0f) 
            return 0.0f; //if less than 0 return 0
        else if (blendFactor > 1.0f) 
            return 1.0f; //if greater than 1 return 1
        else 
            return blendFactor;
    }

    //@brief: Linear interpolation between two float values (Overloaded for float, Vector2, and Color)
    //@param start - The starting float value
    //@param end - The ending float value
    //@param blendFactor - The blend factor (0 to 1)
    //@return: The interpolated float value
    //@version: 1.0
    //@author: Edwin Baiden
    inline float slopeInt(float start, float end, float blendFactor) 
    {
        blendFactor = saturate(blendFactor); // Clamp blend factor between 0 and 1
        return start + (end - start) * blendFactor; //simple y= mx+b formula for linear interpolation
    }

    //@brief: Linear interpolation between two Vector2 values(Overloaded for float, Vector2, and Color)
    //@param start - The starting Vector2 value
    //@param end - The ending Vector2 value
    //@param blendFactor - The blend factor (0 to 1)
    //@return: The interpolated Vector2 value
    inline Vector2 slopeInt(const Vector2& start, const Vector2& end, float blendFactor) 
    {
        blendFactor = saturate(blendFactor);// Clamp blend factor between 0 and 1
        return {start.x + (end.x - start.x) * blendFactor, start.y + (end.y - start.y) * blendFactor}; //same as float version but for both x and y components
    }

    //@brief: Linear interpolation between two Color values(Overloaded for float, Vector2, and Color)
    //@param start - The starting Color value
    //@param end - The ending Color value
    //@param blendFactor - The blend factor (0 to 1)
    //@return: The interpolated Color value
    //@version: 1.0
    //@author: Edwin Baiden
    inline Color slopeInt(const Color& start, const Color& end, float blendFactor) 
    {
        blendFactor = saturate(blendFactor); // Clamp blend factor between 0 and 1
        // Casting into an unsigned char since it goes from 0 to 255
        return {
            static_cast<unsigned char>(start.r + (end.r - start.r) * blendFactor), // Interpolating each color channel separately
            static_cast<unsigned char>(start.g + (end.g - start.g) * blendFactor),
            static_cast<unsigned char>(start.b + (end.b - start.b) * blendFactor),
            static_cast<unsigned char>(start.a + (end.a - start.a) * blendFactor)
        };
    }

    //@brief: Quadratic ease-in function for smooth acceleration of a property over time
    //@param blendFactor - The input blend factor
    //@return: The eased blend factor float value
    //@version: 1.0
    //@author: Edwin Baiden
    inline float easeInQuad(float blendFactor)
    {
        blendFactor = saturate(blendFactor); // Clamp blend factor between 0 and 1
        return ((blendFactor * blendFactor) - (2 * blendFactor)); // Quadratic ease-in formula (1-(1-t)^2)
    }

    //@brief: Cubic ease-in-out function for smooth acceleration and deceleration of a property over time
    //@param blendFactor - The input blend factor
    //@return: The eased blend factor float value
    //@version: 1.0
    //@author: Edwin Baiden
    inline float easeInOutCubic(float blendFactor)
    {
        blendFactor = saturate(blendFactor); // Clamp blend factor between 0 and 1
        if (blendFactor < 0.5f) 
        {
            return 4.f * blendFactor * blendFactor * blendFactor; // First half cubic ease-in (4t^3)
        }
        else
        {
            float f = ((2.f * blendFactor) - 2.f); // Adjusted t value for second half (making reading easier)
            return 0.5f * f * f * f + 1.f; // Second half cubic ease-out ((1/2)((2t-2)^3 + 1)
        }
    }


    // @brief: Sine wave pulse function for oscillating animations
    //@param: amplitudeFactor - Amplitude of the sine wave
    //@param: speedFactor - Speed of the sine wave
    //@param: blendFactor - The input blend factor
    //@return: The sine wave value
    //@version: 1.0
    inline float sinPulse(float amplitudeFactor, float speedFactor, float blendFactor)
    {
        return amplitudeFactor * sinf(speedFactor * saturate(blendFactor)); // Sine wave formula for pulsing effect (A * sin(Bt))
    }
}


#endif //SCREENMANAGER_H
