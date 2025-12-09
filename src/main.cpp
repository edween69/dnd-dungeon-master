/* @authors: Andrew, Edwin, Sebastian
    @date: 10/5/2025
    @brief:
*/
// main.cpp


#include "raylib.h"
#include "screenManager.h"

int main() 
{
    #if defined(__APPLE__) ||  defined (__MACH__) ||defined(__linux__)
        SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    #else
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    #endif

    InitAudioDevice();// Initialize audio device

    

    InitWindow(1280, 720, "The Last Lift"); // Windowed mode for development
    
    
    
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

