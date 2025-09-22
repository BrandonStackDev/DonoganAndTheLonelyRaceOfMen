#ifndef MENU_H
#define MENU_H

// --- deps ---
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// your stuff
#include "game.h"       // GameState gGame (invY, invX, musicVol, soundVol, diff, currentMusic, etc.)
#include "donogan.h"    // Donogan (pos, health/mana/max, level, xp)
#include "interact.h"   // missions[], fires[], counts, names, desc  
#include "collision.h"
#include "core.h"
#include "items.h" //items[]

// --- config ---
#ifndef MENU_VISIBLE_ROWS
#define MENU_VISIBLE_ROWS 10
#endif

// Colors
Color MENU_BG;
Color MENU_PANEL;
Color MENU_BORDER;
Color MENU_TEXT;
Color MENU_SELECT;
Color MENU_OK;
Color MENU_DIM;

void InitMenu()
{
    // Colors
    MENU_BG = (Color){ 20,  20,  20, 220 };
    MENU_PANEL = (Color){ 235, 235, 235, 255 };
    MENU_BORDER = BLACK;
    MENU_TEXT = WHITE;
    MENU_SELECT = YELLOW;
    MENU_OK = GREEN;
    MENU_DIM = GRAY;
}

// layout
static inline Rectangle MenuPanelRect(void) {
    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();
    const int pad = 16;
    const int w = (int)(sw * 0.42f);   // big left panel
    const int h = sh - pad * 2 - 100;    // leave room at top/btm
    return (Rectangle) { pad, pad + 60, (float)w, (float)h };
}

// We store these few fields in GameState (see step #2 patch)
static inline bool   Menu_IsOpen(const GameState* gs) { return gs->menuOpen; }
static inline void   Menu_Open(GameState* gs) { gs->menuOpen = true;  gs->menuPage = MENU_PAGE_MAIN; gs->menuSel = 0; gs->menuScroll = 0; }
static inline void   Menu_Close(GameState* gs) { gs->menuOpen = false; }
static inline void   Menu_Toggle(GameState* gs) { if (gs->menuOpen) Menu_Close(gs); else Menu_Open(gs); }

// helpers
static inline float clampf(float v, float lo, float hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }
static inline void  Menu_ApplyVolumes(GameState* gs) {
    gs->musicVol = clampf(gs->musicVol, 0.0f, 1.0f);
    gs->soundVol = clampf(gs->soundVol, 0.0f, 1.0f);
    if (gs->currentMusicLoaded) SetMusicVolume(gs->currentMusic, gs->musicVol);  // immediate feedback  :contentReference[oaicite:2]{index=2}
    SetMasterVolume(gs->soundVol); // affects all SFX
}

static inline void  Menu_DrawHeader(const char* title, Rectangle panel) {
    DrawRectangleLines((int)panel.x - 4, (int)panel.y - 38, (int)panel.width + 8, 34, MENU_BORDER);
    DrawRectangle((int)panel.x - 2, (int)panel.y - 36, (int)panel.width + 4, 30, MENU_PANEL);
    DrawTextEx(GetFontDefault(), title, (Vector2) { panel.x + 10, panel.y - 34 }, 22.0f, 1.0f, BLACK);
}

static inline void  Menu_DrawBox(Rectangle panel) {
    DrawRectangleLines((int)panel.x - 4, (int)panel.y - 4, (int)panel.width + 8, (int)panel.height + 8, MENU_BORDER);
    DrawRectangleRec(panel, MENU_PANEL);
}

static inline void  Menu_DrawRow(const char* text, int rowIndex, int sel, Rectangle panel, Color base, bool completed) {
    const float lh = 28.0f;
    const float y = panel.y + 8 + rowIndex * lh;
    Color c = (rowIndex == sel) ? MENU_SELECT : (completed ? MENU_OK : base);
    DrawTextEx(GetFontDefault(), text, (Vector2) { panel.x + 12, y }, 24.0f, 1.0f, c);
}

