/*  =================================== screenManager.cpp ====================================
    Project: TTRPG Game ??? Idk what this game is anymore lol
    Subsystem: Screen Manager
    Primary Author: Edwin Baiden
    Description: This file implements the ScreenManager class from screenManager.h  to manage
                 different game screens using raylib and raygui for rendering and GUI
                 elements.

                 Screen States:
                    - MAIN_MENU: The main menu screen where players can start or exit the game.

                    - CHARACTER_SELECT: The character selection screen where players choose
                    their character. You can only select student for now.

                    - GAMEPLAY: The main gameplay screen (not fully implemented here, however
                    combat is being worked on). Values from this screen will be used to
                    determine player actions in combat.

                    - SAVE_QUIT: The save and quit screen (not fully implemented here).

                Each screen has its own styles(managed byfunctuons) and GUI resources
                (textures and rectangles, which are dynamically allocated and deallocated
                when entering and exiting screens).

                Dynamically allocated resources(more on these in enterScreen and exitScreen):
                    - Textures (ScreenTextures + numScreenTextures): Array of textures used
                    in the current screen.

                    - Rectangles (ScreenRects + numScreenRects): Array of rectangles defining GUI
                    element positions and sizes.

                    - Character Cards (characterCards): Array of character card structures used
                    in the character selection screen.

                    - Character Selection State (CharSelectionStuff): Array of integers used to
                    manage character selection state in the character selection screen.

                    - All Stat Lines (allStatLines): String stream holding all character stats
                    from CSV file(function from characters.h/characters.cpp).

                Screen Styles:
                    - defaultStyles(): Sets default GUI styles for buttons and text.
                    - startMenuStyles(): Sets GUI styles for the main menu screen.
                    - playerSelectStyles(): Sets GUI styles for the character selection screen.

                Screen Manager Functions:
                    - ScreenManager::ScreenManager(ScreenState initial): Constructor to initialize
                      the ScreenManager with an initial screen state.

                    - ScreenManager::~ScreenManager(): Destructor to clean up resources when the
                      ScreenManager is destroyed.

                    - void ScreenManager::init():

                Macros for constants(used to save runtime memory):
                    - MAX_CHAR_CARDS: Number of character cards shown on the CHARACTER_SELECT
                    screen.

                    - MAIN_BTN_WIDTH, MAIN_BTN_HEIGHT: Size of main menu buttons ("Start Game",
                    "Exit Game").

                    - MAIN_BTN_OFFSET_Y, MAIN_BTN_SPACING:  Vertical offset of the first main
                    menu button from screen center, and vertical spacing between main menu
                    buttons.

                    - CARD_WIDTH, CARD_HEIGHT: Size of each character selection card.

                    - CARD_SPACING: Horizontal spacing between character cards in the default
                    row.

                    - DOCK_SPACING:  Vertical spacing between docked (i.e nonselected) character
                    cards when one card is centered.

                    PLAY_BTN_WIDTH, PLAY_BTN_HEIGHT: Size of the "Play Game" button on
                    the CHARACTER_SELECT screen.

                    PLAY_BTN_OFFSET_Y: Vertical offset of the "Play Game" button from the character card
                    its under.




*/
/*  =================================== screenManager.cpp ====================================
    Project: TTRPG Game ??? Idk what this game is anymore lol
    Subsystem: Screen Manager
    Primary Author: Edwin Baiden
*/

#include "screenManager.h"

//--------------------------------------------------------------------------------------
// Basic macros / constants
//--------------------------------------------------------------------------------------

#define SCREEN_W            (GetScreenWidth())
#define SCREEN_H            (GetScreenHeight())

// Character select
#define MAX_CHAR_CARDS      4
#define MAIN_BTN_WIDTH      600.0f
#define MAIN_BTN_HEIGHT     70.0f
#define MAIN_BTN_OFFSET_Y   100.0f
#define MAIN_BTN_SPACING    100.0f

#define CARD_WIDTH          300.0f
#define CARD_HEIGHT         400.0f
#define CARD_SPACING        50.0f
#define DOCK_SPACING        90.0f

#define PLAY_BTN_WIDTH      400.0f
#define PLAY_BTN_HEIGHT     60.0f
#define PLAY_BTN_OFFSET_Y   36.0f

