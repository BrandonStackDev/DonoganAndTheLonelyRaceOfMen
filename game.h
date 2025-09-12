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

// Your global game state: add indices & a single active Music object.
typedef struct {
    bool invY, invX;
    float musicVol, soundVol;

    // Playback state
    int   currentAlbumIndex;   // -1 if none selected
    int   currentSongIndex;    // -1 if none selected
    Music currentMusic;        // currently loaded/playing stream (if any)
    bool  currentMusicLoaded;  // guard for unload/play calls
    Timer HonkedHornRecently;
} GameState;

static GameState gGame;

#endif // GAME_H
