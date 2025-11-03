#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "raylib.h"
#include <cmath>




//@author: Edwin Baiden
//@brief: Enum representing different screen states (main menu, character select, gameplay, save & quit).
//@version: 1.0
enum class ScreenState {MAIN_MENU, CHARACTER_SELECT, GAMEPLAY, SAVE_QUIT};

//@author: Edwin Baiden
//@brief: Class to manage screen states and transitions
//@version: 1.0
class ScreenManager 
{
    private:
        ScreenState currentScreen; // Current active screen
        //bool pendingScreenChange = false; // Flag to indicate if a screen change is pending
        void enterScreen(ScreenState screen); // Handle entering a new screen loading resources
        void exitScreen(ScreenState screen); // Handle exiting a screen unloading resources


    public:
        explicit ScreenManager(ScreenState initial = ScreenState::MAIN_MENU); // noexcept; // Constructor with default initial screen
        ~ScreenManager(); //noexcept; // Destructor
        void init(); // Initialize the screen manager
        void changeScreen(ScreenState newScreen); // Request a screen change
        [[nodiscard]] ScreenState getCurrentScreen() const; // noexcept; // Get the current screen state
        void update(float deltaTime); // Update the current screen with delta time
        void render(); // Render the current screen
        //void closeWindow(); // Close the window and clean up resources
};

namespace animation {

    inline float saturate (float blendFactor) 
    {
        if (blendFactor < 0.0f) 
            return 0.0f;
        else if (blendFactor > 1.0f) 
            return 1.0f;
        else 
            return blendFactor;
    }

    inline float slopeInt(float start, float end, float blendFactor) 
    {
        blendFactor = saturate(blendFactor);
        return start + (end - start) * blendFactor;
    }

    inline Vector2 slopeInt(const Vector2& start, const Vector2& end, float blendFactor) 
    {
        blendFactor = saturate(blendFactor);
        return {start.x + (end.x - start.x) * blendFactor,
                start.y + (end.y - start.y) * blendFactor};
    }

    inline Color slopeInt(const Color& start, const Color& end, float blendFactor) 
    {
        blendFactor = saturate(blendFactor);
        return {
            static_cast<unsigned char>(start.r + (end.r - start.r) * blendFactor),
            static_cast<unsigned char>(start.g + (end.g - start.g) * blendFactor),
            static_cast<unsigned char>(start.b + (end.b - start.b) * blendFactor),
            static_cast<unsigned char>(start.a + (end.a - start.a) * blendFactor)
        };
    }

    inline float easeInQuad(float blendFactor)
    {
        blendFactor = saturate(blendFactor);
        return
        {
            1.f -(1.f - blendFactor) * (1.f - blendFactor)
        };
    }

    inline float easeInOutCubic(float blendFactor)
    {
        blendFactor = saturate(blendFactor);
        if (blendFactor < 0.5f)
        {
            return 4.f * blendFactor * blendFactor * blendFactor;
        }
        else
        {
            float f = ((2.f * blendFactor) - 2.f);
            return 0.5f * f * f * f + 1.f;
        }
    }

}





#endif // SCREENMANAGER_H