// Combat UI: rectangle indices
#define R_PLAYER_NAME       0
#define R_ENEMY_NAME        1
#define R_PLAYER_PANEL      2
#define R_ENEMY_PANEL       3
#define R_PLAYER_HP_BG      4
#define R_PLAYER_HP_FG      5
#define R_ENEMY_HP_BG       6
#define R_ENEMY_HP_FG       7
#define R_PLAYER_STATUS     8
#define R_ENEMY_STATUS      9
#define R_BOTTOM_PANEL      10
#define R_BTN_ATTACK        11
#define R_BTN_DEFEND        12
#define R_BTN_USE_ITEM      13
#define R_LOG_BOX           14

// Text sizes
#define FONT_SIZE_NAME      30
#define FONT_SIZE_HP        20
#define FONT_SIZE_BTN       30

// Centered text helpers (no extra variables)
#define CENTER_TEXT_X(rect, txt, size) \
    (int)((rect).x + (rect).width / 2.0f - MeasureText((txt), (size)) / 2.0f)

#define CENTER_TEXT_Y(rect, size) \
    (int)((rect).y + (rect).height / 2.0f - (size) / 2.0f)

// Health bar width helper
#define HEALTH_BAR_WIDTH(rectBg, cur, max) \
    ((float)(rectBg).width * ((float)(cur) / (float)(max)))

#ifndef CLITERAL
    #define CLITERAL(type) (type)
#endif

// Combat UI colors
#define COL_NAME_BAR        CLITERAL(Color){  8,  8, 12, 255}
#define COL_BOTTOM_PANEL    CLITERAL(Color){112,120,128,255}
#define COL_STATUS_PANEL    CLITERAL(Color){ 55, 61, 57, 220}
#define COL_STATUS_INNER    CLITERAL(Color){ 91, 94, 92, 255}
#define COL_LOG_BOX         CLITERAL(Color){167,171,170,255}
#define COL_BUTTON          CLITERAL(Color){ 68, 74, 72, 255}
#define COL_HP_BG           CLITERAL(Color){ 60, 15, 20, 255}
#define COL_HP_FG           CLITERAL(Color){190, 50, 60, 255}

//--------------------------------------------------------------------------------------
// Global / static state
//--------------------------------------------------------------------------------------

std::istringstream *allStatLines = nullptr;

static Texture2D *ScreenTextures = nullptr;
static int        numScreenTextures = 0;

static Rectangle *ScreenRects = nullptr;
static int        numScreenRects = 0;

static charCard *characterCards = nullptr;
static int       counter = 1;

// extra state for character select (dynamic int array)
// [0] = selection index (-1 = none)
// [1] = hovered index   (-1 = none)
// [2] = layoutInitFlag  (0 = false, 1 = true)
static int *CharSelectionStuff = nullptr;

static Student           *Steve  = nullptr;
static NonPlayerCharacter*Zombie = nullptr;

static GameManager *gameManager = nullptr;

//--------------------------------------------------------------------------------------
// GUI style helpers
//--------------------------------------------------------------------------------------

void defaultStyles()
{
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x828282FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0xB6B6B6FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xDADADAFF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,   0xE0E0E0FF);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,  0xC4C4C4FF);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,  0xA8A8A8FF);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,   0x000000FF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED,  0x000000FF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED,  0x000000FF);
    GuiSetStyle(DEFAULT, TEXT_SIZE,          20);
}

void startMenuStyles()
{
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL,  0x646464FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x969696FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xC8C8C8FF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,    0x000000B4);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,   0x323232C8);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,   0x646464DC);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,    0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED,   0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED,   0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE,           56);
}

void playerSelectStyles()
{
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL,    0x006600FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED,   0x008800FF);
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED,   0x00CC00FF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,      0x00000000);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,     0x003300C8);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,     0x006600DC);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,      0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED,     0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED,     0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE,             36);
    GuiSetStyle(BUTTON, BORDER_WIDTH,           6);

    GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED,  0x555555FF);
    GuiSetStyle(BUTTON, BASE_COLOR_DISABLED,    0x222222B4);
    GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED,    0x888888FF);
}

//--------------------------------------------------------------------------------------
// Character creation helper (CSV -> objects)
//--------------------------------------------------------------------------------------

