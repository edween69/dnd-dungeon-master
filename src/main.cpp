/*  @authors: Andrew, Edwin, Sebastian
    @date: 10/5/2025
    @brief:
*/
// main.cpp


#include "raylib.h"
#include "ScreenManager.h"

int main() {
    InitWindow(1920, 1080, "The Last Lift"); // Windowed mode for development
    //Allowed to be resized for testing layout
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    SetTargetFPS(30); // Set desired frame rate

    ScreenManager sm;     //Defining screen manager object
    sm.init();      // Initialize screen manager this loads to the main menu

    while (!WindowShouldClose()) { 
        sm.update(GetFrameTime());
        sm.render();
    }

    CloseWindow();
    return 0;
}