static inline void  Menu_ScrollClamp(int count, int* sel, int* scroll) {
    if (count <= 0) { *sel = *scroll = 0; return; }
    if (*sel < 0) *sel = 0;
    if (*sel >= count) *sel = count - 1;
    int vis = MENU_VISIBLE_ROWS;
    if (*sel < *scroll) *scroll = *sel;
    if (*sel >= *scroll + vis) *scroll = *sel - (vis - 1);
    if (*scroll < 0) *scroll = 0;
    if (*scroll > (count > vis ? count - vis : 0)) *scroll = (count > vis ? count - vis : 0);
}

//write position for placing things to file:
bool RecordPositionForPlacement(Donogan* d)
{
    FILE* f = fopen("placement.txt", "a"); //create if doesnt exist, append
    if (!f) return false;
    // Donogan pos to file, so we can place things easier...
    fprintf(f, "don_pos = %.2f, %.2f, %.2f\n", d->pos.x, d->pos.y, d->pos.z);
    fclose(f);
    PlaySoundVol(menuSaveOrLoad);
    return true;
}
// ---------------- Save / Load (text) ----------------
// Format: simple key=val lines + section markers for fireplaces & missions.
// Overwrites file completely on Save (as requested).
bool SaveGameToFile(char* path, GameState* gs, Donogan* d)
{
    FILE* f = fopen(path ? path : "don.save.txt", "wb");
    if (!f) return false;

    // Header / version
    fprintf(f, "DON_SAVE_V1\n");

    // Donogan stats (position first)
    fprintf(f, "don_pos = %.3f %.3f %.3f\n", d->pos.x, d->pos.y, d->pos.z);
    fprintf(f, "don_bow = %d\n", d->hasBow);
    fprintf(f, "don_health = %d\n", d->health);
    fprintf(f, "don_mana   = %d\n", d->mana);
    fprintf(f, "don_starthealth = %d\n", d->maxHealth);
    fprintf(f, "don_startmana   = %d\n", d->maxMana);
    fprintf(f, "don_level  = %d\n", d->level);
    fprintf(f, "don_xp     = %d\n", d->xp);

    // GameState
    fprintf(f, "invY = %d\n", gs->invY ? 1 : 0);
    fprintf(f, "invX = %d\n", gs->invX ? 1 : 0);
    fprintf(f, "musicVol = %.4f\n", gs->musicVol);
    fprintf(f, "soundVol = %.4f\n", gs->soundVol);
    fprintf(f, "difficulty = %d\n", (int)gs->diff);

    // Fireplaces (with explicit section markers so list can grow)
    fprintf(f, "--FIREPLACES-BEGIN--\n");
    for (int i = 0; i < FIREPIT_TOTAL_COUNT; i++) {                //
        fprintf(f, "FIRE[%d] = %d\n", i, fires[i].lit ? 1 : 0);    //
    }
    fprintf(f, "--FIREPLACES-END--\n");

    // Missions
    fprintf(f, "--MISSIONS-BEGIN--\n");
    for (int i = 0; i < MISSION_TOTAL_COUNT; i++) {                //
        fprintf(f, "MISSION[%d] = %d\n", i, missions[i].complete ? 1 : 0);  //
    }
    fprintf(f, "--MISSIONS-END--\n");

    //tracked items
    fprintf(f, "--ITEMS-BEGIN--\n");
    for (int i = 0; i < NUM_TRACKED_ITEMS; i++) {                //
        fprintf(f, "ITEM[%d] = %d\n", i, map_tracked_items[i].collected ? 1 : 0);  //
    }
    fprintf(f, "--ITEM-END--\n");

    fclose(f);
    PlaySoundVol(menuSaveOrLoad);
    return true;
}