void CreateCharacter(std::string ID)
{
    Attributes     CharAttrs;
    DefenseStats   CharDef;
    CombatStats    CharCbt;
    VitalStats     CharVit;
    StatusEffects  CharStatus;

    CharAttrs = {
        getStatForCharacterID(allStatLines, ID, CSVStats::STR),
        getStatForCharacterID(allStatLines, ID, CSVStats::DEX),
        getStatForCharacterID(allStatLines, ID, CSVStats::CON),
        getStatForCharacterID(allStatLines, ID, CSVStats::WIS),
        getStatForCharacterID(allStatLines, ID, CSVStats::CHA),
        getStatForCharacterID(allStatLines, ID, CSVStats::INT)
    };

    CharDef = {
        getStatForCharacterID(allStatLines, ID, CSVStats::ARMOR),
        0 // magic resist unused
    };

    CharCbt = {
        5,
        3,
        getStatForCharacterID(allStatLines, ID, CSVStats::INITIATIVE)
    };

    CharVit = {
        getStatForCharacterID(allStatLines, ID, CSVStats::MAX_HEALTH),
        getStatForCharacterID(allStatLines, ID, CSVStats::MAX_HEALTH)
    };

    CharStatus = {};

    if (ID == "Student")
    {
        Steve = new Student("Steve", CharAttrs, CharDef, CharCbt, CharVit, CharStatus);
    }
    else if (ID == "Zombie_Standard")
    {
        Zombie = new NonPlayerCharacter("Chad", CharAttrs, CharDef, CharCbt, CharVit, CharStatus);
    }
}

//--------------------------------------------------------------------------------------
// ScreenManager
//--------------------------------------------------------------------------------------

ScreenManager::ScreenManager(ScreenState initial) : currentScreen(initial) {}

ScreenManager::~ScreenManager()
{
    exitScreen(currentScreen);
}

void ScreenManager::init()
{
    enterScreen(currentScreen);
}

void ScreenManager::changeScreen(ScreenState newScreen)
{
    if (newScreen == currentScreen) return;
    exitScreen(currentScreen);
    currentScreen = newScreen;
    enterScreen(currentScreen);
}

ScreenState ScreenManager::getCurrentScreen() const
{
    return currentScreen;
}

void ScreenManager::update(float dt)
{
    switch (currentScreen)
    {
    case ScreenState::MAIN_MENU:
        break;

    case ScreenState::CHARACTER_SELECT:
    {
        if (!characterCards || !CharSelectionStuff) break;

        float t = animation::easeInQuad(1.0f - expf(-10.0f * dt));

        for (int i = 0; i < MAX_CHAR_CARDS; ++i)
        {
            characterCards[i].texture = ScreenTextures[i + 1];
        }

        if (!CharSelectionStuff[2])
        {
            float startX = (SCREEN_W - (MAX_CHAR_CARDS * CARD_WIDTH +
                             (MAX_CHAR_CARDS - 1) * CARD_SPACING)) / 2.0f;
            float targetY = (SCREEN_H - CARD_HEIGHT) / 2.0f;

            for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            {
                float targetX = startX + i * (CARD_WIDTH + CARD_SPACING);
                characterCards[i].defaultRow.x      = targetX;
                characterCards[i].defaultRow.y      = targetY;
                characterCards[i].defaultRow.width  = CARD_WIDTH;
                characterCards[i].defaultRow.height = CARD_HEIGHT;

                characterCards[i].currentAnimationPos = characterCards[i].defaultRow;
                characterCards[i].targetAnimationPos  = characterCards[i].defaultRow;
            }
            CharSelectionStuff[2] = 1;
        }

        if (CharSelectionStuff[0] == -1)
        {
            for (int i = 0; i < MAX_CHAR_CARDS; i++)
            {
                characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
            }
        }
        else
        {
            characterCards[CharSelectionStuff[0]].targetAnimationPos.x      = (SCREEN_W - CARD_WIDTH) / 2.0f;
            characterCards[CharSelectionStuff[0]].targetAnimationPos.y      = characterCards[CharSelectionStuff[0]].defaultRow.y;
            characterCards[CharSelectionStuff[0]].targetAnimationPos.width  = CARD_WIDTH;
            characterCards[CharSelectionStuff[0]].targetAnimationPos.height = CARD_HEIGHT;

            float dockX = SCREEN_W - CARD_WIDTH - 40.0f;
            float dockY = SCREEN_H - CARD_HEIGHT - 300.0f;

            int dockIndex = 0;
            for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            {
                if (i != CharSelectionStuff[0])
                {
                    characterCards[i].targetAnimationPos.x      = dockX;
                    characterCards[i].targetAnimationPos.y      = dockY + DOCK_SPACING * dockIndex++;
                    characterCards[i].targetAnimationPos.width  = CARD_WIDTH;
                    characterCards[i].targetAnimationPos.height = CARD_HEIGHT;
                }
            }
        }

        for (int i = 0; i < MAX_CHAR_CARDS; i++)
        {
            characterCards[i].currentAnimationPos.x =
                animation::slopeInt(characterCards[i].currentAnimationPos.x,
                                    characterCards[i].targetAnimationPos.x, t);
            characterCards[i].currentAnimationPos.y =
                animation::slopeInt(characterCards[i].currentAnimationPos.y,
                                    characterCards[i].targetAnimationPos.y, t);
        }

        ScreenRects[0].x      = (SCREEN_W - PLAY_BTN_WIDTH) / 2.0f;
        ScreenRects[0].width  = PLAY_BTN_WIDTH;
        ScreenRects[0].height = PLAY_BTN_HEIGHT;

        if (CharSelectionStuff[0] == -1)
        {
            ScreenRects[0].y = characterCards[0].currentAnimationPos.y +
                               characterCards[0].currentAnimationPos.height +
                               PLAY_BTN_OFFSET_Y;
        }
        else
        {
            ScreenRects[0].y = characterCards[CharSelectionStuff[0]].currentAnimationPos.y +
                               characterCards[CharSelectionStuff[0]].currentAnimationPos.height +
                               PLAY_BTN_OFFSET_Y;
        }

        break;
    }

    case ScreenState::GAMEPLAY:
        break;

    case ScreenState::SAVE_QUIT:
        break;
    }
}

