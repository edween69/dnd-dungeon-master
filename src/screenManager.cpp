#include "screenManager.h"
#include <fstream>
#include <sstream>
#include "characters.h"
#include "raylib.h"
#include "raygui.h"

//Definining macros for constants to save runtime memory
#define MAX_CHAR_CARDS      4

#define MAIN_BTN_WIDTH      600.0f
#define MAIN_BTN_HEIGHT      70.0f
#define MAIN_BTN_OFFSET_Y   100.0f
#define MAIN_BTN_SPACING    100.0f

#define CARD_WIDTH          300.0f
#define CARD_HEIGHT         400.0f
#define CARD_SPACING         50.0f

#define DOCK_SPACING         90.0f
#define PLAY_BTN_WIDTH      400.0f
#define PLAY_BTN_HEIGHT      60.0f
#define PLAY_BTN_OFFSET_Y    36.0f

// charCard used for character selection cards
struct charCard
{
    Rectangle defaultRow;
    Rectangle currentAnimationPos;
    Rectangle targetAnimationPos;
    Texture2D texture;
};

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



//Creating dynamic GUI items Textures and Rectangles
static Texture2D* ScreenTextures = nullptr;
static int numScreenTextures = 0;

static Rectangle* ScreenRects = nullptr;
static int numScreenRects = 0;

