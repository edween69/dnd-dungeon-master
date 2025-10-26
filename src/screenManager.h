#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H



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



#endif // SCREENMANAGER_H