void ScreenManager::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    switch (currentScreen)
    {
    //--------------------------------------------------------------------------
    // MAIN MENU
    //--------------------------------------------------------------------------
    case ScreenState::MAIN_MENU:
    {
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);
        DrawTexture(ScreenTextures[1],
                    (SCREEN_W - ScreenTextures[1].width) / 2,
                    -150,
                    WHITE);

        startMenuStyles();

        if (GuiButton(ScreenRects[0], "Start Game"))
            changeScreen(ScreenState::CHARACTER_SELECT);

        if (GuiButton(ScreenRects[1], "Exit Game"))
            CloseWindow();
        break;
    }

    //--------------------------------------------------------------------------
    // CHARACTER SELECT
    //--------------------------------------------------------------------------
    case ScreenState::CHARACTER_SELECT:
    {
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);
        playerSelectStyles();

        CharSelectionStuff[1] = -1;

        for (int i = 0; i < MAX_CHAR_CARDS; i++)
        {
            DrawTexturePro(
                characterCards[i].texture,
                {0.0f, 0.0f,
                 (float)characterCards[i].texture.width,
                 (float)characterCards[i].texture.height},
                characterCards[i].currentAnimationPos,
                {0.0f, 0.0f},
                0.0f,
                WHITE);

            DrawRectangleLinesEx(characterCards[i].currentAnimationPos, 4.0f,(Color){0, 68, 0, 255});

            if (CheckCollisionPointRec(GetMousePosition(),characterCards[i].currentAnimationPos)) CharSelectionStuff[1] = i;

            if (i==0 && GuiButton(characterCards[i].currentAnimationPos, ""))
            {
                if (CharSelectionStuff[0] == i) CharSelectionStuff[0] = -1;
                else CharSelectionStuff[0] = i;
            }

            if (CharSelectionStuff[0] == i)
            {
                ScreenRects[2].x      = characterCards[i].currentAnimationPos.x - 6.0f;
                ScreenRects[2].y      = characterCards[i].currentAnimationPos.y - 6.0f;
                ScreenRects[2].width  = characterCards[i].currentAnimationPos.width + 12.0f;
                ScreenRects[2].height = characterCards[i].currentAnimationPos.height + 12.0f;

                ScreenRects[3].x      = characterCards[i].currentAnimationPos.x - 12.0f;
                ScreenRects[3].y      = characterCards[i].currentAnimationPos.y - 12.0f;
                ScreenRects[3].width  = characterCards[i].currentAnimationPos.width + 24.0f;
                ScreenRects[3].height = characterCards[i].currentAnimationPos.height + 24.0f;

                DrawRectangleLinesEx(ScreenRects[2], 4, YELLOW);
                DrawRectangleLinesEx(ScreenRects[3], 2, YELLOW);

                


            }
        }

        if (CharSelectionStuff[1] != -1 && CharSelectionStuff[1] != CharSelectionStuff[0])
        {
            ScreenRects[4] = characterCards[CharSelectionStuff[1]].currentAnimationPos;

            bool placeRight = (ScreenRects[4].x + CARD_WIDTH + 260.0f) < SCREEN_W;
            ScreenRects[1].x = placeRight
                               ? (ScreenRects[4].x + CARD_WIDTH + 5.0f)
                               : (ScreenRects[4].x - 5.0f - 260.0f);
            ScreenRects[1].y      = ScreenRects[4].y - 250.0f;
            ScreenRects[1].width  = 260.0f;
            ScreenRects[1].height = 240.0f;

            DrawRectangleRec(ScreenRects[1],(Color){0, 40, 0, 200});
            DrawRectangleLinesEx(ScreenRects[1], 3.0f,(Color){40, 255, 80, 255});

            switch (CharSelectionStuff[1])
            {
            case 0:
                DrawText("Caste: Student", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                DrawText(("Health: " + std::to_string(getStatForCharacterID(allStatLines,"Student",CSVStats::MAX_HEALTH))).c_str(),(int)(ScreenRects[1].x + 20),(int)(ScreenRects[1].y + 50), 20, WHITE);
                DrawText(("Armor: " + std::to_string(getStatForCharacterID(allStatLines,"Student",CSVStats::ARMOR))).c_str(),(int)(ScreenRects[1].x + 20),(int)(ScreenRects[1].y + 80), 20, WHITE);
                DrawText(("Dexterity: " + std::to_string(getStatForCharacterID(allStatLines,"Student",CSVStats::DEX))).c_str(),(int)(ScreenRects[1].x + 20),(int)(ScreenRects[1].y + 110), 20, WHITE);
                DrawText(("Constitution: " + std::to_string(getStatForCharacterID(allStatLines,"Student",CSVStats::CON))).c_str(),(int)(ScreenRects[1].x + 20),(int)(ScreenRects[1].y + 140), 20, WHITE);
                DrawText(("Initiative: " + std::to_string(getStatForCharacterID(allStatLines,"Student",CSVStats::INITIATIVE))).c_str(),(int)(ScreenRects[1].x + 20),(int)(ScreenRects[1].y + 170), 20, WHITE);
                break;

            case 1:
                DrawText("Caste: Rat", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                DrawText("Not Available", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 50), 20, WHITE);
                break;

            case 2:
                DrawText("Caste: Professor", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                DrawText("Not Available", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 50), 20, WHITE);
                
                break;

            case 3:
                DrawText("Caste: Atilla", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 20),24, WHITE);
                DrawText("Not Available", (int)(ScreenRects[1].x + 20), (int)(ScreenRects[1].y + 50), 20, WHITE);
                break;
            }
        }

        int prevState = GuiGetState();
        if (CharSelectionStuff[0] == -1) GuiDisable();

        if (GuiButton(ScreenRects[0], "Play Game") &&
            CharSelectionStuff[0] != -1)
        {
            CreateCharacter("Student");
            CreateCharacter("Zombie_Standard");
            changeScreen(ScreenState::GAMEPLAY);
        }
        GuiSetState(prevState);

        break;
    }

    //--------------------------------------------------------------------------
    // GAMEPLAY (high-level)
    //--------------------------------------------------------------------------
    case ScreenState::GAMEPLAY:
        if (counter % 50 == 0)
        {
            Steve->dealMeleeDamage(*Zombie);
        }
        else if (counter % 20 == 0)
        {
            Zombie->dealMeleeDamage(*Steve);
        }
        counter++;
        gameManager->update(GetFrameTime());
        gameManager->render();
        break;

    case ScreenState::SAVE_QUIT:
        break;
    }

    EndDrawing();
}

