#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H



//@author: Edwin Baiden
//@brief: Enum representing different screen states (main menu, character select, gameplay, save & quit).
//@version: 1.0
enum class ScreenState {MAIN_MENU, CHARACTER_SELECT, GAME_PLAY, SAVE_QUIT};

//@author: Edwin Baiden
//@brief: Class to manage screen states and transitions
//@version: 1.0
class ScreenManager 
{
    private:
        ScreenState currentScreen;
        bool pendingScreenChange = false;


    public:
        explicit ScreenManager(ScreenState initial = ScreenState::MAIN_MENU);
        void init();
        void changeScreen(ScreenState newScreen);
        ScreenState getCurrentScreen() const;
        void update(float deltaTime);
        void render();
        void closeWindow();
};



#endif // SCREENMANAGER_H