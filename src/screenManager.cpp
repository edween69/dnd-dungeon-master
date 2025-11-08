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
#include "screenManager.h"
#include <fstream>
#include <sstream>
#include "characters.h"
#include "raylib.h"
#include "raygui.h"

// Definining macros for constants to save runtime memory
#define MAX_CHAR_CARDS 4

#define MAIN_BTN_WIDTH 600.0f
#define MAIN_BTN_HEIGHT 70.0f
#define MAIN_BTN_OFFSET_Y 100.0f
#define MAIN_BTN_SPACING 100.0f

#define CARD_WIDTH 300.0f
#define CARD_HEIGHT 400.0f
#define CARD_SPACING 50.0f

#define DOCK_SPACING 90.0f
#define PLAY_BTN_WIDTH 400.0f
#define PLAY_BTN_HEIGHT 60.0f
#define PLAY_BTN_OFFSET_Y 36.0f

std::istringstream *allStatLines = nullptr;

// Creating dynamic GUI items Textures and Rectangles
static Texture2D *ScreenTextures = nullptr;
static int numScreenTextures = 0;

static Rectangle *ScreenRects = nullptr;
static int numScreenRects = 0;

static charCard *characterCards = nullptr;

// extra state for character select (dynamic int array)
// [0] = selection index (-1 = none)
// [1] = hovered index   (-1 = none)  (used in render)
// [2] = layoutInitFlag  (0 = false, 1 = true)
static int *CharSelectionStuff = nullptr;

//@brief: Sets default GUI styles for buttons and text (will be used for resetting styles, if needed).
//@version: 1.0
//@author: Edwin Baiden
void defaultStyles()
{
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x828282FF);  // default gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0xB6B6B6FF); // lighter gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xDADADAFF); // lightest gray border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xE0E0E0FF);    // light gray in hex
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0xC4C4C4FF);   // medium gray in hex
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0xA8A8A8FF);   // darker gray in hex
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0x000000FF);    // black text
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0x000000FF);   // black text
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0x000000FF);   // black text
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);                   // default text size
}

//@brief: Sets GUI styles for the main menu screen (customizes button appearance and text size).
//@version: 1.0//@author: Edwin Baiden
void startMenuStyles()
{
    // Set button styles for the main menu
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x646464FF);  // dark gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x969696FF); // medium gray border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0xC8C8C8FF); // light gray border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x000000B4);    // semi-transparent black in hex
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x323232C8);   // darker gray in hex
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x646464DC);   // lighter gray in hex
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 56); // larger text size
}

//@brief: Sets GUI styles for the character selection screen (customizes button appearance and text size).
//@version: 1.0//@author: Edwin Baiden
void playerSelectStyles()
{
    // Set button styles for the character select screen
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x006600FF);  // dark green border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, 0x008800FF); // medium green border in hex
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, 0x00CC00FF); // light green border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000);    // very transparent so the texture shows through
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x003300C8);   // somewhat transparent green to indicate focus
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x006600DC);   // more opaque green to indicate pressed
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36); // medium text size
    // beveled rectangle style for character portraits
    GuiSetStyle(BUTTON, BORDER_WIDTH, 6); // thicker border

    // Clear “disabled” look defined once (used by Play button when locked)
    GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED, 0x555555FF); // gray border in hex
    GuiSetStyle(BUTTON, BASE_COLOR_DISABLED, 0x222222B4);   // dark semi-transparent gray in hex
    GuiSetStyle(BUTTON, TEXT_COLOR_DISABLED, 0x888888FF);   // light gray text
}

//@brief: Constructor to initialize the ScreenManager with an initial screen state.
//@version: 1.0//@author: Edwin Baiden
ScreenManager::ScreenManager(ScreenState initial)
    : currentScreen(initial) {}

//@brief: Destructor to clean up resources when the ScreenManager is destroyed.
//@version: 1.0//@author: Edwin Baiden
ScreenManager::~ScreenManager()
{
    // Ensure current screen is cleaned up
    exitScreen(currentScreen);
}

//@brief: Initializes the ScreenManager by loading resources for the initial screen (The Start Menu Screen).
//@version: 1.0//@author: Edwin Baiden
void ScreenManager::init()
{
    // Load the initial screen resources
    enterScreen(currentScreen);
}