//--------------------------------------------------------------------------------------
// ScreenManager: enter / exit screen
//--------------------------------------------------------------------------------------

void ScreenManager::enterScreen(ScreenState s)
{
    switch (s)
    {
    case ScreenState::MAIN_MENU:
        numScreenTextures = 2;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png");
        ScreenTextures[1] = LoadTexture("../assets/images/UI/gameTitle.png");

        numScreenRects = 2;
        ScreenRects = new Rectangle[numScreenRects];
        ScreenRects[0].x = (SCREEN_W - MAIN_BTN_WIDTH) / 2.0f;
        ScreenRects[0].y = (SCREEN_H - MAIN_BTN_HEIGHT) / 2.0f + MAIN_BTN_OFFSET_Y;
        ScreenRects[0].width = MAIN_BTN_WIDTH;
        ScreenRects[0].height = MAIN_BTN_HEIGHT;

        ScreenRects[1].x = (SCREEN_W - MAIN_BTN_WIDTH) / 2.0f;
        ScreenRects[1].y = (SCREEN_H - MAIN_BTN_HEIGHT) / 2.0f + MAIN_BTN_OFFSET_Y + MAIN_BTN_SPACING;
        ScreenRects[1].width = MAIN_BTN_WIDTH;
        ScreenRects[1].height = MAIN_BTN_HEIGHT;
        break;

    case ScreenState::CHARACTER_SELECT:
        allStatLines = storeAllStatLines(openStartingStatsCSV());
        characterCards = new charCard[MAX_CHAR_CARDS];

        CharSelectionStuff = new int[3];
        CharSelectionStuff[0] = -1;
        CharSelectionStuff[1] = -1;
        CharSelectionStuff[2] = 0;

        numScreenTextures = 5;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png");
        ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/south.png");
        ScreenTextures[2] = LoadTexture("../assets/images/characters/pc/Rat-Assassin/rotations/south.png");
        ScreenTextures[3] = LoadTexture("../assets/images/characters/pc/Professor-Mage/rotations/south.png");
        ScreenTextures[4] = LoadTexture("../assets/images/characters/pc/Attila-Brawler/rotations/south.png");

        for (int i = 1; i < numScreenTextures; ++i)
        {
            SetTextureFilter(ScreenTextures[i], TEXTURE_FILTER_POINT);
        }

        numScreenRects = 5;
        ScreenRects = new Rectangle[numScreenRects];
        for (int i = 0; i < numScreenRects - 1; ++i)
        {
            ScreenRects[i].x = ScreenRects[i].y =
            ScreenRects[i].width = ScreenRects[i].height = 0.0f;
        }
        ScreenRects[4] = {0, 0, 0, 0};
        break;

    case ScreenState::GAMEPLAY:
        gameManager = new GameManager;
        gameManager->enterGameState(gameManager->getCurrentGameState());
        break;

    case ScreenState::SAVE_QUIT:
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

        if (CharSelectionStuff)
        {
            delete[] CharSelectionStuff;
            CharSelectionStuff = nullptr;
        }

        if (allStatLines)
        {
            allStatLines->clear();
            delete allStatLines;
            allStatLines = nullptr;
        }

        if (s == ScreenState::GAMEPLAY && gameManager)
        {
            gameManager->exitGameState(gameManager->getCurrentGameState());
            delete gameManager;
            gameManager = nullptr;
        }
        break;
    }
    }
}