static bool LoadGameFromFile(const char* path, GameState* gs, Donogan* d)
{
    FILE* f = fopen(path ? path : "don.save.txt", "rb");
    if (!f) return false;

    char line[512]; bool inFires = false, inMissions = false, inItems = false;
    while (fgets(line, sizeof(line), f)) {
        // strip
        char* s = line;
        while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') ++s;
        if (!*s) continue;

        if (!strncmp(s, "--FIREPLACES-BEGIN--", 20)) { inFires = true;  inMissions = false; inItems = false; continue; }
        if (!strncmp(s, "--FIREPLACES-END--", 18)) { inFires = false; continue; }
        if (!strncmp(s, "--MISSIONS-BEGIN--", 18)) { inMissions = true; inFires = false;  inItems = false; continue; }
        if (!strncmp(s, "--MISSIONS-END--", 16)) { inMissions = false; continue; }
        if (!strncmp(s, "--ITEMS-BEGIN--", 20)) { inItems = true;  inMissions = false; inFires = false; continue; }
        if (!strncmp(s, "--ITEMS-END--", 18)) { inItems = false; continue; }

        if (inFires) {
            int idx = 0, val = 0;
            if (sscanf(s, "FIRE[%d] = %d", &idx, &val) == 2) {
                if (idx >= 0 && idx < FIREPIT_TOTAL_COUNT) fires[idx].lit = (val != 0);
            }
            continue;
        }
        if (inMissions) {
            int idx = 0, val = 0;
            if (sscanf(s, "MISSION[%d] = %d", &idx, &val) == 2) {
                if (idx >= 0 && idx < MISSION_TOTAL_COUNT) missions[idx].complete = (val != 0);
            }
            continue;
        }
        if (inMissions) {
            int idx = 0, val = 0;
            if (sscanf(s, "ITEM[%d] = %d", &idx, &val) == 2) {
                if (idx >= 0 && idx < NUM_TRACKED_ITEMS) map_tracked_items[idx].collected = (val != 0);
            }
            continue;
        }

        // scalars
        if (!strncmp(s, "don_pos", 7)) {
            float x, y, z; if (sscanf(s, "don_pos = %f %f %f", &x, &y, &z) == 3) { d->pos.x = x; d->pos.y = y; d->pos.z = z; }
        }
        else if (!strncmp(s, "don_bow", 7)) {
            sscanf(s, "don_bow = %d", &d->hasBow);
        }
        else if (!strncmp(s, "don_health", 10)) {
            sscanf(s, "don_health = %d", &d->health);
        }
        else if (!strncmp(s, "don_mana", 8)) {
            sscanf(s, "don_mana = %d", &d->mana);
        }
        else if (!strncmp(s, "don_starthealth", 15)) {
            sscanf(s, "don_starthealth = %d", &d->maxHealth);
        }
        else if (!strncmp(s, "don_startmana", 13)) {
            sscanf(s, "don_startmana = %d", &d->maxMana);
        }
        else if (!strncmp(s, "don_level", 9)) {
            sscanf(s, "don_level = %d", &d->level);
        }
        else if (!strncmp(s, "don_xp", 6)) {
            sscanf(s, "don_xp = %d", &d->xp);
        }
        else if (!strncmp(s, "invY", 4)) {
            int v = 0; sscanf(s, "invY = %d", &v); gs->invY = (v != 0);
        }
        else if (!strncmp(s, "invX", 4)) {
            int v = 0; sscanf(s, "invX = %d", &v); gs->invX = (v != 0);
        }
        else if (!strncmp(s, "musicVol", 8)) {
            sscanf(s, "musicVol = %f", &gs->musicVol);
        }
        else if (!strncmp(s, "soundVol", 8)) {
            sscanf(s, "soundVol = %f", &gs->soundVol);
        }
        else if (!strncmp(s, "difficulty", 10)) { int v = 0; sscanf(s, "difficulty = %d", &v); gs->diff = (Difficulty)v; }
    }
    fclose(f);

    // snap to ground after warps/loads
    d->groundY = GetTerrainHeightFromMeshXZ(d->pos.x, d->pos.z);
    DonSnapToGround(d);
    Menu_ApplyVolumes(gs);
    PlaySoundVol(menuSaveOrLoad);
    return true;
}