//@brief: Changes the current screen to a new screen state, handling resource cleanup and loading.
//@version: 1.0//@author: Edwin Baiden
void ScreenManager::changeScreen(ScreenState newScreen)
{
    if (newScreen == currentScreen)
        return;
    exitScreen(currentScreen);
    currentScreen = newScreen;
    enterScreen(currentScreen);
}

//@brief: Retrieves the current screen state.
//@version: 1.0//@author: Edwin Baiden
ScreenState ScreenManager::getCurrentScreen() const
{
    return currentScreen;
}

//@brief: Updates the current screen based on the elapsed time (delta time), not used currently. All done in render().
/*@version: 1.0
  @author: Edwin Baiden*/
void ScreenManager::update(float dt)
{
    switch (currentScreen)
    {
    case ScreenState::MAIN_MENU:
        // handle menu input (e.g., raygui buttons) if needed
        break;

    case ScreenState::CHARACTER_SELECT:
    {
        if (!characterCards || !CharSelectionStuff)
            break;

        // animation + layout logic
        float t = animation::easeInQuad(1.0f - expf(-10.0f * dt)); // Animation time factor

        // assign textures to cards (textures are already loaded in enterScreen)
        for (int i = 0; i < MAX_CHAR_CARDS; ++i)
        {
            characterCards[i].texture = ScreenTextures[i + 1];
        }

        // layout init flag = CharSelectionStuff[2] (0 or 1)
        if (!CharSelectionStuff[2])
        {
            float startX = (GetScreenWidth() - (MAX_CHAR_CARDS * CARD_WIDTH + (MAX_CHAR_CARDS - 1) * CARD_SPACING)) / 2.0f;
            float targetY = (GetScreenHeight() - CARD_HEIGHT) / 2.0f;

            for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            {
                float targetX = startX + i * (CARD_WIDTH + CARD_SPACING);
                characterCards[i].defaultRow.x = targetX;
                characterCards[i].defaultRow.y = targetY;
                characterCards[i].defaultRow.width = CARD_WIDTH;
                characterCards[i].defaultRow.height = CARD_HEIGHT;

                characterCards[i].currentAnimationPos = characterCards[i].defaultRow;
                characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
            }
            CharSelectionStuff[2] = 1; // layout initialized
        }

        // selection index is CharSelectionStuff[0]
        if (CharSelectionStuff[0] == -1)
        {
            // no selection: all cards go back to default row
            for (int i = 0; i < MAX_CHAR_CARDS; i++)
            {
                characterCards[i].targetAnimationPos = characterCards[i].defaultRow;
            }
        }
        else
        {
            // selected card centers, others dock
            characterCards[CharSelectionStuff[0]].targetAnimationPos.x = (GetScreenWidth() - CARD_WIDTH) / 2.0f;
            characterCards[CharSelectionStuff[0]].targetAnimationPos.y = characterCards[CharSelectionStuff[0]].defaultRow.y;
            characterCards[CharSelectionStuff[0]].targetAnimationPos.width = CARD_WIDTH;
            characterCards[CharSelectionStuff[0]].targetAnimationPos.height = CARD_HEIGHT;

            float dockX = GetScreenWidth() - CARD_WIDTH - 40.0f;
            float dockY = GetScreenHeight() - CARD_HEIGHT - 300.0f;

            int dockIndex = 0;
            for (int i = 0; i < MAX_CHAR_CARDS; ++i)
            {
                if (i != CharSelectionStuff[0])
                {
                    characterCards[i].targetAnimationPos.x = dockX;
                    characterCards[i].targetAnimationPos.y = dockY + DOCK_SPACING * dockIndex++;
                    characterCards[i].targetAnimationPos.width = CARD_WIDTH;
                    characterCards[i].targetAnimationPos.height = CARD_HEIGHT;
                }
            }
        }

        // animate cards towards their targets
        for (int i = 0; i < MAX_CHAR_CARDS; i++)
        {
            characterCards[i].currentAnimationPos.x =
                animation::slopeInt(characterCards[i].currentAnimationPos.x,
                                    characterCards[i].targetAnimationPos.x, t);
            characterCards[i].currentAnimationPos.y =
                animation::slopeInt(characterCards[i].currentAnimationPos.y,
                                    characterCards[i].targetAnimationPos.y, t);
        }

        // ScreenRects layout for this screen:
        // [0] = Play button (set here)
        // [1] = Stats box (set in render, when we know which card is hovered)
        // [2] = Selection outline r1
        // [3] = Selection outline r2

        // Play button rect based on selected or default card
        ScreenRects[0].x = (GetScreenWidth() - PLAY_BTN_WIDTH) / 2.0f;
        ScreenRects[0].width = PLAY_BTN_WIDTH;
        ScreenRects[0].height = PLAY_BTN_HEIGHT;

        // y uses selected card if any, otherwise card 0
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
        // movement, circle hotspots, etc.
        break;
    case ScreenState::SAVE_QUIT:
        // confirm/save flow
        break;
    }
}

//@brief: Renders the current screen based on the active screen state.
//@version: 1.0//@author: Edwin Baiden
void ScreenManager::render()
{
    BeginDrawing();         // Start the drawing phase
    ClearBackground(BLACK); // Clear the background to black

    switch (currentScreen)
    {
    case ScreenState::MAIN_MENU:
    {

        // Draw the main menu background
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);

        // draw program title in the top center using getScreenWidth and getScreenHeight
        DrawTexture(ScreenTextures[1], (GetScreenWidth() - ScreenTextures[1].width) / 2, -150, WHITE);

        startMenuStyles(); // Apply main menu styles

        if (GuiButton(ScreenRects[0], "Start Game"))
            changeScreen(ScreenState::CHARACTER_SELECT); // If start button is clicked, go to character select screen

        if (GuiButton(ScreenRects[1], "Exit Game"))
            CloseWindow(); // Close the window and exit
        break;
    }
    case ScreenState::CHARACTER_SELECT:
    {
        // Draw the character selection background
        DrawTexture(ScreenTextures[0], 0, 0, WHITE);
        playerSelectStyles(); // Apply character select styles

        // hovered index lives in CharSelectionStuff[1]
        CharSelectionStuff[1] = -1;

        // draw cards (positions already updated in update())
        for (int i = 0; i < MAX_CHAR_CARDS; i++)
        {
            DrawTexturePro(
                characterCards[i].texture,
                {0.0f, 0.0f, (float)characterCards[i].texture.width, (float)characterCards[i].texture.height},
                characterCards[i].currentAnimationPos,
                {0.0f, 0.0f},
                0.0f,
                WHITE);

            DrawRectangleLinesEx(characterCards[i].currentAnimationPos, 4.0f, Color{0, 68, 0, 255});

            if (CheckCollisionPointRec(GetMousePosition(), characterCards[i].currentAnimationPos))
                CharSelectionStuff[1] = i;

            if (GuiButton(characterCards[i].currentAnimationPos, ""))
            {
                if (CharSelectionStuff[0] == i)
                    CharSelectionStuff[0] = -1;
                else
                    CharSelectionStuff[0] = i;
            }

            if (CharSelectionStuff[0] == i)
            {
                // selection outline r1 -> ScreenRects[2]
                ScreenRects[2].x = characterCards[i].currentAnimationPos.x - 6.0f;
                ScreenRects[2].y = characterCards[i].currentAnimationPos.y - 6.0f;
                ScreenRects[2].width = characterCards[i].currentAnimationPos.width + 12.0f;
                ScreenRects[2].height = characterCards[i].currentAnimationPos.height + 12.0f;

                // selection outline r2 -> ScreenRects[3]
                ScreenRects[3].x = characterCards[i].currentAnimationPos.x - 12.0f;
                ScreenRects[3].y = characterCards[i].currentAnimationPos.y - 12.0f;
                ScreenRects[3].width = characterCards[i].currentAnimationPos.width + 24.0f;
                ScreenRects[3].height = characterCards[i].currentAnimationPos.height + 24.0f;

                DrawRectangleLinesEx(ScreenRects[2], 4, YELLOW);
                DrawRectangleLinesEx(ScreenRects[3], 2, YELLOW);
            }
        }

        // ScreenRects[1] = stats box rect (updated dynamically)
        if (CharSelectionStuff[1] != -1 && CharSelectionStuff[1] != CharSelectionStuff[0])
        {
            ScreenRects[4] = characterCards[CharSelectionStuff[1]].currentAnimationPos;

            bool placeRight = (ScreenRects[4].x + CARD_WIDTH + 260.0f) < GetScreenWidth();
            ScreenRects[1].x = placeRight ? (ScreenRects[4].x + CARD_WIDTH + 5.0f) : (ScreenRects[4].x - 5.0f - 260.0f);
            ScreenRects[1].y = ScreenRects[4].y - 250.0f;
            ScreenRects[1].width = 260.0f;
            ScreenRects[1].height = 240.0f;

            DrawRectangleRec(ScreenRects[1], Color{0, 40, 0, 200});
            DrawRectangleLinesEx(ScreenRects[1], 3.0f, Color{40, 255, 80, 255});

            switch (CharSelectionStuff[1])
            {
            case 0:
                DrawText("Caste: Student", ScreenRects[1].x + 20, ScreenRects[1].y + 20, 24, WHITE);
                DrawText(("Health: " + std::to_string(getStatForCharacterID(allStatLines, "Student", CSVStats::MAX_HEALTH))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 50, 20, WHITE);
                DrawText(("Armor: " + std::to_string(getStatForCharacterID(allStatLines, "Student", CSVStats::ARMOR))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 80, 20, WHITE);
                DrawText(("Strength: " + std::to_string(getStatForCharacterID(allStatLines, "Student", CSVStats::STR))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 110, 20, WHITE);
                DrawText(("Dexterity: " + std::to_string(getStatForCharacterID(allStatLines, "Student", CSVStats::DEX))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 140, 20, WHITE);
                DrawText(("Constitution: " + std::to_string(getStatForCharacterID(allStatLines, "Student", CSVStats::CON))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 170, 20, WHITE);
                DrawText(("Initiative: " + std::to_string(getStatForCharacterID(allStatLines, "Student", CSVStats::INITIATIVE))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 200, 20, WHITE);
                break;
            case 1:
                DrawText("Caste: Rat", ScreenRects[1].x + 20, ScreenRects[1].y + 20, 24, WHITE);
                DrawText(("Health: " + std::to_string(getStatForCharacterID(allStatLines, "Rat", CSVStats::MAX_HEALTH))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 50, 20, WHITE);
                DrawText(("Armor: " + std::to_string(getStatForCharacterID(allStatLines, "Rat", CSVStats::ARMOR))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 80, 20, WHITE);
                DrawText(("Dexterity: " + std::to_string(getStatForCharacterID(allStatLines, "Rat", CSVStats::DEX))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 110, 20, WHITE);
                DrawText(("Constitution: " + std::to_string(getStatForCharacterID(allStatLines, "Rat", CSVStats::CON))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 140, 20, WHITE);
                DrawText(("Initiative: " + std::to_string(getStatForCharacterID(allStatLines, "Rat", CSVStats::INITIATIVE))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 170, 20, WHITE);
                break;
            case 2:
                DrawText("Caste: Professor", ScreenRects[1].x + 20, ScreenRects[1].y + 20, 24, WHITE);
                DrawText(("Health: " + std::to_string(getStatForCharacterID(allStatLines, "Professor", CSVStats::MAX_HEALTH))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 50, 20, WHITE);
                DrawText(("Armor: " + std::to_string(getStatForCharacterID(allStatLines, "Professor", CSVStats::ARMOR))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 80, 20, WHITE);
                DrawText(("Dexterity: " + std::to_string(getStatForCharacterID(allStatLines, "Professor", CSVStats::DEX))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 110, 20, WHITE);
                DrawText(("Constitution: " + std::to_string(getStatForCharacterID(allStatLines, "Professor", CSVStats::CON))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 140, 20, WHITE);
                DrawText(("Initiative: " + std::to_string(getStatForCharacterID(allStatLines, "Professor", CSVStats::INITIATIVE))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 170, 20, WHITE);
                break;
            case 3:
                DrawText("Caste: Atilla", ScreenRects[1].x + 20, ScreenRects[1].y + 20, 24, WHITE);
                DrawText(("Health: " + std::to_string(getStatForCharacterID(allStatLines, "Atilla", CSVStats::MAX_HEALTH))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 50, 20, WHITE);
                DrawText(("Armor: " + std::to_string(getStatForCharacterID(allStatLines, "Atilla", CSVStats::ARMOR))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 80, 20, WHITE);
                DrawText(("Dexterity: " + std::to_string(getStatForCharacterID(allStatLines, "Atilla", CSVStats::DEX))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 110, 20, WHITE);
                DrawText(("Constitution: " + std::to_string(getStatForCharacterID(allStatLines, "Atilla", CSVStats::CON))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 140, 20, WHITE);
                DrawText(("Initiative: " + std::to_string(getStatForCharacterID(allStatLines, "Atilla", CSVStats::INITIATIVE))).c_str(), ScreenRects[1].x + 20, ScreenRects[1].y + 170, 20, WHITE);
                break;
            }
        }

        int prevState = GuiGetState();
        if (CharSelectionStuff[0] == -1)
            GuiDisable();
        if (GuiButton(ScreenRects[0], "Play Game") && CharSelectionStuff[0] != -1)
        {
            changeScreen(ScreenState::GAMEPLAY);
        }
        GuiSetState(prevState);

        break;
    }
    case ScreenState::GAMEPLAY:
        // draw gameplay here later
        break;
    case ScreenState::SAVE_QUIT:
        // simple overlay/text
        break;
    }

    EndDrawing();
}

void ScreenManager::enterScreen(ScreenState s)
{
    switch (s)
    {
    case ScreenState::MAIN_MENU:
        numScreenTextures = 2;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png");  // Main menu background
        ScreenTextures[1] = LoadTexture("../assets/images/UI/programTitle.png"); // Program title

        numScreenRects = 2;
        ScreenRects = new Rectangle[numScreenRects];
        ScreenRects[0].x = (GetScreenWidth() - MAIN_BTN_WIDTH) / 2.0f;
        ScreenRects[0].y = (GetScreenHeight() - MAIN_BTN_HEIGHT) / 2.0f + MAIN_BTN_OFFSET_Y;
        ScreenRects[0].width = MAIN_BTN_WIDTH;
        ScreenRects[0].height = MAIN_BTN_HEIGHT; // Start Game button

        ScreenRects[1].x = (GetScreenWidth() - MAIN_BTN_WIDTH) / 2.0f;
        ScreenRects[1].y = (GetScreenHeight() - MAIN_BTN_HEIGHT) / 2.0f + MAIN_BTN_OFFSET_Y + MAIN_BTN_SPACING;
        ScreenRects[1].width = MAIN_BTN_WIDTH;
        ScreenRects[1].height = MAIN_BTN_HEIGHT; // Exit Game button
        break;

    case ScreenState::CHARACTER_SELECT:
        allStatLines = storeAllStatLines(openStartingStatsCSV());
        characterCards = new charCard[MAX_CHAR_CARDS];

        // CharSelectionStuff holds [selection, hovered, layoutInitFlag]
        CharSelectionStuff = new int[3];
        CharSelectionStuff[0] = -1; // selection
        CharSelectionStuff[1] = -1; // hovered
        CharSelectionStuff[2] = 0;  // layout not initialized yet

        numScreenTextures = 5;
        ScreenTextures = new Texture2D[numScreenTextures];
        ScreenTextures[0] = LoadTexture("../assets/images/UI/startMenuBg.png");                                // Character select background
        ScreenTextures[1] = LoadTexture("../assets/images/characters/pc/Student-Fighter/rotations/south.png"); // Student Fighter
        ScreenTextures[2] = LoadTexture("../assets/images/characters/pc/Rat-Assassin/rotations/south.png");    // Rat Assassin
        ScreenTextures[3] = LoadTexture("../assets/images/characters/pc/Professor-Mage/rotations/south.png");  // Professor Mage
        ScreenTextures[4] = LoadTexture("../assets/images/characters/pc/Attila-Brawler/rotations/south.png");  // Attila Brawler

        // set filtering once here instead of every frame
        for (int i = 1; i < numScreenTextures; ++i)
        {
            SetTextureFilter(ScreenTextures[i], TEXTURE_FILTER_POINT);
        }

        // Rects:
        // [0] = Play button (set in update)
        // [1] = Stats box   (set in render)
        // [2] = r1 selection outline
        // [3] = r2 selection outline
        numScreenRects = 5;
        ScreenRects = new Rectangle[numScreenRects];
        for (int i = 0; i < numScreenRects - 1; ++i)
        {
            ScreenRects[i].x = ScreenRects[i].y = ScreenRects[i].width = ScreenRects[i].height = 0.0f;
        }
        ScreenRects[4] = {0, 0, 0, 0}; // used for center card rectangle
        break;

    case ScreenState::GAMEPLAY:
        // later: load gameplay textures
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

        break;
    }
    }
}
