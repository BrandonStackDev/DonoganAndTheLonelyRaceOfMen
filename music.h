#ifndef MUSIC_H
#define MUSIC_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


// --- Limits & sizes ----------------------------------------------------------
#define MAX_SONGS_PER_ALBUM   16
#define MAX_ALBUMS             8
#define MAX_NAME_LEN          64
#define MAX_FILE_LEN         128

// Forward declarations so GameState can reference these types
typedef struct Song Song;
typedef struct Album Album;
typedef struct GameMusic GameMusic;

// --- Data types --------------------------------------------------------------

// A single track: just display + filename on disk.
// You will load/UnloadMusicStream() at runtime only for the *current* track.
struct Song {
    char display[MAX_NAME_LEN];  // e.g. "Main Theme"
    char file[MAX_FILE_LEN];   // e.g. "audio/main_theme.ogg"
};

// A music album: display name, artist, and up to N tracks.
struct Album {
    char display[MAX_NAME_LEN];  // e.g. "Donogan OST"
    char artist[MAX_NAME_LEN];  // e.g. "Fiver"
    Song songs[MAX_SONGS_PER_ALBUM];
    int  songCount;
};

// Top-level music library: up to M albums.
struct GameMusic {
    Album albums[MAX_ALBUMS];
    int   albumCount;
};

// Your global game state: add indices & a single active Music object.
typedef struct {
    bool invY, invX;
    float musicVol, soundVol;

    // Playback state
    int   currentAlbumIndex;   // -1 if none selected
    int   currentSongIndex;    // -1 if none selected
    Music currentMusic;        // currently loaded/playing stream (if any)
    bool  currentMusicLoaded;  // guard for unload/play calls
} GameState;

// --- Small helpers -----------------------------------------------------------

static inline void Album_Clear(Album* a) {
    memset(a, 0, sizeof(*a));
}

static inline void Album_AddSong(Album* a, const char* display, const char* file) {
    if (a->songCount >= MAX_SONGS_PER_ALBUM) return;
    Song* s = &a->songs[a->songCount++];
    snprintf(s->display, sizeof(s->display), "%s", display ? display : "");
    snprintf(s->file, sizeof(s->file), "%s", file ? file : "");
}

static inline void GameMusic_Clear(GameMusic* lib) {
    memset(lib, 0, sizeof(*lib));
}

static inline void GameState_InitAudio(GameState* gs) {
    gs->currentAlbumIndex = -1;
    gs->currentSongIndex = -1;
    gs->currentMusicLoaded = false;
    gs->musicVol = 1.0f;
    gs->soundVol = 1.0f;
}

// --- Dummy content seeding ---------------------------------------------------
// This does NOT load any Music; it only fills names/paths.
// Adjust filenames to match your actual asset layout.
static inline void GameMusic_Init(GameMusic* lib) {
    GameMusic_Clear(lib);

    // Album demo (I)
    Album* a0 = &lib->albums[lib->albumCount++];
    Album_Clear(a0);
    snprintf(a0->display, sizeof(a0->display), "I");
    snprintf(a0->artist, sizeof(a0->artist), "FlannelKat");
    Album_AddSong(a0, "Pink Lady", "music/I/01 Pink Lady.mp3");
    Album_AddSong(a0, "Locomotive", "music/I/02 Locomotive.mp3");
    Album_AddSong(a0, "Warlock", "music/I/03 Warlock.mp3");
    Album_AddSong(a0, "Tri. Rex", "music/I/04 Trichotomous Rex.mp3");

    // Album garden
    Album* a1 = &lib->albums[lib->albumCount++];
    Album_Clear(a1);
    snprintf(a1->display, sizeof(a1->display), "Garden");
    snprintf(a1->artist, sizeof(a1->artist), "Fat Buckle");
    Album_AddSong(a1, "Arketized", "music/Garden/Arketized.mp3");
    Album_AddSong(a1, "Bells", "music/Garden/Bells.mp3");
    Album_AddSong(a1, "BucketMan", "music/Garden/BucketMan.mp3");
    Album_AddSong(a1, "Fire", "music/Garden/Fire.mp3");
    Album_AddSong(a1, "Freckles", "music/Garden/Freckles.mp3");
    Album_AddSong(a1, "Frogg", "music/Garden/Frogg.mp3");
    Album_AddSong(a1, "NinjaKitten", "music/Garden/NinjaKitten.mp3");
    Album_AddSong(a1, "Salmon", "music/Garden/Salmon.mp3");
    Album_AddSong(a1, "Scottlan", "music/Garden/Scottlan.mp3");
    Album_AddSong(a1, "Words", "music/Garden/Words.mp3");
    Album_AddSong(a1, "Yes", "music/Garden/Yes.mp3");
}

// --- Optional: minimal runtime helpers (no auto-loading) --------------------
// You can call these to select what should be *currently* active.
// Actual LoadMusicStream/PlayMusicStream can be done when you detect a change.

static inline const Album* GM_GetAlbum(const GameMusic* lib, int idx) {
    if (idx < 0 || idx >= lib->albumCount) return NULL;
    return &lib->albums[idx];
}

static inline const Song* GM_GetSong(const Album* a, int idx) {
    if (!a || idx < 0 || idx >= a->songCount) return NULL;
    return &a->songs[idx];
}

static inline void GM_Select(GameState* gs, const GameMusic* lib, int albumIndex, int songIndex) {
    const Album* a = GM_GetAlbum(lib, albumIndex);
    if (!a) { gs->currentAlbumIndex = gs->currentSongIndex = -1; return; }
    if (songIndex < 0 || songIndex >= a->songCount) { gs->currentAlbumIndex = albumIndex; gs->currentSongIndex = -1; return; }
    gs->currentAlbumIndex = albumIndex;
    gs->currentSongIndex = songIndex;
}

// Example loader that (un)loads the single current Music stream.
// Call this when you change selection (GM_Select), or when starting playback.
// NOTE: Requires audio device initialized (InitAudioDevice).
// music.h
static inline bool GM_LoadCurrent(GameState* gs, const GameMusic* lib) {
    const Album* a = GM_GetAlbum(lib, gs->currentAlbumIndex);
    const Song* s = GM_GetSong(a, gs->currentSongIndex);
    if (!s) return false;

    if (gs->currentMusicLoaded) {
        UnloadMusicStream(gs->currentMusic);
        gs->currentMusicLoaded = false;
    }

    // Helpful: verify the file is there for better logs
    bool exists = FileExists(s->file);
    if (!exists) {
        TraceLog(LOG_ERROR, "GM_LoadCurrent: file missing: %s", s->file);
        return false;
    }

    const int maxAttempts = 7;
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        gs->currentMusic = LoadMusicStream(s->file);
        if (gs->currentMusic.frameCount > 0) {
            SetMusicVolume(gs->currentMusic, gs->musicVol);
            gs->currentMusicLoaded = true;
            return true;
        }

        TraceLog(LOG_WARNING, "GM_LoadCurrent: open failed (attempt %d/%d) for %s",
            attempt + 1, maxAttempts, s->file);
        // Yield briefly to let the file manager/decoder catch up
        WaitTime(0.05); // 50 ms
    }

    TraceLog(LOG_ERROR, "GM_LoadCurrent: giving up on %s", s->file);
    return false;
}



#endif // MUSIC_H