//--------------------------------------------------------------------------------------
// GameManager
//--------------------------------------------------------------------------------------

GameManager::GameManager(GameState initial) : currentGameState(initial) {}
GameManager::~GameManager() {}

void GameManager::changeGameState(GameState newState)
{
    if (newState == currentGameState) return;
    exitGameState(currentGameState);
    currentGameState = newState;
    enterGameState(currentGameState);
}

GameState GameManager::getCurrentGameState() const
{
    return currentGameState;
}

void GameManager::enterGameState(GameState state)
{
    switch (state)
    {
    case GameState::EXPLORATION:
        break;

    case GameState::COMBAT:
        numScreenTextures = 3;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/environments/Building1/Hallway/Hallway[1-2].png");
        ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/north-west.png");
        ScreenTextures[2] = LoadTexture("../assets/images/characters/npc/Enemies/FratBro1.png");

        numScreenRects = 15;
        ScreenRects = new Rectangle[numScreenRects];

        ScreenRects[R_PLAYER_NAME] = {0,0, 450, 50};
        ScreenRects[R_ENEMY_NAME] = {SCREEN_W - ScreenRects[R_PLAYER_NAME].width, 0, ScreenRects[R_PLAYER_NAME].width, ScreenRects[R_PLAYER_NAME].height};
        ScreenRects[R_PLAYER_PANEL]= {0, ScreenRects[R_PLAYER_NAME].height, ScreenRects[R_PLAYER_NAME].width, 832};
        ScreenRects[R_ENEMY_PANEL] = {ScreenRects[R_ENEMY_NAME].x,ScreenRects[R_ENEMY_NAME].y + ScreenRects[R_ENEMY_NAME].height, ScreenRects[R_ENEMY_NAME].width, 832};
        ScreenRects[R_PLAYER_HP_BG] = {ScreenRects[R_PLAYER_PANEL].x + 20, ScreenRects[R_PLAYER_PANEL].y + 100, ScreenRects[R_PLAYER_PANEL].width - 40, 30};
        ScreenRects[R_PLAYER_HP_FG] = ScreenRects[R_PLAYER_HP_BG];
        ScreenRects[R_ENEMY_HP_BG] = {ScreenRects[R_ENEMY_PANEL].x + 20, ScreenRects[R_ENEMY_PANEL].y + 100,ScreenRects[R_ENEMY_PANEL].width - 40,30};
        ScreenRects[R_ENEMY_HP_FG] = ScreenRects[R_ENEMY_HP_BG];
        ScreenRects[R_PLAYER_STATUS] = {ScreenRects[R_PLAYER_PANEL].x + 20,ScreenRects[R_PLAYER_PANEL].y + 200, ScreenRects[R_PLAYER_PANEL].width - 40,500};
        ScreenRects[R_ENEMY_STATUS] = {ScreenRects[R_ENEMY_PANEL].x + 20,ScreenRects[R_ENEMY_PANEL].y + 200,ScreenRects[R_ENEMY_PANEL].width - 40,500};
        ScreenRects[R_BOTTOM_PANEL] = {0,SCREEN_H - 200,SCREEN_W,215};
        ScreenRects[R_BTN_ATTACK] = {ScreenRects[R_BOTTOM_PANEL].x + 20,ScreenRects[R_BOTTOM_PANEL].y + 20,400,80};
        ScreenRects[R_BTN_DEFEND] = {ScreenRects[R_BOTTOM_PANEL].x + 20,ScreenRects[R_BTN_ATTACK].y + 100,400,80};
        ScreenRects[R_BTN_USE_ITEM] = {ScreenRects[R_BTN_ATTACK].x + ScreenRects[R_BTN_ATTACK].width + 150,ScreenRects[R_BTN_ATTACK].y,400,80};
        ScreenRects[R_LOG_BOX] = {SCREEN_W - 800, ScreenRects[R_BTN_ATTACK].y, 780, 175};
        break;

    case GameState::DIALOGUE:
        break;

    case GameState::PAUSE_MENU:
        break;
    }
}

