#include "screenManager.h"
#include "raylib.h"
#include "raygui.h"
#include <cmath>


// Example screen-local textures (replace with your asset-table results)
static Texture2D gMenuBG{};
static Texture2D gProgramTitle{};
static Texture2D gSelectBG{};
static Texture2D gGameplayBG{};
static Texture2D gPlayer{};
static Texture2D gEnemy{};

ScreenManager::ScreenManager(ScreenState initial)
: currentScreen(initial) {}

ScreenManager::~ScreenManager(){
    // Ensure current screen is cleaned up
    exitScreen(currentScreen);
}

void ScreenManager::init() {
    // Load the initial screen resources
    enterScreen(currentScreen);
}

void ScreenManager::changeScreen(ScreenState newScreen){
    if (newScreen == currentScreen) return;
    exitScreen(currentScreen);
    currentScreen = newScreen;
    enterScreen(currentScreen);
}

ScreenState ScreenManager::getCurrentScreen() const{
    return currentScreen;
}

void ScreenManager::update(float dt) {
    switch (currentScreen) {
        case ScreenState::MAIN_MENU:
            // handle menu input (e.g., raygui buttons)
            break;
        case ScreenState::CHARACTER_SELECT:
            // handle selection logic
            break;
        case ScreenState::GAMEPLAY:
            // movement, circle hotspots, etc.
            break;
        case ScreenState::SAVE_QUIT:
            // confirm/save flow
            break;
    }
}

void ScreenManager::render() {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (currentScreen) {
        case ScreenState::MAIN_MENU:{
            DrawTexture(gMenuBG, 0, 0, WHITE);
            //draw program title in the top center using getScreenWidth and getScreenHeight
            DrawTexture(gProgramTitle, (GetScreenWidth() - gProgramTitle.width) / 2,-150, WHITE);
            // draw raygui buttons
            // change button style into a sorta distressed look
            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x646464FF); // dark gray border in hex
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x969696FF); // medium gray border in hex
            GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xC8C8C8FF);// light gray border in hex
            //GuiSetStyle(BUTTON, BORDER_WIDTH, 4); // thicker border
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x000000B4); // semi-transparent black in hex
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x323232C8); // darker gray in hex
            GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x646464DC); // lighter gray in hex
            // change text color to white
            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
            GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
            GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
            // change font size
            GuiSetStyle(DEFAULT, TEXT_SIZE, 56); // larger text size
            // create a large button in the center of the screen
            Rectangle startBtn = {(GetScreenWidth() - 600) / 2, (GetScreenHeight() - 70) / 2 + 100, 600, 70};
            if (GuiButton(startBtn, "Start Game")) {
                changeScreen(ScreenState::CHARACTER_SELECT);
            }
            

            //create an exit button below the start button
            Rectangle exitBtn = {(GetScreenWidth() - 600) / 2, (GetScreenHeight() - 70) / 2 + 200, 600, 70};
            if (GuiButton(exitBtn, "Exit Game")) {
                CloseWindow(); // Close the window and exit

            }
            break;
        }
        case ScreenState::CHARACTER_SELECT:
            DrawTexture(gSelectBG, 0, 0, WHITE);
            break;
        case ScreenState::GAMEPLAY:
            DrawTexture(gGameplayBG, 0, 0, WHITE);
            // draw player/enemy at positions
            DrawTexture(gPlayer,  200, 300, WHITE);
            DrawTexture(gEnemy,   500, 300, WHITE);
            break;
        case ScreenState::SAVE_QUIT:
            // simple overlay/text
            break;
    }

    EndDrawing();
}

void ScreenManager::enterScreen(ScreenState s) {
    switch (s) {
        case ScreenState::MAIN_MENU:
            gMenuBG = LoadTexture("../assets/images/UI/startMenuBg.png");
            gProgramTitle = LoadTexture("../assets/images/UI/gameTitle.png");
            break;
        case ScreenState::CHARACTER_SELECT:
            gSelectBG = LoadTexture("../assets/images/UI/WJFzrOG.png");
            break;
        case ScreenState::GAMEPLAY:
            // In your version, these paths come from asset tables by id
            //gGameplayBG = LoadTexture("assets/scenes/EnvironButtons.png");
            //gPlayer     = LoadTexture("assets/characterSprites/RatKing.png");
            //gEnemy      = LoadTexture("assets/enemies/FratBro1.png");
            break;
        case ScreenState::SAVE_QUIT:
            // nothing special to load
            break;
    }
}

void ScreenManager::exitScreen(ScreenState s) {
    switch (s) {
        case ScreenState::MAIN_MENU:
            if (gMenuBG.id) UnloadTexture(gMenuBG), gMenuBG = {};
            break;
        case ScreenState::CHARACTER_SELECT:
            if (gSelectBG.id) UnloadTexture(gSelectBG), gSelectBG = {};
            break;
        case ScreenState::GAMEPLAY:
            if (gGameplayBG.id) UnloadTexture(gGameplayBG), gGameplayBG = {};
            if (gPlayer.id)     UnloadTexture(gPlayer),     gPlayer = {};
            if (gEnemy.id)      UnloadTexture(gEnemy),      gEnemy = {};
            break;
        case ScreenState::SAVE_QUIT:
            break;
    }
}


