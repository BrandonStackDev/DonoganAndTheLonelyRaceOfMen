#ifndef GAME_H
#define GAME_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>
//me
#include "timer.h"

typedef enum {
    MENU_PAGE_NONE = 0,
    MENU_PAGE_MAIN,
    MENU_PAGE_INVENTORY,
    MENU_PAGE_OPTIONS,
    MENU_PAGE_MISSIONS,
    MENU_PAGE_MISSION_DETAIL,
    MENU_PAGE_WARP
} MenuPage;

typedef enum {
    DIFF_EASY,
    DIFF_NORMAL,
    DIFF_HARD
} Difficulty;

// Your global game state: add indices & a single active Music object.
typedef struct {
    bool invY, invX;
    float musicVol, soundVol;
    Difficulty diff;

    // Playback state
    int   currentAlbumIndex;   // -1 if none selected
    int   currentSongIndex;    // -1 if none selected
    Music currentMusic;        // currently loaded/playing stream (if any)
    bool  currentMusicLoaded;  // guard for unload/play calls
    Timer HonkedHornRecently;
    

    // --- menu state ---
    bool     menuOpen;
    MenuPage menuPage;
    int      menuSel;
    int      menuScroll;
    int      menuDetailIndex; // for mission detail
    Timer    menuTimer;
} GameState;

static GameState gGame;

static inline void PlaySoundVol(Sound s) { 
    SetSoundVolume(s, gGame.soundVol); 
    if (!IsSoundPlaying(s))
    {
        PlaySound(s);
    }
}

static inline void PlaySoundVolContinuousAllowed(Sound s) {
    SetSoundVolume(s, gGame.soundVol);
    PlaySound(s);
}

#endif // GAME_H
