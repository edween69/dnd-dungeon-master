# TTRPG Game  
_(formerly **dnd-dungeon-master**)_

A small turn-based TTRPG prototype built with C++, raylib, and raygui. For this **milestone**, the project is split into two separate prototypes: **a GUI prototype (main menu + character select + basic combat screen)** and a **console combat engine prototype** that runs entirely in the terminal and focuses on turn-based logic and damage resolution. Both prototypes load character stats from a CSV file and use the same character/stat system; once they are each finalized, the combat engine will be fully integrated into the GUI version so that all combat logic is driven by the same underlying system.

---
## How to Run (Very Important)

As aforesaid, there are two separate prototype submissions for this milestone. To run either one, you must first have **raylib** installed on your system. Please refer to the [raylib installation guide](https://raylib.com/) for your specific device if you are having issues with following these steps. 

On Windows, go to raylib's [Windows installation instructions](https://raylib.com/) and download the windows installer. Follow the steps in the installer to set up raylib on your system.

On Linux, you can install raylib using your package manager. For example, on Ubuntu, you can run:

```
sudo apt-get install libraylib-dev
```

On MacOS, you can install raylib using Homebrew. Run the following command in your terminal:

```
brew install raylib
```

After installing raylib, follow the instructions below to build and run each portion of the project.
### 0. Download the Project ###
- Clone the repository or download the ZIP file from GitHub and extract it to your desired location.


### 1. Combat Engine Prototype ###
- To run this portion of the project, navigate to the `src` directory in your terminal and enter the following command: 

    On Windows:
    ```
    g++ trialSebastian.cpp characters.cpp rng.cpp -o trialSebastian.exe -I "C:\raylib\raylib\src" -std=c++17 -Wall  -Wextra -g
    ```

    On Linux:
    ```
    g++ trialSebastian.cpp characters.cpp rng.cpp -o trialSebastian -I "/usr/local/include/raylib" -std=c++17 -Wall -Wextra -g
    ```

    On MacOS:
    ```
    g++ trialSebastian.cpp characters.cpp rng.cpp -o trialSebastian $(pkg-config --cflags --libs raylib) -std=c++17 -Wall -Wextra -g
    ```
    Then run the compiled executable in the terminal with the following command:

    On Windows:
    ```
    .\trialSebastian
    ```

    On Linux or MacOS:
    ```
    ./trialSebastian
    ```


### 2. GUI Prototype ###
- To run this portion of the project, you will also need `make` installed on your system.

    - On Windows, you can install `mingw32-make` using MSYS2 URCT with the command 
    ```
    pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
    ```


    - On Linux, `make` is usually pre-installed. If not, you can install it using your package manager. For example, on Ubuntu, you can run:
    ```
    sudo apt-get install build-essential
    ```
    - On MacOS, you can install `make` using Homebrew. Run the following command in your terminal:
    ```
    brew install make
    ```

    - Once you have `make` installed, navigate to the root directory of the project in your terminal and run the following command:
    ```
    make
    ```

    - This will compile the source files and create an executable named `TheLastLift` (or `TheLastLift.exe` on Windows) in the `src` directory.


## Screen Flow / GUI

- **Main Menu**
  - Background + title textures
  - `Start Game` → goes to character select  
  - `Exit Game` → closes the game  

- **Character Select**
  - Up to 4 character cards: Student, Rat, Professor, Attila  
  - Cards are centered in a row and smoothly animate:
    - No selection → all in a row
    - One selected → that card centers, others “dock” on the right  
  - Hovering a card shows a stats/info box
    - Student stats read from `dat/Character_Starting_Stats.csv`
  - Only **Student** is selectable right now
  - `Play Game` button:
    - Disabled when nothing is selected
    - When Student is selected:
      - Creates `Student` and `Zombie_Standard`
      - Switches to gameplay/combat

- **Gameplay (Combat UI)**
  - Handled by `GameManager` in `COMBAT` state
  - Draws:
    - Background environment
    - Player and enemy sprites
    - Left/right side panels for player/enemy
    - HP bars that scale with `currentHP / maxHealth`
    - Bottom panel with buttons: `Attack`, `Defend`, `Use Item`
    - Log box to show the **last** action’s message
  - `Attack` button calls `dealMeleeDamage` on the enemy and updates the log text  
  - `Defend` / `Use Item` are placeholders in the GUI

---

## Console Combat Engine

File: `trialSebastian.cpp`  

Separate, text-only combat prototype that uses the same character/stat system:

- Builds a **Student** and a **Zombie_Standard** from `Character_Starting_Stats.csv`
- Turn-based loop in the console:
  - Player menu:
    - `1) Attack`
    - `2) Defend`
    - `3) Use Range` (stub)
    - `4) Use Item` (stub)
  - Enemy AI:
    - 25% chance to **Defend**
    - Otherwise **Attack**
- Uses:
  - `resolve_melee(...)`
  - `resolve_ranged(...)`
  - Temporary defend bonus from `defend_bonus_armor(...)`
- Logs each turn’s result with a `std::stringstream`
- Ends on death of either side with a simple win/lose message

This engine does not depend on raylib and is used to test combat logic by itself.

---

## Code Layout

- `screenManager.h / screenManager.cpp`
  - `ScreenManager`:
    - Manages `MAIN_MENU`, `CHARACTER_SELECT`, `GAMEPLAY`, `SAVE_QUIT`
    - `enterScreen` / `exitScreen` load and unload textures and rectangles
  - `GameManager`:
    - Manages `EXPLORATION`, `COMBAT`, `DIALOGUE`, `PAUSE_MENU`
    - `COMBAT` sets up the full combat UI layout and rendering
  - GUI style helpers:
    - `defaultStyles()`
    - `startMenuStyles()`
    - `playerSelectStyles()`
  - Character select animation and layout logic

- `characters.h / characters.cpp`
  - Base `Character` plus `Student`, `NonPlayerCharacter`
  - Stat structs: `Attributes`, `DefenseStats`, `CombatStats`, `VitalStats`, `StatusEffects`
  - CSV helpers:
    - `openStartingStatsCSV()`
    - `storeAllStatLines(...)`
    - `getStatForCharacterID(...)`

- `rng.cpp`
  - RNG utilities for damage rolls, AI decisions, etc.

- `trialSebastian.cpp`
  - Console combat engine and temporary `main()` for combat testing

- `assets/`
  - `images/UI/...` – menus
  - `images/characters/...` – player and enemy sprites
  - `images/environments/...` – combat backgrounds
  - `dat/Character_Starting_Stats.csv` – all starting stats

---

## Status / Notes

- Only **Student** is fully playable and selectable.
- Rat, Professor, and Attila exist visually but have no real implementation yet.
- GUI combat only fully supports **Attack** right now.
- Console combat has more complete logic than the GUI flow.
- Resources use manual `new[]` / `delete[]` in `enterScreen` / `exitScreen`, so ordering and cleanup matter.

---

## Building

Build and run instructions are handled separately.


