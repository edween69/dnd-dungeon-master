#include "screenManager.h"
#include <fstream>
#include <sstream>
#include "characters.h"
#include "raylib.h"
#include "raygui.h"

//Getting character stats data for display
std::ifstream* statsFile = openStartingStatsCSV();
std::istringstream* allStatLines = storeAllStatLines(statsFile);



//@brief: Sets default GUI styles for buttons and text (will be used for resetting styles, if needed).
//@version: 1.0
//@author: Edwin Baiden
void defaultStyles() 
{
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x828282FF); // default gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0xB6B6B6FF); // lighter gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xDADADAFF); // lightest gray border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xE0E0E0FF); // light gray in hex
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0xC4C4C4FF); // medium gray in hex
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0xA8A8A8FF); // darker gray in hex
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0x000000FF); // black text
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0x000000FF); // black text
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0x000000FF); // black text
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // default text size
}

//@brief: Sets GUI styles for the main menu screen (customizes button appearance and text size).
//@version: 1.0
//@author: Edwin Baiden
void startMenuStyles() 
{
    // Set button styles for the main menu
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x646464FF); // dark gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x969696FF); // medium gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xC8C8C8FF); // light gray border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x000000B4); // semi-transparent black in hex
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x323232C8); // darker gray in hex
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x646464DC); // lighter gray in hex
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 56); // larger text size

}

//@brief: Sets GUI styles for the character selection screen (customizes button appearance and text size).
//@version: 1.0
//@author: Edwin Baiden
void playerSelectStyles()
{
    // Set button styles for the character select screen
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x004400FF); // dark green border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x008800FF); // medium green border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0x00CC00FF); // light green border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x000000B4); // semi-transparent black in hex
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x003300C8); // darker blue in hex
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x006600DC); // lighter blue in hex
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36); // medium text size
    //beveled rectangle style for character portraits
    GuiSetStyle(BUTTON, BORDER_WIDTH, 6); // thicker border

    // Clear “disabled” look defined once (used by Play button when locked)
    GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED, 0x555555FF); // gray border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_DISABLED,   0x222222B4); // dark semi-transparent gray in hex
    GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED,   0x888888FF); // light gray text
}



// Example screen-local textures (will be replaced by asset table later once the gameplay screen is implemented)
static Texture2D gMenuBG{};
static Texture2D gProgramTitle{};
static Texture2D gSelectBG{};
static Texture2D gGameplayBG{};
static Texture2D gPlayer{};
static Texture2D gEnemy{};

//@brief: Constructor to initialize the ScreenManager with an initial screen state.
//@version: 1.0
//@author: Edwin Baiden
ScreenManager::ScreenManager(ScreenState initial)
: currentScreen(initial) {}

//@brief: Destructor to clean up resources when the ScreenManager is destroyed.
//@version: 1.0
//@author: Edwin Baiden
ScreenManager::~ScreenManager(){
    // Ensure current screen is cleaned up
    exitScreen(currentScreen);
}

//@brief: Initializes the ScreenManager by loading resources for the initial screen (The Start Menu Screen).
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::init() {
    // Load the initial screen resources
    enterScreen(currentScreen);
}

//@brief: Changes the current screen to a new screen state, handling resource cleanup and loading.
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::changeScreen(ScreenState newScreen){
    if (newScreen == currentScreen) return;
    exitScreen(currentScreen);
    currentScreen = newScreen;
    enterScreen(currentScreen);
}

//@brief: Retrieves the current screen state.
//@version: 1.0
//@author: Edwin Baiden
ScreenState ScreenManager::getCurrentScreen() const{
    return currentScreen;
}

//@brief: Updates the current screen based on the elapsed time (delta time), not used currently. All done in render().
//@version: 1.0
//@author: Edwin Baiden
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