// ---------------- Input glue (edge-driven) ----------------
// Call these from your input loop ONLY WHEN Menu_IsOpen() == true.
static void Menu_OnUp(GameState* gs) { gs->menuSel--; }
static void Menu_OnDown(GameState* gs) { gs->menuSel++; }
static void Menu_OnLeft(GameState* gs) {
    if (gs->menuPage == MENU_PAGE_OPTIONS) {
        switch (gs->menuSel) {
        case 0: gs->invY = !gs->invY; break;
        case 1: gs->invX = !gs->invX; break;
        case 2: gs->musicVol -= 0.05f; Menu_ApplyVolumes(gs); break;
        case 3: gs->soundVol -= 0.05f; Menu_ApplyVolumes(gs); break;
        case 4: if (gs->diff > DIFF_EASY) gs->diff = (Difficulty)((int)gs->diff - 1); break;
        }
    }
}
static void Menu_OnRight(GameState* gs) {
    if (gs->menuPage == MENU_PAGE_OPTIONS) {
        switch (gs->menuSel) {
        case 0: gs->invY = !gs->invY; break;
        case 1: gs->invX = !gs->invX; break;
        case 2: gs->musicVol += 0.05f; Menu_ApplyVolumes(gs); break;
        case 3: gs->soundVol += 0.05f; Menu_ApplyVolumes(gs); break;
        case 4: if (gs->diff < DIFF_HARD) gs->diff = (Difficulty)((int)gs->diff + 1); break;
        }
    }
}

static bool Menu_SaveNow(GameState* gs, Donogan* d) { return SaveGameToFile("don.save.txt", gs, d); }
static bool Menu_LoadNow(GameState* gs, Donogan* d) { bool ok = LoadGameFromFile("don.save.txt", gs, d); return ok; } //ok...why?

static void Menu_OnTriangle(GameState* gs) {
    PlaySoundVol(menuBack);
    switch (gs->menuPage) {
    case MENU_PAGE_INVENTORY:
    case MENU_PAGE_OPTIONS:
    case MENU_PAGE_MISSIONS:
    case MENU_PAGE_MISSION_DETAIL:
    case MENU_PAGE_WARP:
        gs->menuPage = MENU_PAGE_MAIN; gs->menuSel = 0; gs->menuScroll = 0; break;
    case MENU_PAGE_MAIN:
    default:
        Menu_Close(gs);
        break;
    }
}

// helper for warp building: collect lit firepits into temp list
typedef struct { int idx; } _LitIdx;
static int _BuildLitFires(_LitIdx* out, int cap) {
    int n = 0;
    for (int i = 0; i < FIREPIT_TOTAL_COUNT; i++) if (fires[i].lit) { if (n < cap) out[n].idx = i; n++; }  // :contentReference[oaicite:8]{index=8}
    return n;
}

static void Menu_OnCross(GameState* gs, Donogan* d)
{
    PlaySoundVol(menuSelect);
    if (gs->menuPage == MENU_PAGE_MAIN) {
        static const char* entries[] = { "Inventory", "Save", "Load", "Options", "Missions", "Warp" };
        const int count = 6;
        Menu_ScrollClamp(count, &gs->menuSel, &gs->menuScroll);
        switch (gs->menuSel) {
        case 0: gs->menuPage = MENU_PAGE_INVENTORY; gs->menuSel = 0; gs->menuScroll = 0; break;
        case 1: Menu_SaveNow(gs, d); break;
        case 2: Menu_LoadNow(gs, d); break;
        case 3: gs->menuPage = MENU_PAGE_OPTIONS; gs->menuSel = 0; gs->menuScroll = 0; break;
        case 4: gs->menuPage = MENU_PAGE_MISSIONS; gs->menuSel = 0; gs->menuScroll = 0; break;
        case 5: gs->menuPage = MENU_PAGE_WARP; gs->menuSel = 0; gs->menuScroll = 0; break;
        }
        return;
    }

    if (gs->menuPage == MENU_PAGE_MISSIONS) {
        // enter detail for selected mission
        const int count = MISSION_TOTAL_COUNT;                                             // :contentReference[oaicite:9]{index=9}
        Menu_ScrollClamp(count, &gs->menuSel, &gs->menuScroll);
        gs->menuDetailIndex = gs->menuSel;
        gs->menuPage = MENU_PAGE_MISSION_DETAIL;
        return;
    }

    if (gs->menuPage == MENU_PAGE_WARP) {
        // warp to selected lit firepit
        _LitIdx tmp[256];
        int totalLit = _BuildLitFires(tmp, 256);
        if (totalLit <= 0) return;
        Menu_ScrollClamp(totalLit, &gs->menuSel, &gs->menuScroll);
        int fireIdx = tmp[gs->menuSel].idx;
        // +5 X/Z away from center so we don't land ON the pit
        d->pos.x = fires[fireIdx].pos.x + 5.0f;
        d->pos.z = fires[fireIdx].pos.z + 5.0f;                                           // :contentReference[oaicite:10]{index=10}
#ifdef GetTerrainHeightFromMeshXZ
        d->groundY = GetTerrainHeightFromMeshXZ(d->pos.x, d->pos.z);
#endif
        DonSnapToGround(d);                                                               // :contentReference[oaicite:11]{index=11}
        Menu_Close(gs);
        return;
    }

    // OPTIONS && MISSION_DETAIL have no Cross action (Options changes on Left/Right).
    //INVENTORY
}