static charCard* characterCards = nullptr;



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
            DrawTexture(ScreenTextures[0], 0, 0, WHITE);

            //draw program title in the top center using getScreenWidth and getScreenHeight
            DrawTexture(ScreenTextures[1], (GetScreenWidth() - ScreenTextures[1].width) / 2, -150, WHITE);

            startMenuStyles(); // Apply main menu styles

            if (GuiButton(ScreenRects[0], "Start Game")) changeScreen(ScreenState::CHARACTER_SELECT); // If start button is clicked, go to character select screen

            if (GuiButton(ScreenRects[1], "Exit Game")) CloseWindow(); // Close the window and exit
            break;
        }
        case ScreenState::CHARACTER_SELECT:
        {
            // Draw the character selection background
            DrawTexture(ScreenTextures[0], 0, 0, WHITE);
            playerSelectStyles(); // Apply character select styles

            float t = animation::easeInQuad(1.0f - expf(-10.0f * GetFrameTime())); // Animation time factor

            static int charSelection = -1; // No selection by default
            int charHovered = -1; // No hover by default

            // assign textures to cards (textures are already loaded in enterScreen)
            for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            {
                characterCards[i].texture = ScreenTextures[i + 1];
            }

            static bool layoutInitialized = false;
            if (!layoutInitialized) 
            {
                float startX = (GetScreenWidth() - (MAX_CHAR_CARDS * CARD_WIDTH + (MAX_CHAR_CARDS - 1) * CARD_SPACING)) / 2.0f;
                float targetY = (GetScreenHeight() - CARD_HEIGHT) / 2.0f;

                for (int i = 0; i < MAX_CHAR_CARDS; ++i) 
                {
                    float targetX = startX + i * (CARD_WIDTH + CARD_SPACING);
                    characterCards[i].defaultRow = {targetX, targetY, CARD_WIDTH, CARD_HEIGHT};
                    characterCards[i].currentAnimationPos = characterCards[i].defaultRow;
                    characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
                }
                layoutInitialized = true;
            }

            Vector2 mousePoint = GetMousePosition();

            if (charSelection == -1)
            {
                for (int i = 0; i < MAX_CHAR_CARDS; i++)
                {
                    characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
                }
            }
            else
            {
                Rectangle centeredCard = 
                {
                    (GetScreenWidth() - CARD_WIDTH) / 2.0f,
                    characterCards[charSelection].defaultRow.y,
                    CARD_WIDTH,
                    CARD_HEIGHT
                };
                characterCards[charSelection].targetAnimationPos = centeredCard;

                float dockX = GetScreenWidth() - CARD_WIDTH - 40.0f;
                float dockY = GetScreenHeight() - CARD_HEIGHT - 300.0f;

                int dockIndex = 0;
                for (int i = 0; i < MAX_CHAR_CARDS; ++i) 
                {
                    if (i != charSelection)
                    {
                        characterCards[i].targetAnimationPos = 
                        {
                            dockX,
                            dockY + DOCK_SPACING * dockIndex++,
                            CARD_WIDTH,
                            CARD_HEIGHT
                        };
                    }
                }
            }

            // animate & draw cards
            for (int i = 0; i < MAX_CHAR_CARDS; i++)
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

                DrawRectangleLinesEx(characterCards[i].currentAnimationPos, 4.0f, Color{0,68,0,255});

                if (CheckCollisionPointRec(mousePoint, characterCards[i].currentAnimationPos)) charHovered = i;

                if (GuiButton(characterCards[i].currentAnimationPos, ""))
                {
                    if (charSelection == i) charSelection = -1;
                    else charSelection = i;
                }

                if (charSelection == i)
                {
                    Rectangle r1 = {
                        characterCards[i].currentAnimationPos.x - 6,
                        characterCards[i].currentAnimationPos.y - 6,
                        characterCards[i].currentAnimationPos.width + 12,
                        characterCards[i].currentAnimationPos.height + 12
                    };
                    Rectangle r2 = {
                        characterCards[i].currentAnimationPos.x - 12,
                        characterCards[i].currentAnimationPos.y - 12,
                        characterCards[i].currentAnimationPos.width + 24,
                        characterCards[i].currentAnimationPos.height + 24
                    };

                    DrawRectangleLinesEx(r1, 4, YELLOW);
                    DrawRectangleLinesEx(r2, 2, YELLOW);
                }
            }

            // ScreenRects[1] = stats box rect (updated dynamically)
            if (charHovered != -1 && charHovered != charSelection)
            {
                Rectangle cRect = characterCards[charHovered].currentAnimationPos;

                bool placeRight = (cRect.x + CARD_WIDTH + 260.0f) < GetScreenWidth();
                ScreenRects[1] = {
                    placeRight ? (cRect.x + CARD_WIDTH + 5.0f) : (cRect.x - 5.0f - 260.0f),
                    cRect.y - 250.0f,
                    260.0f,
                    240.0f
                };

                DrawRectangleRec(ScreenRects[1], Color{0,40,0,200});
                DrawRectangleLinesEx(ScreenRects[1], 3.0f , Color{40,255,80,255});

                // you can add DrawText() calls here later using stats from allStatLines if you want
            }

            // ScreenRects[0] = Play button (depends on selected or default card)
            {
                int baseIndex = (charSelection == -1) ? 0 : charSelection;
                Rectangle baseCard = characterCards[baseIndex].currentAnimationPos;

                ScreenRects[0] = {
                    (GetScreenWidth() - PLAY_BTN_WIDTH) / 2.0f,
                    baseCard.y + baseCard.height + PLAY_BTN_OFFSET_Y,
                    PLAY_BTN_WIDTH,
                    PLAY_BTN_HEIGHT
                };
            }

            int prevState = GuiGetState();
            if (charSelection == -1) GuiDisable();
            if (GuiButton(ScreenRects[0], "Play Game") && charSelection != -1)
            {
                changeScreen(ScreenState::GAMEPLAY);
            }
            GuiSetState(prevState);

            break;
        }
        case ScreenState::GAMEPLAY:
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
            numScreenTextures = 2;
            ScreenTextures = new Texture2D[numScreenTextures];
            ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png"); // Main menu background
            ScreenTextures[1] = LoadTexture("../assets/images/UI/programTitle.png"); // Program title

            numScreenRects = 2;
            ScreenRects = new Rectangle[numScreenRects];
            ScreenRects[0] = {(GetScreenWidth() - MAIN_BTN_WIDTH) / 2.0f, (GetScreenHeight() - MAIN_BTN_HEIGHT) / 2.0f + MAIN_BTN_OFFSET_Y, MAIN_BTN_WIDTH, MAIN_BTN_HEIGHT}; // Start Game button
            ScreenRects[1] = {(GetScreenWidth() - MAIN_BTN_WIDTH) / 2.0f, (GetScreenHeight() - MAIN_BTN_HEIGHT) / 2.0f + MAIN_BTN_OFFSET_Y + MAIN_BTN_SPACING, MAIN_BTN_WIDTH, MAIN_BTN_HEIGHT}; // Exit Game button
            break;

        case ScreenState::CHARACTER_SELECT:
            characterCards = new charCard[MAX_CHAR_CARDS];
            
            numScreenTextures = 5;
            ScreenTextures = new Texture2D[numScreenTextures];
            ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png");// Character select background
            ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/south.png"); // Student Fighter
            ScreenTextures[2] = LoadTexture("../assets/images/characters/pc/Rat-Assassin/rotations/south.png"); // Rat Assassin
            ScreenTextures[3] = LoadTexture("../assets/images/characters/pc/Professor-Mage/rotations/south.png"); // Professor Mage
            ScreenTextures[4] = LoadTexture("../assets/images/characters/pc/Attila-Brawler/rotations/south.png"); // Attila Brawler

            // set filtering once here instead of every frame
            for (int i = 1; i < numScreenTextures; ++i)
            {
                SetTextureFilter(ScreenTextures[i], TEXTURE_FILTER_POINT);
            }

            // Rects: [0] = Play button (set in render), [1] = stats box (set in render)
            numScreenRects = 2;
            ScreenRects = new Rectangle[numScreenRects];
            ScreenRects[0] = {0,0,0,0};
            ScreenRects[1] = {0,0,0,0};
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

void ScreenManager::exitScreen(ScreenState s) 
{
    switch (s) 
    {
        case ScreenState::MAIN_MENU:
        case ScreenState::CHARACTER_SELECT:
        case ScreenState::GAMEPLAY:
        case ScreenState::SAVE_QUIT:
        {
            for (int i = 0; i < numScreenTextures; ++i)
            {
                UnloadTexture(ScreenTextures[i]);
                ScreenTextures[i] = {};
            }
            delete[] ScreenTextures;
            ScreenTextures = nullptr;
            numScreenTextures = 0;

            for (int i = 0; i < numScreenRects; ++i)
            {
                ScreenRects[i] = {};
            }
            delete[] ScreenRects;
            ScreenRects = nullptr;
            numScreenRects = 0;

            if (characterCards)
            {
                delete[] characterCards;
                characterCards = nullptr;
            }
            break;
        }
    }
}