//@brief: Renders the current screen based on the active screen state.
//@version: 1.0
//@author: Edwin Baiden
void ScreenManager::render() 
{
    BeginDrawing(); // Start the drawing phase
    ClearBackground(BLACK);// Clear the background to black

    switch (currentScreen) 
    {
        case ScreenState::MAIN_MENU:
        {

            // Draw the main menu background
            DrawTexture(gMenuBG, 0, 0, WHITE);

            //draw program title in the top center using getScreenWidth and getScreenHeight
            DrawTexture(gProgramTitle, (GetScreenWidth() - gProgramTitle.width) / 2, -150, WHITE);

            startMenuStyles(); // Apply main menu styles

            // Create a start button in the center of the screen
            Rectangle startBtn = {(GetScreenWidth() - 600) / 2.0f, (GetScreenHeight() - 70) / 2.0f + 100, 600, 70};

            if (GuiButton(startBtn, "Start Game")) // If start button is clicked
            {
                changeScreen(ScreenState::CHARACTER_SELECT);
            }

            //create an exit button below the start button
            Rectangle exitBtn = {(GetScreenWidth() - 600) / 2.0f, (GetScreenHeight() - 70) / 2.0f + 200, 600, 70};

            if (GuiButton(exitBtn, "Exit Game")) // If exit button is clicked
            {
                CloseWindow(); // Close the window and exit
            }

            break;
        }
        case ScreenState::CHARACTER_SELECT:
        {
            // Draw the character selection background
            DrawTexture(gSelectBG, 0, 0, WHITE);
            playerSelectStyles(); // Apply character select styles

            float dt = GetFrameTime();
            float t= 1.0f - expf(-10.0f * dt); //smoothing factor for animations
            t= animation::easeInQuad(t);

            static int charSelection = -1; // No selection by default
            int charHovered = -1; // No hover by default

            struct charCard 
            {
                Rectangle defaultRow;
                Rectangle currentAnimationPos;
                Rectangle targetAnimationPos;
                Texture2D texture;
            };

            static charCard characterCards[4];
            characterCards[0].texture = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/south.png");
            characterCards[1].texture = LoadTexture("../assets/images/characters/pc/Rat-Assassin/rotations/south.png");
            characterCards[2].texture = LoadTexture("../assets/images/characters/pc/Professor-Mage/rotations/south.png");
            characterCards[3].texture = LoadTexture("../assets/images/characters/pc/Attila-Brawler/rotations/south.png");
            for (int i = 0; i < 4; ++i) SetTextureFilter(characterCards[i].texture, TEXTURE_FILTER_POINT);
            

            struct charStatsDisplay
            {
                const char* name;
                const char* health;
                const char* strength;
                const char* dexterity;
                const char* constitution;
                const char* wisdom;
                const char* charisma;
                const char* intelligence;
            };

            charStatsDisplay characterStats[4] = 
            {
                {"Student","","","","","","",""},
                {"Rat","","","","","","",""},
                {"Professor","","","","","","",""},
                {"Atilla","","","","","","",""}
            };

        static bool layoutInitialized = false;
            if (!layoutInitialized) 
            {
                float cardWidth = 300.0f;
                float cardHeight = 400.0f;
                float spacing = 50.0f;
                float startX = (GetScreenWidth() - (4 * cardWidth + 3 * spacing)) / 2.0f;
                float targetY = (GetScreenHeight() - cardHeight) / 2.0f;

                for (int i = 0; i < 4; ++i) 
                {
                    float targetX = startX + i * (cardWidth + spacing);
                    characterCards[i].defaultRow = {targetX, targetY, cardWidth, cardHeight};
                    characterCards[i].currentAnimationPos = characterCards[i].defaultRow;
                    characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
                }
                layoutInitialized = true;
            }

            Vector2 mousePoint = GetMousePosition();

            if (charSelection ==-1)
            {
                for (int i = 0; i<4; i++)
                {
                    characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
                }
                
            }else
            {
                float cardWidth = characterCards[charSelection].defaultRow.width;
                float cardHeight = characterCards[charSelection].defaultRow.height;

                Rectangle centeredCard = 
                {
                    (GetScreenWidth() - cardWidth) / 2.0f,
                    characterCards[charSelection].defaultRow.y,
                    cardWidth,
                    cardHeight
                };
                characterCards[charSelection].targetAnimationPos = centeredCard;

                float dockX = GetScreenWidth() - cardWidth - 40.0f;
                float dockY = GetScreenHeight() - cardHeight -300.0f;
                float spacing = 90.0f;

                int dockIndex = 0;
                for (int i = 0; i < 4; ++i) 
                {
                    if (i != charSelection)
                    {
                        characterCards[i].targetAnimationPos = 
                        {
                            dockX,
                            dockY + spacing * dockIndex++,
                            cardWidth,
                            cardHeight
                        };
                        
                    }

                }

        
            }

            for (int i=0; i<4; i++)
            {
                characterCards[i].currentAnimationPos.x = animation::slopeInt(characterCards[i].currentAnimationPos.x, characterCards[i].targetAnimationPos.x, t);
                characterCards[i].currentAnimationPos.y = animation::slopeInt(characterCards[i].currentAnimationPos.y, characterCards[i].targetAnimationPos.y, t);
                DrawTexturePro(
                    characterCards[i].texture,
                    {0.0f, 0.0f, (float)characterCards[i].texture.width, (float)characterCards[i].texture.height},
                    characterCards[i].currentAnimationPos,
                    {0.0f, 0.0f},
                    0.0f,
                    WHITE
                );
                SetTextureFilter(characterCards[i].texture, TEXTURE_FILTER_POINT);

                DrawRectangleLinesEx(characterCards[i].currentAnimationPos, 4.0f, Color{0,68,0,255});

                if (CheckCollisionPointRec(mousePoint, characterCards[i].currentAnimationPos)) charHovered = i;

                if (GuiButton(characterCards[i].currentAnimationPos, characterStats[i].name))
                {
                    if (charSelection == i)
                    {
                        charSelection = -1;
                    }
                    else
                    {
                        charSelection = i;
                    }
                }

                if(charSelection == i)
                {
                    Rectangle r1 = {characterCards[i].currentAnimationPos.x - 6, characterCards[i].currentAnimationPos.y -6 , characterCards[i].currentAnimationPos.width + 12, characterCards[i].currentAnimationPos.height + 12};
                    Rectangle r2 = {characterCards[i].currentAnimationPos.x - 12, characterCards[i].currentAnimationPos.y - 12, characterCards[i].currentAnimationPos.width + 24, characterCards[i].currentAnimationPos.height + 24};

                    DrawRectangleLinesEx(r1, 4, YELLOW);
                    DrawRectangleLinesEx(r2, 2, YELLOW);
                    
                }
                   
            }

            if (charHovered != -1 && charHovered != charSelection)
            {
                const charStatsDisplay& stats = characterStats[charHovered];
                Rectangle cRect = characterCards[charHovered].currentAnimationPos;

                bool placeRight = (cRect.x + cRect.width +260) < (GetScreenWidth());
                Rectangle statsBox = 
                {
                    placeRight ? (cRect.x + cRect.width + 5) : (cRect.x - 5 - 260),
                    cRect.y-250.0f,
                    260.0f,
                    240.0f
                };

                DrawRectangleRec(statsBox, Color{0,40,0,200});
                DrawRectangleLinesEx(statsBox, 3.0f , Color{40,255,80,255});

                float textX = statsBox.x + 10.0f;
                float textY = statsBox.y + 10.0f;
                DrawText (stats.name, textX, textY, 20, WHITE);
                DrawText (stats.health, textX, textY + 25.0f, 16, WHITE);
                DrawText (stats.strength, textX, textY + 50.0f, 16, WHITE);
                DrawText (stats.dexterity, textX, textY + 75.0f, 16, WHITE);
                DrawText (stats.constitution, textX, textY + 100.0f, 16, WHITE);
                DrawText (stats.wisdom, textX, textY + 125.0f, 16, WHITE);
                DrawText (stats.charisma, textX, textY + 150.0f, 16, WHITE);
                DrawText (stats.intelligence, textX, textY + 175.0f, 16, WHITE);

            }

            Rectangle playBtn = {(GetScreenWidth() - 400) / 2.0f, characterCards[charSelection == -1 ? 0 : charSelection].currentAnimationPos.y + characterCards[0].currentAnimationPos.height + 36.0f, 400, 60};

            int prevState = GuiGetState();
            if (charSelection == -1) GuiDisable();
            if (GuiButton(playBtn, "Play Game") && charSelection != -1)
            {
                changeScreen(ScreenState::GAMEPLAY);
            }

            GuiSetState(prevState);

            

            break;
        }
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
            gSelectBG = LoadTexture("../assets/images/UI/startMenuBg.png");
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