void GameManager::exitGameState(GameState state)
{
    switch (state)
    {
    case GameState::EXPLORATION:
        break;
    case GameState::COMBAT:
        break;
    case GameState::DIALOGUE:
        break;
    case GameState::PAUSE_MENU:
        break;
    }
}

void GameManager::update(float dt)
{
    switch (currentGameState)
    {
    case GameState::EXPLORATION:
        break;

    case GameState::COMBAT:
        ScreenRects[R_PLAYER_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_PLAYER_HP_BG],Steve->vit.health, Steve->vit.maxHealth);
        ScreenRects[R_ENEMY_HP_FG].width = HEALTH_BAR_WIDTH(ScreenRects[R_ENEMY_HP_BG], Zombie->vit.health, Zombie->vit.maxHealth);
        break;

    case GameState::DIALOGUE:
        break;

    case GameState::PAUSE_MENU:
        break;
    }
}

void GameManager::render()
{
    switch (currentGameState)
    {
    case GameState::EXPLORATION:
        break;

    case GameState::COMBAT:
        // Centered background
        DrawTexture(ScreenTextures[0],(int)(SCREEN_W / 2.0f - ScreenTextures[0].width  / 2.0f), (int)(SCREEN_H / 2.0f - ScreenTextures[0].height / 2.0f - 175.f), WHITE);

        // Player
        DrawTexturePro(ScreenTextures[1], 
            {0.0f, 0.0f,(float)ScreenTextures[1].width,
             (float)ScreenTextures[1].height},
            {SCREEN_W / 2.0f + ScreenTextures[0].width / 2.0f - 500.f,
             SCREEN_H / 2.0f + ScreenTextures[0].height / 2.0f - 650.f,
             500.0f,
             500.0f},
            {0.0f, 0.0f},
            0.0f,
            WHITE);

        // Enemy
        DrawTexturePro(
            ScreenTextures[2],
            {0.0f, 0.0f,
             (float)ScreenTextures[2].width,
             (float)ScreenTextures[2].height},
            {SCREEN_W / 2.0f + ScreenTextures[0].width / 2.0f - 600.f,
             SCREEN_H / 2.0f + ScreenTextures[0].height / 2.0f - 725.f,
             200.f,
             300.f},
            {0.0f, 0.0f},
            0.0f,
            WHITE);

        // Panels
        DrawRectangleRec(ScreenRects[R_PLAYER_NAME],COL_NAME_BAR);
        DrawRectangleRec(ScreenRects[R_ENEMY_NAME], COL_NAME_BAR);
        DrawRectangleRec(ScreenRects[R_BOTTOM_PANEL], COL_BOTTOM_PANEL);
        DrawRectangleRec(ScreenRects[R_PLAYER_PANEL], COL_STATUS_PANEL);
        DrawRectangleRec(ScreenRects[R_ENEMY_PANEL], COL_STATUS_PANEL);

        // HP bars
        DrawRectangleRec(ScreenRects[R_PLAYER_HP_BG],COL_HP_BG);
        DrawRectangleRec(ScreenRects[R_PLAYER_HP_FG],COL_HP_FG);
        DrawRectangleRec(ScreenRects[R_ENEMY_HP_BG], COL_HP_BG);
        DrawRectangleRec(ScreenRects[R_ENEMY_HP_FG],COL_HP_FG);

        // Status / log
        DrawRectangleRec(ScreenRects[R_PLAYER_STATUS],COL_STATUS_INNER);
        DrawRectangleRec(ScreenRects[R_ENEMY_STATUS],COL_STATUS_INNER);
        DrawRectangleRec(ScreenRects[R_LOG_BOX],COL_LOG_BOX);

        // Buttons
        DrawRectangleRec(ScreenRects[R_BTN_ATTACK],COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_BTN_DEFEND], COL_BUTTON);
        DrawRectangleRec(ScreenRects[R_BTN_USE_ITEM], COL_BUTTON);

        // Borders (skip HP overlays)
        for (int i = 0; i < 15; ++i)
        {
            if (i == R_PLAYER_HP_FG || i == R_ENEMY_HP_FG) continue;
            DrawRectangleLinesEx(ScreenRects[i], 3.0f, BLACK);
        }

        // Text: names + HP
        DrawText(("Player: " + Steve->getName()).c_str(), (int)(ScreenRects[R_PLAYER_NAME].x + 20),(int)(ScreenRects[R_PLAYER_NAME].y + 10), FONT_SIZE_NAME, WHITE);

        DrawText(("Enemy: " + Zombie->getName()).c_str(), (int)(ScreenRects[R_ENEMY_NAME].x + 20), (int)(ScreenRects[R_ENEMY_NAME].y + 10), FONT_SIZE_NAME, WHITE);

        DrawText(("HP: " + std::to_string(Steve->vit.health) + " / " +
                  std::to_string(Steve->vit.maxHealth)).c_str(),
                 (int)(ScreenRects[R_PLAYER_PANEL].x + 30),
                 (int)(ScreenRects[R_PLAYER_PANEL].y + 130),
                 FONT_SIZE_HP,
                 WHITE);

        DrawText(("HP: " + std::to_string(Zombie->vit.health) + " / " +
                  std::to_string(Zombie->vit.maxHealth)).c_str(),
                 (int)(ScreenRects[R_ENEMY_PANEL].x + 30),
                 (int)(ScreenRects[R_ENEMY_PANEL].y + 130),
                 FONT_SIZE_HP,
                 WHITE);

        // Button labels (centered, same logic as before)
        DrawText("ATTACK",
                 CENTER_TEXT_X(ScreenRects[R_BTN_ATTACK], "ATTACK", FONT_SIZE_BTN),
                 CENTER_TEXT_Y(ScreenRects[R_BTN_ATTACK], FONT_SIZE_BTN),
                 FONT_SIZE_BTN,
                 WHITE);

        DrawText("DEFEND",
                 CENTER_TEXT_X(ScreenRects[R_BTN_DEFEND], "DEFEND", FONT_SIZE_BTN),
                 CENTER_TEXT_Y(ScreenRects[R_BTN_DEFEND], FONT_SIZE_BTN),
                 FONT_SIZE_BTN,
                 WHITE);

        DrawText("USE ITEM",
                 CENTER_TEXT_X(ScreenRects[R_BTN_USE_ITEM], "USE ITEM", FONT_SIZE_BTN),
                 CENTER_TEXT_Y(ScreenRects[R_BTN_USE_ITEM], FONT_SIZE_BTN),
                 FONT_SIZE_BTN,
                 WHITE);

        break;

    case GameState::DIALOGUE:
        break;

    case GameState::PAUSE_MENU:
        break;
    }
}
