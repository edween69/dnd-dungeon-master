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

//Including necessary libraries and headers
#include "raylib.h"
#include <cmath>
#include "characters.h"
#include "raygui.h"
#include "rng.h"
#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

// ======================== GAME AND SCREEN STATE ENUMS ========================

//@author: Edwin Baiden
//@brief: Enum representing different screen states (main menu, character select, gameplay, save & quit).
//@version: 1.0
enum class ScreenState {MAIN_MENU, CHARACTER_SELECT,INTRO_CRAWL, GAMEPLAY, SAVE_QUIT};

//author: Edwin Baiden
//@brief: Enum representing different game states (exploration, combat, dialogue, pause menu).
//@version: 1.0
enum class GameState { EXPLORATION, COMBAT, DIALOGUE, PAUSE_MENU };

//@author: Edwin Baiden
//@brief: Class to manage screen states and transitions
//@version: 1.0

//======================= SCREEN MANAGER CLASS DEFINITION =======================
class ScreenManager 
{
    private:
        ScreenState currentScreen; // Current active screen state
        void enterScreen(ScreenState screen); // Handle entering a new screen loading resources
        void exitScreen(ScreenState screen); // Handle exiting a screen unloading resources


    public:
        explicit ScreenManager(ScreenState initial = ScreenState::MAIN_MENU);// Constructor with default initial screen
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
};

//======================= GAME MANAGER CLASS DEFINITION =======================
//@author: Edwin Baiden
//@brief: Class to manage game states and transitions.Helper for screenManager once gameplay is started. Literally the same as ScreenManager but for game states
//@version: 1.0
class GameManager {
    private:
        GameState currentGameState; // Current active game state
        
    
    public:
        explicit GameManager(GameState initial = GameState::COMBAT); //TODO: REMEMBER TO CHANGE TO EXPLORATION LATER
        ~GameManager(); // Destructor
        void changeGameState(GameState newState); // Request a game state change
        [[nodiscard]] GameState getCurrentGameState() const; // Get the current game state
        void update(float deltaTime); // Update the current game state with delta time
        void render(); // Render the current game state
        void enterGameState(GameState state); // Handle entering a new game state loading resources
        void exitGameState(GameState state); // Handle exiting a game state unloading resources
};

enum ArrowDirection {NONE=-1, UP, DOWN, LEFT, RIGHT};

struct SceneCharacter
{
    std::string id;
    Texture2D texture{};
    Rectangle pos {};
    bool isPlayerCharacter = false;
};

struct SceneArrow
{
    Rectangle pos {};
    ArrowDirection direction;
    int targetSceneIndex = -1;
    bool isEnabled;

};

struct SceneObject
{
    Texture2D texture{};
    Rectangle pos {};
    bool isInteractable = false;
    bool isItem = false;
};
// ========================= GAMESCENE CLASS DEFINITION =========================
//@author: Edwin Baiden
//@brief: Class to represent a game scene (contains game objects, environments, type of scene, positioning, etc.)
//@version: 1.0
class GameScene 
{
    public:
        GameState sceneType;
        std::string sceneName;
    
    private:
        std::string envPath;


};

class CombatScene : public GameScene
{
    public:
        std::vector<SceneCharacter> charactersInScene;
        std::vector<SceneArrow> sceneArrows;
        std::vector<SceneObject> sceneObjects;
};

class ExplorationScene : public GameScene
{
    public:
        std::vector<SceneCharacter> charactersInScene;
        std::vector<SceneArrow> sceneArrows;
        std::vector<SceneObject> sceneObjects;
};

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
        //Casting into an unsinged char since it goes from 0 to 255
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
        return ((blendFactor * blendFactor)- (2*blendFactor));// Quadratic ease-in formula (1-(1-t)^2)
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

}

#endif //SCREENMANAGER_H