// ---------------- Drawing ----------------
static void _DrawMain(GameState* gs) {
    Rectangle panel = MenuPanelRect();
    Menu_DrawHeader("Main Menu", panel);
    Menu_DrawBox(panel);

    static const char* entries[] = { "Inventory", "Save", "Load", "Options", "Missions", "Warp" };
    const int count = 6;
    Menu_ScrollClamp(count, &gs->menuSel, &gs->menuScroll);

    int row = 0;
    for (int i = gs->menuScroll; i < count && row < MENU_VISIBLE_ROWS; ++i, ++row) {
        Menu_DrawRow(entries[i], row, gs->menuSel - gs->menuScroll, panel, MENU_TEXT, false);
    }
}

static void _DrawInventory(GameState* gs) {
    Rectangle panel = MenuPanelRect();
    Menu_DrawHeader("Inventory", panel);
    Menu_DrawBox(panel);
    DrawTextEx(GetFontDefault(), "work in progress", (Vector2) { panel.x + 12, panel.y + 12 }, 24.0f, 1.0f, MENU_DIM);
}

static const char* _DiffName(Difficulty d) {
    switch (d) { case DIFF_EASY: return "Easy"; case DIFF_NORMAL: return "Normal"; default: return "Hard"; }
}

static void _DrawOptions(GameState* gs) {
    Rectangle panel = MenuPanelRect();
    Menu_DrawHeader("Options", panel);
    Menu_DrawBox(panel);

    const char* rows[5]; char buf2[64], buf3[64];
    snprintf(buf2, sizeof(buf2), "Music Volume: %.2f", gs->musicVol);
    snprintf(buf3, sizeof(buf3), "Sound Volume: %.2f", gs->soundVol);
    rows[0] = gs->invY ? "Invert Y: On" : "Invert Y: Off";
    rows[1] = gs->invX ? "Invert X: On" : "Invert X: Off";
    rows[2] = buf2; rows[3] = buf3;
    static char diffbuf[32]; snprintf(diffbuf, sizeof(diffbuf), "Difficulty: %s", _DiffName(gs->diff));
    rows[4] = diffbuf;

    const int count = 5;
    Menu_ScrollClamp(count, &gs->menuSel, &gs->menuScroll);

    int row = 0;
    for (int i = gs->menuScroll; i < count && row < MENU_VISIBLE_ROWS; ++i, ++row) {
        Menu_DrawRow(rows[i], row, gs->menuSel - gs->menuScroll, panel, MENU_TEXT, false);
    }

    DrawTextEx(GetFontDefault(), "Left/Right: change value  |  Triangle: back", (Vector2) { panel.x + 12, panel.y + panel.height - 28 }, 20.0f, 1.0f, MENU_DIM);
}

static void _DrawMissions(GameState* gs) {
    Rectangle panel = MenuPanelRect();
    Menu_DrawHeader("Missions", panel);
    Menu_DrawBox(panel);

    const int count = MISSION_TOTAL_COUNT;                                                // :contentReference[oaicite:12]{index=12}
    Menu_ScrollClamp(count, &gs->menuSel, &gs->menuScroll);

    int row = 0;
    for (int i = gs->menuScroll; i < count && row < MENU_VISIBLE_ROWS; ++i, ++row) {
        const char* nm = missions[i].name ? missions[i].name : "(unnamed)";               // :contentReference[oaicite:13]{index=13}
        bool done = missions[i].complete;
        Menu_DrawRow(nm, row, gs->menuSel - gs->menuScroll, panel, MENU_TEXT, done);
    }
    DrawTextEx(GetFontDefault(), "Cross: details   |   Triangle: back", (Vector2) { panel.x + 12, panel.y + panel.height - 28 }, 20.0f, 1.0f, MENU_DIM);
}

static void _DrawMissionDetail(GameState* gs) {
    Rectangle panel = MenuPanelRect();
    Menu_DrawHeader("Mission", panel);
    Menu_DrawBox(panel);

    int i = gs->menuDetailIndex;
    if (i < 0 || i >= MISSION_TOTAL_COUNT) { DrawTextEx(GetFontDefault(), "Invalid mission", (Vector2) { panel.x + 12, panel.y + 12 }, 24, 1, RED); return; } // :contentReference[oaicite:14]{index=14}
    const char* nm = missions[i].name ? missions[i].name : "(unnamed)";
    const char* ds = missions[i].desc ? missions[i].desc : "(no description)";
    DrawTextEx(GetFontDefault(), nm, (Vector2) { panel.x + 12, panel.y + 12 }, 26.0f, 1.0f, BLACK);
    // wrap description
    Rectangle textBox = (Rectangle){ panel.x + 12, panel.y + 48, panel.width - 24, panel.height - 60 };
    DrawTextBoxed(GetFontDefault(), ds, textBox, 22.0f, 2.0f, true, BLACK);
    DrawTextEx(GetFontDefault(), "Triangle: back", (Vector2) { panel.x + 12, panel.y + panel.height - 28 }, 20.0f, 1.0f, MENU_DIM);
}

static void _DrawWarp(GameState* gs) {
    Rectangle panel = MenuPanelRect();
    Menu_DrawHeader("Warp", panel);
    Menu_DrawBox(panel);

    _LitIdx tmp[256];
    int totalLit = _BuildLitFires(tmp, 256);
    if (totalLit <= 0) {
        DrawTextEx(GetFontDefault(), "No lit fireplaces yet.", (Vector2) { panel.x + 12, panel.y + 12 }, 24.0f, 1.0f, MENU_TEXT);
        DrawTextEx(GetFontDefault(), "Triangle: back", (Vector2) { panel.x + 12, panel.y + panel.height - 28 }, 20.0f, 1.0f, MENU_DIM);
        return;
    }

    Menu_ScrollClamp(totalLit, &gs->menuSel, &gs->menuScroll);

    int row = 0;
    for (int ri = gs->menuScroll; ri < totalLit && row < MENU_VISIBLE_ROWS; ++ri, ++row) {
        int i = tmp[ri].idx;
        const char* nm = fires[i].name ? fires[i].name : "Fireplace";                    // :contentReference[oaicite:15]{index=15}
        Menu_DrawRow(nm, row, gs->menuSel - gs->menuScroll, panel, MENU_TEXT, false);
    }

    DrawTextEx(GetFontDefault(), "Cross: warp  |  Triangle: back", (Vector2) { panel.x + 12, panel.y + panel.height - 28 }, 20.0f, 1.0f, MENU_DIM);
}

// Public draw entrypoint: call this from your 2D HUD pass (does NOT pause your world)
static void Menu_DrawOverlay(const GameState* gs, const Donogan* d)
{
    if (!gs->menuOpen) return;

    // Dim the left edge subtly so text pops
    DrawRectangle(0, 0, (int)(GetScreenWidth() * 0.46f), GetScreenHeight(), MENU_BG);

    GameState* mgs = (GameState*)gs; // mutate selection/scroll during draw if needed
    switch (gs->menuPage) {
    case MENU_PAGE_MAIN:            _DrawMain(mgs);           break;
    case MENU_PAGE_INVENTORY:       _DrawInventory(mgs);      break;
    case MENU_PAGE_OPTIONS:         _DrawOptions(mgs);        break;
    case MENU_PAGE_MISSIONS:        _DrawMissions(mgs);       break;
    case MENU_PAGE_MISSION_DETAIL:  _DrawMissionDetail(mgs);  break;
    case MENU_PAGE_WARP:            _DrawWarp(mgs);           break;
    default: break;
    }
}

#endif // MENU_H
