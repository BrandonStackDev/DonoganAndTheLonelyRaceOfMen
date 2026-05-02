//raylib
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

//#define RLIGHTS_IMPLEMENTATION    // <— add this line, or dont, it wouldnt change the fact I have to bum rides off of people!
#include "rlights.h"
//me
#include "whale.h"
#include "truck.h"
#include "control.h"
#include "jc.h" //homes (Jimmy Carter)
#include "fish.h"
#include "donogan.h"
#include "util.h"
#include "timer.h"
#include "interact.h"
#include "collision.h"
#include "core.h"
#include "models.h"
#include "music.h"   // song/album structs + helpers
#include "duct_tape.h"
#include "game.h"
#include "bg.h"
#include "npc.h"
#include "menu.h"
#include "platform.h"
#include "items.h"
#include "apples.h"
#include "texture.h"
#include "machine.h"
#include "frustum.h"
#include "shark.h"
#include "corn.h"
#include "garden.h"

//fairly standard things
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//for big report numbers
#include <stdint.h>
#include <inttypes.h>
//debugging
//#include <vld.h>


bool bugGenHappened = false;
LightningBug* GenerateLightningBugs(Vector3 cameraPos, int count, float maxDistance)
{
    LightningBug* bugs = (LightningBug*)malloc(sizeof(LightningBug) * count);
    if (!bugs) return NULL;
    BoundingBox box = {
        .min = (Vector3){ -0.25f, -0.25f, -0.25f },
        .max = (Vector3){  0.25f,  0.25f,  0.25f }
    };
    lastLBSpawnPosition = cameraPos;
    for (int i = 0; i < count; i++)
    {
        float angle = (float)GetRandomValue(0, 359) * DEG2RAD;
        float dist = ((float)GetRandomValue(10, 1000) * 0.001f) * maxDistance;
        float x = cameraPos.x + (cosf(angle) * dist);
        float z = cameraPos.z + (sinf(angle) * dist);
        bugs[i].angle = 0.0f;
        bugs[i].pos = (Vector3){ x, 0.0f, z }; // you'll set .y later
        bugs[i].pos.y = GetTerrainHeightFromMeshXZ(bugs[i].pos.x, bugs[i].pos.z);
        bugs[i].pos.y = bugs[i].pos.y + GetRandomValue(1, 10);
        if (bugs[i].pos.y < -5000) { bugs[i].pos.y = 500; }
        bugs[i].rate = GetRandomValue(0.1f, 10.01f);
        bugs[i].origBox = box;
        bugs[i].box = UpdateBoundingBox(box, bugs[i].pos);
    }

    return bugs;
}

void RegenerateLightningBugs(LightningBug* bugs, Vector3 cameraPos, int count, float maxDistance)
{
    BoundingBox box = {
        .min = (Vector3){ -0.25f, -0.25f, -0.25f },
        .max = (Vector3){  0.25f,  0.25f,  0.25f }
    };
    lastLBSpawnPosition = cameraPos;
    //if (!bugs) return NULL;
    for (int i = 0; i < count; i++)
    {
        float angle = (float)GetRandomValue(0, 359) * DEG2RAD;
        float dist = ((float)GetRandomValue(10, 1000) * 0.001f) * maxDistance;

        float x = cameraPos.x + cosf(angle) * dist;
        float z = cameraPos.z + sinf(angle) * dist;
        bugs[i].angle = 0.0f;
        bugs[i].pos = (Vector3){ x, 0.0f, z }; // you'll set .y later
        bugs[i].pos.y = GetTerrainHeightFromMeshXZ(bugs[i].pos.x, bugs[i].pos.z);
        bugs[i].pos.y = bugs[i].pos.y + GetRandomValue(1, 10);
        if (bugs[i].pos.y < -5000) { bugs[i].pos.y = 500; }
        bugs[i].rate = GetRandomValue(0.1f, 10.01f);
        bugs[i].origBox = box;
        bugs[i].box = UpdateBoundingBox(box, bugs[i].pos);
    }
    // return bugs;
}

void UpdateLightningBugs(LightningBug* bugs, int count, float deltaTime)
{
    for (int i = 0; i < count; i++)
    {
        // === XZ movement ===
        float speed = 0.26f; // units per second
        float randDelt = (float)((float)GetRandomValue(0, 10)) / 187.0f;
        float randDeltZ = (float)((float)GetRandomValue(0, 10)) / 188.0f;
        bugs[i].pos.x += cosf(bugs[i].angle) * speed * (deltaTime + randDelt);
        bugs[i].pos.z += sinf(bugs[i].angle) * speed * (deltaTime + randDeltZ);

        // Drift the angle slightly (wander)
        float angleWander = ((float)GetRandomValue(-50, 50) / 360.0f) * PI; // small random
        bugs[i].angle += angleWander;

        // === Y movement ===
        float verticalSpeed = bugs[i].rate * deltaTime;
        bugs[i].pos.y += verticalSpeed;

        // Optional: bounce up/down within limits (simple floating)
        if (bugs[i].pos.y > 2.5f || bugs[i].pos.y < 0.5f) {
            bugs[i].rate *= -1.0f; // invert direction
        }
        bugs[i].box = UpdateBoundingBox(bugs[i].origBox, bugs[i].pos);

        //new stuff
        bugs[i].timer -= deltaTime;
        if (bugs[i].timer <= 0.0f)
        {
            // 25% chance this bug blinks
            if (GetRandomValue(0, 99) < 23)
            {
                bugs[i].alpha = 1.0f;
            }
            bugs[i].timer = 1.0f + (GetRandomValue(0, 100) / 100.0f); // reset 1–2 sec
        }
        // fade out
        if (bugs[i].alpha > 0.0f)
        {
            bugs[i].alpha -= deltaTime * 2.0f; // fade fast
            if (bugs[i].alpha < 0.0f) bugs[i].alpha = 0.0f;
        }
    }
}

static GameMusic gMusic;
static float gSongPrevPlayed = 0.0f;

// Edge detection for D-pad (so we act on "press", not "hold")
static int prevDpadUp = 0, prevDpadDown = 0, prevDpadLeft = 0, prevDpadRight = 0;

static void Audio_SelectAlbumRelative(int delta)
{
    if (gMusic.albumCount <= 0) return;

    int a = (gGame.currentAlbumIndex < 0) ? 0 : gGame.currentAlbumIndex;
    a = (a + delta + gMusic.albumCount) % gMusic.albumCount;

    // Reset to first track on album change
    GM_Select(&gGame, &gMusic, a, 0);
    if (GM_LoadCurrent(&gGame, &gMusic)) {
        PlayMusicStream(gGame.currentMusic);
        gSongPrevPlayed = 0.0f;  // <-- add
    }
}

static void Audio_SelectSongRelative(int delta)
{
    const Album* alb = GM_GetAlbum(&gMusic, gGame.currentAlbumIndex);
    if (!alb || alb->songCount <= 0) return;

    int s = (gGame.currentSongIndex < 0) ? 0 : gGame.currentSongIndex;
    s = (s + delta + alb->songCount) % alb->songCount;

    GM_Select(&gGame, &gMusic, gGame.currentAlbumIndex, s);
    if (GM_LoadCurrent(&gGame, &gMusic)) {
        PlayMusicStream(gGame.currentMusic);
        gSongPrevPlayed = 0.0f;  // <-- add
    }
}
static inline void PushDonnyOutOfMachineWorldBoxes(Donogan* d)
{
    if (!d) return;

    const float SKIN = 0.03f;

    for (int i = 0; i < gMachineCount; i++)
    {
        Machine* m = &gMachines[i];
        if (!m->visible) continue;

        BoundingBox a = d->box;
        BoundingBox b = m->worldBox;

        // small vertical forgiveness, but this is X/Z wall collision only
        b.min.y -= 0.5f;
        b.max.y += 0.5f;

        if (!CheckCollisionBoxes(a, b)) continue;

        Vector3 ac = {
            (a.min.x + a.max.x) * 0.5f,
            (a.min.y + a.max.y) * 0.5f,
            (a.min.z + a.max.z) * 0.5f
        };

        Vector3 bc = {
            (b.min.x + b.max.x) * 0.5f,
            (b.min.y + b.max.y) * 0.5f,
            (b.min.z + b.max.z) * 0.5f
        };

        Vector3 ah = {
            (a.max.x - a.min.x) * 0.5f,
            (a.max.y - a.min.y) * 0.5f,
            (a.max.z - a.min.z) * 0.5f
        };

        Vector3 bh = {
            (b.max.x - b.min.x) * 0.5f,
            (b.max.y - b.min.y) * 0.5f,
            (b.max.z - b.min.z) * 0.5f
        };

        Vector3 diff = Vector3Subtract(ac, bc);

        float penX = (ah.x + bh.x) - fabsf(diff.x);
        float penZ = (ah.z + bh.z) - fabsf(diff.z);

        if (penX < penZ)
        {
            d->pos.x += (diff.x >= 0.0f) ? penX + SKIN : -penX - SKIN;
        }
        else
        {
            d->pos.z += (diff.z >= 0.0f) ? penZ + SKIN : -penZ - SKIN;
        }

        d->box = UpdateBoundingBox(d->origBB, d->pos);
        d->innerBox = UpdateBoundingBox(d->origInnerBB, d->pos);
        d->outerBox = UpdateBoundingBox(d->origOuterBB, d->pos);
    }
}
static BoundingBox MakeCottageDoorBox(Vector3 center)
{
    return (BoundingBox) {
        { center.x - 10.0f, center.y - 8.0f, center.z - 1.67f },
        { center.x + 10.0f, center.y + 8.0f, center.z + 1.67f }
    };
}
static inline bool IsInCaveMode(Donogan* d, bool caveMode)
{
    if (!d) return false;

    // Special trigger boxes, like Cinder Spire entrance/drop zone
    for (int i = 0; i < gEnvBoundingBoxCount; i++)
    {
        if (gEnvBoundingBoxes[i].disable) continue;
        if (gEnvBoundingBoxes[i].type != EBBT_CAVE_START) continue;

        if (CheckCollisionBoxes(d->box, gEnvBoundingBoxes[i].box))
        {
            return true;
        }
    }
    if (caveMode && CheckCollisionBoxes(d->box, Scenes[SCENE_CINDER].box))
    {
        return true;
    }
    if (caveMode && CheckCollisionBoxes(d->box, Scenes[SCENE_CINDER_CAVE].box)) { return true;}

    return false;
}

static int storeSel = 0;

static inline bool Store_CanSell(InventoryType type)
{
    return type != INV_BOOK && type != INV_EVIL_BOOK;
}

static inline int Store_GetPrice(InventoryType type)
{
    switch (type)
    {
    case INV_BERRY:       return 20;
    case INV_HEALTH:      return 35;
    case INV_POTION:      return 45;
    case INV_APPLE:       return 65;
    case INV_HEALTH_FULL: return 95;
    case INV_RX:          return 120;
    default:              return 50;
    }
}

static void Store_MoveSel(int dir)
{
    storeSel += dir;

    if (storeSel < 0) storeSel = INV_TOTAL_TYPES - 1;
    if (storeSel >= INV_TOTAL_TYPES) storeSel = 0;

    // skip books
    int guard = 0;
    while (!Store_CanSell(inventory[storeSel].type) && guard++ < INV_TOTAL_TYPES)
    {
        storeSel += dir;
        if (storeSel < 0) storeSel = INV_TOTAL_TYPES - 1;
        if (storeSel >= INV_TOTAL_TYPES) storeSel = 0;
    }
}

static void Store_BuySelected(Donogan* d)
{
    if (!d) return;

    InventoryItem* it = &inventory[storeSel];
    if (!Store_CanSell(it->type)) return;

    int price = Store_GetPrice(it->type);

    
    if (it->type == INV_BERRY) // SELL BERRY
    {
        if (it->count > 0)
        {
            it->count--;
            d->money += price;
            PlaySoundVolContinuousAllowed(menuSaveOrLoad);
        }
        else
        {
            toast = "Not enough berries.";
            StartTimer(&toastTimer);
        }
    }
    else if (it->type == INV_APPLE)  // SELL APPLE
    {
        if (it->count > 0)
        {
            it->count--;
            d->money += price;
            PlaySoundVolContinuousAllowed(menuSaveOrLoad);
        }
        else
        {
            toast = "Not enough apples.";
            StartTimer(&toastTimer);
        }
    }
    else if (d->money >= price)
    {
        d->money -= price;
        it->count++;

        toast = "Purchased item!";
        StartTimer(&toastTimer);
        PlaySoundVolContinuousAllowed(menuSaveOrLoad);
    }
    else
    {
        toast = "Not enough money.";
        StartTimer(&toastTimer);
    }
}

static void FinishTalking(Donogan* d)
{
    d->isTalking = false;
    StartTimer(&d->talkStartTimer);

    if (d->who == TALK_TYPE_DARREL)
    {
        npcs[NPC_DARREL].state = DARREL_STATE_CONFUSED;
    }
    else if (d->who == TALK_TYPE_LUCY_ONE || d->who == TALK_TYPE_LUCY_TWO)
    {
        npcs[NPC_LUCY].state = LUCY_STATE_HELLO;
    }
    else if (d->who == TALK_TYPE_NICK)
    {
        npcs[NPC_NICK].state = DARREL_STATE_CONFUSED;
    }
    else if (d->who == TALK_TYPE_WIZARD)
    {
        d->talkedToBlueWizard = true;
        npcs[NPC_WIZARD].state = WIZARD_STATE_FLY;
        npcs[NPC_WIZARD].targetPos = (Vector3){ 8008, 8008, 8008 };
    }
    //abby not needed, galdriel not needed?, new guys not needed
}

static void Abby_GiveMedicine(Donogan* d)
{
    if (!d) return;

    if (missions[MISSION_ABBY_RX].complete)
    {
        FinishTalking(d);
        return;
    }

    if (inventory[INV_RX].count <= 0)
    {
        toast = "You do not have medicine.";
        StartTimer(&toastTimer);
        FinishTalking(d);
        return;
    }

    inventory[INV_RX].count--;

    missions[MISSION_ABBY_RX].complete = true;

    d->xp += 100;
    d->money += 5;//abby is poor

    toast = "Completed mission! Abby has the medicine.";
    StartTimer(&toastTimer);

    d->who = TALK_TYPE_ABBY_4;
    Talk_Reset(d->who);
}

#define GALADRIEL_BOOK_GOAL 10
#define GALADRIEL_BOOK_PRICE 80.0f

static TALK_TYPE Galadriel_GetTalkType(Donogan* d)
{
    if (!d) return TALK_TYPE_GAL_1;

    if (d->galBooksGiven >= GALADRIEL_BOOK_GOAL)
    {
        return TALK_TYPE_GAL_3;
    }

    if (inventory[INV_BOOK].count > 0)
    {
        return TALK_TYPE_GAL_2;
    }

    return TALK_TYPE_GAL_1;
}

char galToast[128];
static void Galadriel_GiveBooks(Donogan* d)
{
    if (!d) return;

    if (d->galBooksGiven >= GALADRIEL_BOOK_GOAL)
    {
        d->who = TALK_TYPE_GAL_3;
        Talk_Reset(d->who);
        return;
    }

    int needed = GALADRIEL_BOOK_GOAL - d->galBooksGiven;
    int available = inventory[INV_BOOK].count;

    if (available <= 0)
    {
        toast = "You do not have any good books.";
        StartTimer(&toastTimer);
        FinishTalking(d);
        return;
    }

    int giveCount = available;
    if (giveCount > needed) giveCount = needed;

    inventory[INV_BOOK].count -= giveCount;
    d->galBooksGiven += giveCount;
    d->money += (float)giveCount * GALADRIEL_BOOK_PRICE;

    snprintf(galToast, sizeof(galToast), "Sold %d good book%s to Galadriel.", giveCount, giveCount == 1 ? "" : "s");
    toast = galToast;
    StartTimer(&toastTimer);
    PlaySoundVolContinuousAllowed(menuSaveOrLoad);

    if (d->galBooksGiven >= GALADRIEL_BOOK_GOAL)
    {
        d->galBooksGiven = GALADRIEL_BOOK_GOAL;
        d->hasGuitar = true;

        toast = "Galadriel gave you the Guitar!";
        StartTimer(&toastTimer);

        d->who = TALK_TYPE_GAL_3;
        Talk_Reset(d->who);
    }
    else
    {
        FinishTalking(d);
    }
}

static bool Don_BoxHitsSceneMeshAtPos(Donogan* d, int sceneIndex, Vector3 testPos)
{
    BoundingBox testOuter = UpdateBoundingBox(d->origOuterBB, testPos);

    const float groundSlopeCos = DEFAULT_GROUND_SLOPE_COS;

    MeshBoxHit testHit = CollideAABBWithMeshTriangles(
        testOuter,
        &HomeModels[Scenes[sceneIndex].modelType].meshes[0],
        Scenes[sceneIndex].pos,
        Scenes[sceneIndex].scale,
        Scenes[sceneIndex].yaw,
        groundSlopeCos,
        true
    );

    return testHit.hit && !testHit.hitGround;
}
static bool Don_HistoryIsWiggingOut(Donogan* d)
{
    float path = 0.0f;
    for (int hb = 1; hb < DON_POS_HISTORY_MAX; hb++)
    {
        Vector3 a = Don_GetHistoryPosition(d, hb);
        Vector3 b = Don_GetHistoryPosition(d, hb + 1);
        a.y = b.y = 0.0f;
        path += Vector3Distance(a, b);
    }

    Vector3 newest = Don_GetHistoryPosition(d, 1);
    Vector3 oldest = Don_GetHistoryPosition(d, DON_POS_HISTORY_MAX);
    newest.y = oldest.y = 0.0f;

    float net = Vector3Distance(newest, oldest);

    return (path > 4.0f && net < path * 0.35f);
}


/// @brief main!
/// @param  
/// @return 
int main(void) {
    static bool prevOptions = false;
    toast = "";
    toastTimer = CreateTimer(8);
    StartTimer(&toastTimer);
    bool devDisplay = false;
    //SetTraceLogLevel(LOG_ALL);
    SetTraceLogLevel(LOG_WARNING);
    MUTEX_INIT(mutex);
    bool displayBoxes = false;
    bool displayLod = false;
    bool dropped_firepits = false;
    bool caveMode = false;
    bool wasCaveMode = false;
    LightningBug *bugs;
    Star *stars;
    bool vehicleMode = false;
    bool hoverMode = false;
    bool prevHoverR3 = false;
    float hoverLift = 0.0f;        // current visual/physics lift
    float hoverLiftTarget = 0.0f;  // 0 normal, 3-ish hover
    float hoverTireFold = 0.0f;    // 0 normal tires, 1 flat tires
    // --- Donny mode state ---
    bool donnyMode = true;
    Vector3 donMove = (Vector3){ 0 };
    ControllerData gpad = { 0 };
    bool havePad = false;
    int prevCross = 0;
    int prevTri = 0;
    bool prevTalkX = false;
    bool prevTalkTri = false;
    float moveMag = 0.0f;
    // --- Third-person orbit camera state (around don.pos) ---
    float yaw = 0.0f, pitch = 0.25f, radius = 14.0f;
    int oldLevel = 0;
    //day-night timer
    Timer nightTimer = CreateTimer(128); //128 seconds, just above 2 minutes
    //bool to mark if we loading corn yet
    bool madeCorn = false;
    
    //idk
    int pad_axis = 0;
    bool mouse = false;
    int gamepad = 0; // which gamepad to display
    
    //chase camera
    Vector3 cameraTargetPos = { 0 };
    Vector3 cameraOffset = { 0.0f, 6.0f, -14.0f };
    float camYaw = 0.0f;   // Left/right
    float camPitch = 15.0f; // Up/down, slightly above by default
    float camDistance = 14.0f;  // Distance from truck
    float relativeYaw = 0.0f;  // <-- instead of camYaw
    float relativePitch = 0.0f;  // <-- instead of camYaw
    //for the cottage door
    bool cottageDoorOpen = false;
    float cottageDoorSlide = 0.0f; // 0 closed, 1 open
    Vector3 cottageDoorClosed = { -600, 806.0f, 2863.95f };
    Vector3 cottageDoorOpenPos = { -617, 806.0f, 2863.95f };
    BoundingBox cottageDoorBox;
    //int for model type to search for when pressing R
    int modelSearchType = 0;
    //---------------RAYLIB INIT STUFF---------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Donogan And The Lonely Race Of Men");
    //game state
    gGame.HonkedHornRecently = CreateTimer(60);//60 seconds
    gGame.diff = DIFF_NORMAL; // default to normal
    gGame.menuTimer = CreateTimer(0.22f);
    StartTimer(&gGame.menuTimer);
    //audio
    SetAudioStreamBufferSizeDefault(4096);
    InitAudioDevice();
    GameMusic_Init(&gMusic);
    GameState_InitAudio(&gGame);
    GM_Select(&gGame, &gMusic, 1, 0);         // default: Second album, first track, (Garden -> Arketized)
    if (GM_LoadCurrent(&gGame, &gMusic)) {
        SetMusicVolume(gGame.currentMusic, gGame.musicVol);
        PlayMusicStream(gGame.currentMusic);
    }
    //get sound clips
    carHorn = LoadSound("sounds/horn.mp3");
    donScream = LoadSound("sounds/scream.mp3");
    menuSelect = LoadSound("sounds/select.mp3");
    menuBack = LoadSound("sounds/back.mp3");
    menuSaveOrLoad = LoadSound("sounds/save.mp3");
    grow = LoadSound("sounds/grow.mp3");
    pick = LoadSound("sounds/pick.mp3");
    wrenchSound = LoadSound("sounds/wrench.mp3");
    sharkGulp = LoadSound("sounds/shark_gulp.mp3");
    //enable the cursor
    EnableCursor();//now that we default to donny boy, lets not capture the mouse
    SetTargetFPS(60);
    //icon
    Image icon = LoadImage("res/icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    //npc
    InitAllNPC();
    //load the homes models/scenes and stuff like that
    InitHomes();
    //water wheel
    WaterWheel_Init();
    //talking
    InitTalkingInteractions();
    //missions
    InitMissions();
    //lasers
    LasersInit();
    //apples
    InitApples();
    //machines and lift
    Machine_Init();
    //maps
    InitMaps();
    //env bounding boxes, duct tape
    GoGoGadgetDuctTape();
    //rocket ship
    Model rocketModel = LoadModel("models/rocket.obj");
    rocketModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/rocket.png");
    Vector3 rocketPos = (Vector3){ 2556.61, 732.95, 542.69 };
    BoundingBox rocketBox = UpdateBoundingBox(GetModelBoundingBox(rocketModel), rocketPos);
    //other stuff
    Rectangle talk_contain = { 25.0f, 160.0f, (SCREEN_WIDTH/2.0f) - 50.0f, (SCREEN_HEIGHT) - 250.0f};
    Rectangle res_contain = { (SCREEN_WIDTH / 2.0f) + 25, 160.0f, (SCREEN_WIDTH / 2.0f) - 50.0f, (SCREEN_HEIGHT) - 250.0f};
    Font default_font = GetFontDefault();
    Font req_font = LoadFontEx("res/Tangerine/Tangerine-Bold.ttf", 32, 0, 250);
    Font res_font = LoadFontEx("res/Lexend/static/Lexend-SemiBold.ttf", 15, 0, 250);
    //omg spaghetti code lol
    bool disableDonInputNextFrame = false;
    ////PLATS-----------------------------------------------------
    InitPlats();
    //items
    InitItems();
    ////whales---------------------------------------------------
    int numWhales = 9; // six whales right now
    Whale* whales = (Whale*)malloc(sizeof(Whale) * numWhales);
    //init whale struct
    whales[0] = (Whale){ 0 };
    InitWhale(&whales[0], (Vector3) { 1513, 245, 4951 }, 205, WHALE_SURFACE);
    if (!LoadWhale(&whales[0])) { return 1; }
    whales[1] = (Whale){ 0 };
    InitWhale(&whales[1], (Vector3) { -4498, 150, 6150 }, 50, WHALE_SURFACE);
    if (!LoadWhale(&whales[1])) { return 1; }
    whales[2] = (Whale){ 0 };
    InitWhale(&whales[2], (Vector3) { 6000, 150, 6000 }, 50, WHALE_SURFACE);
    if (!LoadWhale(&whales[2])) { return 1; }
    whales[3] = (Whale){ 0 };
    InitWhale(&whales[3], (Vector3) { -6000, 80, -6000 }, 50, WHALE_SURFACE);
    if (!LoadWhale(&whales[3])) { return 1; }
    whales[4] = (Whale){ 0 };
    InitWhale(&whales[4], (Vector3) { 6000, 130, -6000 }, 50, WHALE_SURFACE);
    if (!LoadWhale(&whales[4])) { return 1; }
    whales[5] = (Whale){ 0 };
    InitWhale(&whales[5], (Vector3) { 2782.00, 140, -2063.00 }, 80, WHALE_SURFACE);
    if (!LoadWhale(&whales[5])) { return 1; }
    whales[6] = (Whale){ 0 };
    InitWhale(&whales[6], (Vector3) { -4320.00, 256, 43 }, 190, WHALE_SURFACE);
    if (!LoadWhale(&whales[6])) { return 1; }
    whales[7] = (Whale){ 0 };
    InitWhale(&whales[7], (Vector3) { -3504.00, 267, -1533.00 }, 230, WHALE_SURFACE);
    if (!LoadWhale(&whales[7])) { return 1; }
    whales[8] = (Whale){ 0 };
    InitWhale(&whales[8], (Vector3) { -3273, 190, -4415.00 }, 110, WHALE_SURFACE);
    if (!LoadWhale(&whales[8])) { return 1; }
    ////end whales setup-----------------------------------------
        // fish
    // --- FISH SETUP --------------------------------------------------------------
    Model fishModel = LoadModel("models/fish.obj");
    fishModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    fishModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/fish.png");

    // School allocation + seeding
    int numSchools = 15;
    int schoolCount = 42;
    // First allocate the top-level array of pointers (one per school)
    School* fish = MemAlloc(sizeof(School) * numSchools);

    // For each school, allocate its own array of Fish structs
    for (int s = 0; s < numSchools; s++) {
        fish[s].fish = MemAlloc(sizeof(Fish) * schoolCount);
        fish[s].schoolCount = schoolCount;
        fish[s].schoolRadius = 25.8f;
    }

    // A target the school will “want” to wander toward
    fish[0].center = (Vector3){ 1513, 235, 4951 };
    fish[1].center = (Vector3){ -4498, 145, 6150 };
    fish[2].center = (Vector3){ 6000, 150, 6000 };
    fish[3].center = (Vector3){ -6000, 100, -6000 };
    fish[4].center = (Vector3){ 6000, 130, -6000 };
    fish[5].center = (Vector3){ 4467.84f, 160, 4253.51f };
    fish[6].center = (Vector3){ 4467.84f, 200, 4253.51f };
    fish[7].center = (Vector3){ 1821.18f, 220, 3307.20f };
    fish[8].center = (Vector3){ 3183.93f, 220, 5489.83f };
    fish[9].center = (Vector3){ -2541.69, 278, 714.14 };
    fish[10].center = (Vector3){ 2782.14, 100, -2063.78 };
    fish[11].center = (Vector3){ 2782.00, 140, -2063.00 };
    fish[12].center = (Vector3){ 2780.00, 180, -2060.00 };
    fish[13].center = (Vector3){ 1055, 260, 3170 };
    fish[14].center = (Vector3){ 623, 200, 4204 };

    for (int s = 0; s < numSchools; s++) {
        fish[s].schoolCount = schoolCount;
        fish[s].schoolRadius = 25.8f;
        for (int i = 0; i < schoolCount; i++) {
            float a = ((float)GetRandomValue(0, 360)) * DEG2RAD;
            float r = (float)GetRandomValue(0, 1000) / 1000.0f * fish[s].schoolRadius;
            Vector3 c = fish[s].center; // <- use the chosen school center
            fish[s].fish[i].pos = (Vector3){ c.x + sinf(a) * r, c.y + GetRandomValue(-5,5) * 0.2f, c.z + cosf(a) * r };
            fish[s].fish[i].yawDeg = (float)GetRandomValue(0, 359);
            fish[s].fish[i].scale = 1.0f;  // bump to 3–10 if your model is tiny
            fish[s].fish[i].vel = (Vector3){ 0 };
        }
        fish[s].fishTarget = fish[s].center;
    }
    // ---------------------------------------------------------------------------

        // - shaders
    Shader heightShaderLight = LoadShader("shaders/120/height_color_lighting.vs", "shaders/120/height_color_lighting.fs");
    int mvpLocLight = GetShaderLocation(heightShaderLight, "mvp");
    int modelLocLight = GetShaderLocation(heightShaderLight, "model");
    float strengthLight = 0.25f;
    SetShaderValue(heightShaderLight, GetShaderLocation(heightShaderLight, "slopeStrength"), &strengthLight, SHADER_UNIFORM_FLOAT);
    // Set standard locations
    heightShaderLight.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(heightShaderLight, "mvp");
    heightShaderLight.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(heightShaderLight, "model");
    // Set light direction manually
    Vector3 lightDir = (Vector3){ -10.2f, -100.0f, -10.3f };
    int lightDirLoc = GetShaderLocation(heightShaderLight, "lightDir");
    SetShaderValue(heightShaderLight, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
        // - 
    Shader waterShader = LoadShader("shaders/120/water.vs", "shaders/120/water.fs");
    int timeLoc = GetShaderLocation(waterShader, "uTime");
    int offsetLoc = GetShaderLocation(waterShader, "worldOffset");
    gWaterShader = waterShader;
    gWaterTimeLoc = timeLoc;
    gWaterOffsetLoc = offsetLoc;
    // Load lighting shader---------------------------------------------------------------------------------------
    Shader instancingLightShader = LoadShader("shaders/100/lighting_instancing.vs","shaders/100/lighting.fs");
    // Get shader locations
    instancingLightShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(instancingLightShader, "mvp");
    instancingLightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(instancingLightShader, "viewPos");
    // Create one light
    Light instanceLight = CreateLight(LIGHT_DIRECTIONAL, LightPosDraw, LightTargetDraw, lightColorDraw, instancingLightShader);
    //grass shader
    Shader grassInstancingLightShader = LoadShader("shaders/100/grass_lighting_instancing.vs", "shaders/100/grass_lighting.fs");
    // Get shader locations
    grassInstancingLightShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(grassInstancingLightShader, "mvp");
    grassInstancingLightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(grassInstancingLightShader, "viewPos");
    // Create one light (distinct var name!)
    Light grassLight = CreateLight(LIGHT_DIRECTIONAL, LightPosDraw, LightTargetDraw, lightColorDraw, grassInstancingLightShader);
    int grassTimeLoc = GetShaderLocation(grassInstancingLightShader, "uTime");
    //init the static game props stuff
    InitStaticGameProps(instancingLightShader, grassInstancingLightShader);//get the high fi models ready
    fishModel.materials[0].shader = instancingLightShader; //use this (regular not grassy) guy for fish as well
    //END -- lighting shader---------------------------------------------------------------------------------------
    //START -- lightning bug shader :)---------------------------------------------------------------------------------------
    // Load PBR shader and setup all required locations
    Shader lightningBugShader = LoadShader("shaders/120/lightning_bug.vs","shaders/120/lightning_bug.fs");
    lightningBugShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(lightningBugShader, "mvp");
    //--stars
    Shader starShader = LoadShader("shaders/120/lighting_star.vs","shaders/120/lighting_star.fs");
    starShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(starShader, "mvp");
    //START LIGHTNING BALL SHADERS
    Shader lightningBall = LoadShader("shaders/120/lightning_ball.vs", "shaders/120/lightning_ball.fs");
    lightningBall.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(lightningBall, "mvp");
    lightningBall.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(lightningBall, "model");
    int lbTimeLoc = GetShaderLocation(lightningBall, "uTime");
    int lbViewLoc = GetShaderLocation(lightningBall, "uViewPos");
    int lbColorLoc = GetShaderLocation(lightningBall, "uColor");
    int lbIntLoc = GetShaderLocation(lightningBall, "uIntensity");
    int lbDispLoc = GetShaderLocation(lightningBall, "uDispAmp");
    int lbNoiseLoc = GetShaderLocation(lightningBall, "uNoiseScale");
    Mesh ballMesh = GenMeshSphere(0.5f, 24, 24);
    Model ball = LoadModelFromMesh(ballMesh);
    ball.materials[0].shader = lightningBall;
    //ghost shader
    Shader ghostShader = LoadShader("shaders/330/ghost.vs", "shaders/330/ghost.fs");
    ghostShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(ghostShader, "mvp");
    ghostShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(ghostShader, "matModel");
    ghostShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(ghostShader, "matNormal");
    int locGhostTime = GetShaderLocation(ghostShader, "u_time");
    //bg
    InitBadGuys(ghostShader);
    //tol bloom
    InitBloomSystem(instancingLightShader);
    bool genesis = false;
    // --- Firepit shader + model ---
    Shader fireShader = LoadShader("shaders/120/fire.vs", "shaders/120/fire.fs");
    fireShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(fireShader, "mvp");
    fireShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(fireShader, "model");
    int fireTimeLoc = GetShaderLocation(fireShader, "uTime");
    int fireVariantLoc = GetShaderLocation(fireShader, "uVariant");
    // a small sphere we stretch upward in the vertex shader
    Mesh fireMesh = GenMeshSphere(0.6f, 18, 18);
    Model fireModel = LoadModelFromMesh(fireMesh);
    fireModel.materials[0].shader = fireShader;
    // end all shaders

    //tree model //todo: replace with bounding boxes for reals in the models.h stuff
    Model treeCubeModel;
    treeCubeModel = LoadModelFromMesh(GenMeshCube(0.67f, 16.0f, 0.67f));
    treeCubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = DARKGREEN;
    BoundingBox treeOrigBox = GetModelBoundingBox(treeCubeModel);
    //game map
    Texture2D mapTexture;
    bool showMap = true;
    float mapZoom = 1.0f;
    Rectangle mapViewport = { SCREEN_WIDTH - GAME_MAP_SIZE - 10, 10, 128, 128 };  // Map position + size
    mapTexture = LoadMyTexture("map/treasure_map.png"); //mapTexture = LoadMyTexture("map/elevation_color_map.png");
    //shark
    Shark shark = { 0 };
    InitShark(&shark, (Vector3) { 3875.50, 196.84, 3838.98 }, PLAYER_FLOAT_Y_POSITION);
    if (!LoadShark(&shark)) return 1;
    //controller and truck and donny
    // //donny boy
    Color targetHitColor = (Color){ 200,220,250,100 };
    Donogan don = InitDonogan();
    don.gs = &gGame;
    don.pos = Scenes[SCENE_HOME_CABIN_02].pos;
    don.pos.y = 533.333f;
    StartTimer(&don.talkStartTimer); //so we can talk right away;
    Don_UpdateBoxes(&don);
    Don_ResetPositionHistory(&don);
    //windmill Rotor
    Model rotor = LoadModel("models/rotor.obj");
    rotor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/rotor.png");
    Vector3 rotorOffset = { 1.0f, 12.0f, 18.0f };
    float rotorSpin = 0; //degrees + dt, then convert to radians
    //tree of Life
    Model tol = LoadModel("models/tree_of_life.obj");
    tol.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/tree_of_life.png");
    Vector3 tolPos = {-334.0f, 564.0f, -497.35f};
    //atreyu
    Model atreyu = LoadModel("models/atreyu.obj");
    atreyu.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/atreyu.png");
    Vector3 atreyuPos = { -2167.0f, 816.0f, 1416.00f };
    Model canoe = LoadModel("models/canoe.obj");
    canoe.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/canoe.png");
    Vector3 canoePos = { -1419.90f, 306.32f, 1718.38f };
    //wrench before you have it
    Model wrenchModel = LoadModel("models/wrench.obj");
    wrenchModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/wrench.png");
    Vector3 wrenchPos = (Vector3){ 875.34, 357.0, 1353.11 };
    BoundingBox wrenchBox = UpdateBoundingBox(GetModelBoundingBox(wrenchModel), wrenchPos);
    //heads
    Texture don_head = LoadMyTexture("textures/don_head.png");
    Texture tol_head = LoadMyTexture("textures/tol_head.png");
    Texture atreyu_head = LoadMyTexture("textures/atreyu_head.png");
    Texture darrel_head = LoadMyTexture("textures/darrel_head.png");
    Texture lucy_head = LoadMyTexture("textures/lucy_head.png");
    Texture wiz_head = LoadMyTexture("textures/wiz_head.png");
    Texture abby_head = LoadMyTexture("textures/abby_head.png");
    Texture clerk_head = LoadMyTexture("textures/clerk_head.png");
    Texture gal_head = LoadMyTexture("textures/gal_head.png");
    Texture roger_head = LoadMyTexture("textures/roger_head.png");
    Texture geoff_head = LoadMyTexture("textures/geoff_head.png");
    Texture mary_head = LoadMyTexture("textures/mary_head.png");
    //truck
    InitTruck();
    //more lb stuff
    Mesh sphereMesh = GenMeshHemiSphere(0.108f,8, 8);
    Material sphereMaterial = LoadMaterialDefault();
    sphereMaterial.maps[MATERIAL_MAP_DIFFUSE].color = (Color){50,200,100,200};
    sphereMaterial.shader = lightningBugShader;
    Mesh sphereStarMesh = GenMeshHemiSphere(6.2f,3, 3);
    Material sphereStarMaterial = LoadMaterialDefault();
    sphereStarMaterial.maps[MATERIAL_MAP_DIFFUSE].color = (Color){80,80,150,230};
    sphereStarMaterial.shader = starShader;
    Vector4 starColorVecs[4];
    for (int i = 0; i < 4; i++)
    {
        starColorVecs[i] = (Vector4) {
            starColors[i].r / 255.0f,
            starColors[i].g / 255.0f,
            starColors[i].b / 255.0f,
            1.0f
        };
    }
    float instanceIDs[STAR_COUNT];
    for (int i = 0; i < STAR_COUNT; i++) {
        instanceIDs[i] = (float)i;
    }
    int idAttribLoc = GetShaderLocationAttrib(starShader, "instanceId");
    SetShaderValueV(starShader, idAttribLoc, instanceIDs, SHADER_ATTRIB_FLOAT, STAR_COUNT);
    
    //END -- lighting bug shader---------AND STARS!------------------------------------------------------------------------------
    skyboxTint = skyboxDay;
    //skybox stuff
    skyboxPanelMesh = GenMeshCube(1.0f, 1.0f, 0.01f); // very flat panel
    skyboxPanelFrontModel = LoadModelFromMesh(skyboxPanelMesh);
    skyboxPanelBackModel = LoadModelFromMesh(skyboxPanelMesh);
    skyboxPanelLeftModel = LoadModelFromMesh(skyboxPanelMesh);
    skyboxPanelRightModel = LoadModelFromMesh(skyboxPanelMesh);
    skyboxPanelUpModel = LoadModelFromMesh(skyboxPanelMesh);
    Texture2D skyTexFront, skyTexBack, skyTexLeft, skyTexRight, skyTexUp;
    skyTexFront = LoadMyTexture("skybox/sky_front_smooth.png");
    skyTexBack  = LoadMyTexture("skybox/sky_back_smooth.png");
    skyTexLeft  = LoadMyTexture("skybox/sky_left_smooth.png");
    skyTexRight = LoadMyTexture("skybox/sky_right_smooth.png");
    skyTexUp    = LoadMyTexture("skybox/sky_up_smooth.png");
    skyboxPanelFrontModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexFront;
    skyboxPanelBackModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexBack;
    skyboxPanelLeftModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexLeft;
    skyboxPanelRightModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexRight;
    skyboxPanelUpModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexUp;
    // for (int cy = 0; cy < CHUNK_COUNT; cy++) {
    //     for (int cx = 0; cx < CHUNK_COUNT; cx++) {
    //         if (!chunks[cx][cy].isLoaded && !chunks[cx][cy].isReady) {
    //             //PreLoadMyTexture(cx, cy);
    //             //LoadChunk(cx, cy);
    //         }
    //     }
    // }
    //fireplaces
    Model firepit = LoadModel("models/firepit.obj");
    firepit.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/firepit.png");
    fires[FIREPIT_HOME].location = FIREPIT_HOME;
    fires[FIREPIT_HOME].pos = (Vector3){ 3022.00f, 319.00f, 4042.42f };
    fires[FIREPIT_HOME].name = "Home";
    fires[FIREPIT_CASTLE].location = FIREPIT_CASTLE;
    fires[FIREPIT_CASTLE].pos = (Vector3){ 2619.73f, 481, 1385 };
    fires[FIREPIT_CASTLE].name = "Castle";
    fires[FIREPIT_VILLAGE].location = FIREPIT_VILLAGE;
    fires[FIREPIT_VILLAGE].pos = (Vector3){ -3743, 327, 1069 };
    fires[FIREPIT_VILLAGE].name = "Village";
    fires[FIREPIT_COTTAGE].location = FIREPIT_COTTAGE;
    fires[FIREPIT_COTTAGE].pos = (Vector3){ -1214.70, 400, 664 };
    fires[FIREPIT_COTTAGE].name = "Cottage";
    fires[FIREPIT_BARN].location = FIREPIT_BARN;
    fires[FIREPIT_BARN].pos = (Vector3){ 922.00f, 353.00f, 2026.00f };
    fires[FIREPIT_BARN].name = "Barn/Windmill";
    fires[FIREPIT_YETI_MT].location = FIREPIT_YETI_MT;
    fires[FIREPIT_YETI_MT].pos = (Vector3){ -649.22, 790.59, 2887.02 };
    fires[FIREPIT_YETI_MT].name = "Yeti Mountain";
    fires[FIREPIT_RAP].location = FIREPIT_RAP;
    fires[FIREPIT_RAP].pos = (Vector3){ 788.23, 877.18, -1849.37 };
    fires[FIREPIT_RAP].name = "Reach Around Point";
    fires[FIREPIT_WRENVILLE].location = FIREPIT_WRENVILLE;
    fires[FIREPIT_WRENVILLE].pos = (Vector3){ -2624.41, 406.59, -2408.02 };
    fires[FIREPIT_WRENVILLE].name = "Town of Wrenville";
    fires[FIREPIT_CINDERSPIRE].location = FIREPIT_CINDERSPIRE;
    fires[FIREPIT_CINDERSPIRE].pos = (Vector3){ -342.71, 649.72, -1065.31 };
    fires[FIREPIT_CINDERSPIRE].name = "Cinderspire";
    fires[FIREPIT_ELYNDOR_ISLAND].location = FIREPIT_ELYNDOR_ISLAND;
    fires[FIREPIT_ELYNDOR_ISLAND].pos = (Vector3){ -397.66, 323.76, -4040.06 };
    fires[FIREPIT_ELYNDOR_ISLAND].name = "Elyndor Island";
    // INIT INTERACTIVE POINTS OF INTEREST
    InteractivePoints[POI_TYPE_TRUCK] = (POI){ POI_TYPE_TRUCK , &truckPosition};
    InteractivePoints[POI_TYPE_TREE_OF_LIFE] = (POI){ POI_TYPE_TREE_OF_LIFE , &tolPos };
    InteractivePoints[POI_TYPE_ATREYU] = (POI){ POI_TYPE_ATREYU , &atreyuPos };
    InteractivePoints[POI_TYPE_DARREL] = (POI){ POI_TYPE_DARREL , &npcs[NPC_DARREL].pos};
    InteractivePoints[POI_TYPE_CHICKEN] = (POI){ POI_TYPE_CHICKEN , &npcs[NPC_CHICKEN].pos };
    InteractivePoints[POI_TYPE_LUCY] = (POI){ POI_TYPE_LUCY , &npcs[NPC_LUCY].pos };
    InteractivePoints[POI_TYPE_NICK] = (POI){ POI_TYPE_NICK , &npcs[NPC_NICK].pos }; //rescue mission
    InteractivePoints[POI_TYPE_WIZARD] = (POI){ POI_TYPE_WIZARD , &npcs[NPC_WIZARD].pos }; //wiz
    InteractivePoints[POI_TYPE_ABBY] = (POI){ POI_TYPE_ABBY , &npcs[NPC_ABBY].pos }; //abby
    InteractivePoints[POI_TYPE_STORE_1] = (POI){ POI_TYPE_STORE_1, &npcs[NPC_CLERK].pos };// store POI points to the clerk
    InteractivePoints[POI_TYPE_GALADRIEL] = (POI){ POI_TYPE_GALADRIEL, &npcs[NPC_GALADRIEL].pos }; //galadriel
    InteractivePoints[POI_TYPE_ROGER] = (POI){ POI_TYPE_ROGER, &npcs[NPC_ROGER].pos }; //roger
    InteractivePoints[POI_TYPE_GEOFF] = (POI){ POI_TYPE_GEOFF, &npcs[NPC_GEOFF].pos }; //geoff
    InteractivePoints[POI_TYPE_MARY] = (POI){ POI_TYPE_MARY, &npcs[NPC_MARY].pos }; //mary
    //help maps know things like the important people
    map_tol = &tolPos;
    map_atreyu = &atreyuPos;
    map_gal = &npcs[NPC_GALADRIEL].pos;
    Corn_Init(instancingLightShader);
    //init the stuff before launching thread launcher
    InitMenu(&don);//just for some color stuff
    //INIT
    //----------------------init chunks---------------------
    chunks = malloc(sizeof(Chunk *) * CHUNK_COUNT);
    for (int i = 0; i < CHUNK_COUNT; i++) chunks[i] = calloc(CHUNK_COUNT, sizeof(Chunk));
    if (!chunks) {
        TraceLog(LOG_ERROR, "Failed to allocate chunk row pointers");
        exit(1);
    }

    for (int x = 0; x < CHUNK_COUNT; x++) {
        chunks[x] = malloc(sizeof(Chunk) * CHUNK_COUNT);
        if (!chunks[x]) {
            TraceLog(LOG_ERROR, "Failed to allocate chunk column %d", x);
            exit(1); // or clean up and fail gracefully
        }

        // Optional: clear/init each chunk
        for (int y = 0; y < CHUNK_COUNT; y++) {
            memset(&chunks[x][y], 0, sizeof(Chunk));
            chunks[x][y].water = NULL;chunks[x][y].waterCount = 0;//make sure water is ready to be checked and then instantiated
        }
    }
    //----------------------DONE -> init chunks---------------------
    //-----INIT TILES
    int maxTiles = ((CHUNK_COUNT * CHUNK_COUNT) * (TILE_GRID_SIZE * TILE_GRID_SIZE));  // = 16,384
    foundTiles = malloc(sizeof(TileEntry) * maxTiles);
    foundTileCount = 0;

    if (!foundTiles) {
        TraceLog(LOG_ERROR, "Out of memory allocating tile entry buffer");
        return -666;
    }
    //DONE INIT
    //lets get the water
    FILE *f = fopen("map/water_manifest.txt", "r"); // Open for append
    if (f != NULL) {
        //need to count the lines in the file and then set manifestTileCount
        int lines = 0;
        int c;
        while ((c = fgetc(f)) != EOF) {
            if (c == '\n') lines++;
        }
        waterManifestCount = lines;
        fclose(f);
        //OpenWaterObjects(waterShader);//water manifest is required right now
        StartWaterLoader();
    }
    //TODO: loop through each chunk, then each water feature for that chunk, set the sahder of the model
    //launch the initial loading background threads
    StartChunkLoader();
    //StartFileManger();
    StartCloseTileWorker();

    Camera3D camera = {
        .position = (Vector3){ 0.0f, 1400.0f, 0.0f },  // Higher if needed,
        .target = (Vector3){ 5000.0f, 120.0f, 7000.0f },  // Centered
        .up = (Vector3){ 0.0f, 1.0f, 0.0f },
        .fovy = 80.0f,
        .projection = CAMERA_PERSPECTIVE
    };
    Camera skyCam = camera;
    skyCam.position = (Vector3){ 0, 0, 0 };
    skyCam.target = (Vector3){ 0, 0, 1 };  // looking forward
    skyCam.up = (Vector3){0, 1, 0};
    skyCam.fovy = 60.0f;
    skyCam.projection = CAMERA_PERSPECTIVE;

    StartTimer(&don.hitTimer);
    long loop_counter = 0;
    while (!WindowShouldClose() && keepAlive)
    {
        //shut off in water if in vehicle
        if (!donnyMode) { don.inWater = false; }
        //maps
        if (onLoad)
        {
            //garden setup
            if (!genesis) { Garden_Init(); genesis = true; }
            //maps?
            int foundOn = -1;
            for (int i = 0; i < MAP_TOTAL_COUNT; i++) {
                if (!maps[i].collected) maps[i].display = false;

                if (maps[i].display) {
                    if (foundOn < 0) foundOn = i;
                    else maps[i].display = false;
                }
            }
            //corn
            if (!madeCorn)
            {
                // ---------- CORN FIELDS ----------
                    // Big square split into 4 smaller fields
                Vector3 bigCornCenter = (Vector3){ -2229.12f, 489.41f, -1807.96f };
                float bigCornSizeX = 220.0f;
                float bigCornSizeZ = 180.0f;

                // use smaller measurement so it stays square-ish and does not overreach
                float bigCornSize = fminf(bigCornSizeX, bigCornSizeZ);

                float gap = 14.0f;
                float halfField = (bigCornSize - gap) * 0.5f;
                float quarterOffset = (halfField + gap) * 0.5f;

                Corn_AddFieldBox(
                    (Vector3) {
                    bigCornCenter.x - quarterOffset, 0.0f, bigCornCenter.z - quarterOffset
                },
                    halfField, halfField,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );

                Corn_AddFieldBox(
                    (Vector3) {
                    bigCornCenter.x + quarterOffset, 0.0f, bigCornCenter.z - quarterOffset
                },
                    halfField, halfField,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );

                Corn_AddFieldBox(
                    (Vector3) {
                    bigCornCenter.x - quarterOffset, 0.0f, bigCornCenter.z + quarterOffset
                },
                    halfField, halfField,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );

                Corn_AddFieldBox(
                    (Vector3) {
                    bigCornCenter.x + quarterOffset, 0.0f, bigCornCenter.z + quarterOffset
                },
                    halfField, halfField,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );


                // Small square as the 5th field
                Vector3 smallCornCenter = (Vector3){ -2658.74f, 394.61f, -2495.44f };
                float smallCornSizeX = 80.0f;
                float smallCornSizeZ = 65.0f;

                float smallCornSize = fminf(smallCornSizeX, smallCornSizeZ);

                Corn_AddFieldBox(
                    smallCornCenter,
                    smallCornSize, smallCornSize,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );
                madeCorn = true;

                Corn_AddFieldBox(
                    (Vector3){-2232.95, 448.00, -2192.04},
                    200, 210,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );
                madeCorn = true;

                Corn_AddFieldBox(
                    (Vector3){2727.29, 416.75, 1916.11},
                    200, 200,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );

                Corn_AddFieldBox(
                    (Vector3){2137.45, 338.82, 4824.10},
                    200, 200,
                    4.0f, 1.2f,
                    2.8f, 3.6f
                );
                madeCorn = true;
            }
        }
        if (don.talkedToBlueWizard && npcs[NPC_WIZARD].state != WIZARD_STATE_DONE)
        {
            npcs[NPC_WIZARD].state = WIZARD_STATE_FLY;
            npcs[NPC_WIZARD].targetPos = (Vector3){ 8008, 8008, 8008 };
        }
        //mostly things that need save file updates
        if (don.unlockedTruck) 
        { 
            gTruckLiftAtFloor = true;
            gTruckLiftHeight = -5.0;
        }
        for (int mi = 0; mi < MACHINE_COUNT_WINDMILL; mi++)
        {
            if (gMachines[mi].active)
            {
                if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_01) { plats[1].disabled = false; } //castle close by itself, 1st one
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_10) { plats[21].disabled = false; plats[36].disabled = false; plats[40].disabled = false;} //by barn
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_02) { plats[56].disabled = false; } //by donogans
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_03) { plats[57].disabled = false; } //farside castle spaceshipmode
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_08) { plats[65].disabled = false; } //treehouse by windmill
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_04) { plats[70].disabled = false; } //pair
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_05) { plats[71].disabled = false; } //pair
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_06) { plats[76].disabled = false; plats[111].disabled = false; } //bridge
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_09) { cottageDoorOpen = true; gEnvBoundingBoxes[26].disable = true; } //yetimt
                else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_11) { plats[123].disabled = false; } //waterwheel
            }
        }
        
        //detect general missions for completion (the non talking/interaction triggered missions)
        if (onLoad && !missions[MISSION_START_ALL_MILLS].complete)
        {
            int milCnt = 0;
            for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
            {
                if (Scenes[i].modelType == MODEL_HOME_WINDMILL && Scenes[i].active) { milCnt++; }
            }
            if (milCnt >= MACHINE_COUNT_WINDMILL) //number of mills
            {
                missions[MISSION_START_ALL_MILLS].complete = true;
                toast = "All Windmills Activated!";
                StartTimer(&toastTimer);
                don.xp += 400;
                don.money += 1000;
            }
        }
        if (!missions[MISSION_KILL_GHOST].complete && ghostKillCount >= 10)
        {
            toast = "Completed mission! You killed ten Ghosts!";
            StartTimer(&toastTimer);
            don.xp += 100;
            don.money += 600;
            missions[MISSION_KILL_GHOST].complete = true;
        }
        if (!missions[MISSION_KILL_YETI].complete && yetiKillCount >= 10)
        {
            toast = "Completed mission! You killed ten Yetis!";
            StartTimer(&toastTimer);
            don.xp += 150;
            don.money += 800;
            missions[MISSION_KILL_YETI].complete = true;
        }
        if (!missions[MISSION_KILL_ROBO].complete && roboKillCount >= 10)
        {
            toast = "Completed mission! You killed ten Robot Orbs!";
            StartTimer(&toastTimer);
            don.xp += 200;
            don.money += 1000;
            missions[MISSION_KILL_ROBO].complete = true;
        }
        //handle health and mana re-gen, xp to level conversion as well
        oldLevel = don.level;
        //don.level = (int)(120.0 * log(1.0 + don.xp / 200.0));
        don.level = (int)(110.0 * (1.0 - exp(-don.xp / 12000.0)));
        if (don.level != oldLevel && don.level%5==0) {
            don.maxHealth += 2;
            don.maxMana += 2;
            if (don.maxHealth > 160) { don.maxHealth = 160; }//cap em
            if (don.maxMana > 160) { don.maxMana = 160; }//cap em
        } //we just raised up, and then level is divisable by five, increase our health
        if (loop_counter % 8000 == 0)
        {
            don.health += 1; //regen
        }
        if (loop_counter % 1000 == 0)
        {
            don.mana += 1; //regen
        }
        if (don.health > don.maxHealth) { don.health = don.maxHealth; }
        if (don.health < 0) { don.health = 0; }
        if (don.mana > don.maxMana) { don.mana = don.maxMana; }
        if (don.mana < 0) { don.mana = 0; }
        //increment loop counter and rotor spin
        loop_counter++;
        rotorSpin += GetFrameTime() * 128;
        //
        DocumentCloseItems(&don);
        if (onLoad)
        {
            ConsumeSimpleItems(&don);
            ConsumeTrackedItems(&don);
        }
        //document close props
        if (onLoad)//tweak modulus and total size
        {
            int j = 0;
            for (int cy = 0; cy < CHUNK_COUNT; cy++) 
            {
                for (int cx = 0; cx < CHUNK_COUNT; cx++) 
                {
                    if (!chunks[cx][cy].isLoaded || chunks[cx][cy].lod != LOD_64) { continue; }
                    for (int i = 0; i < chunks[cx][cy].treeCount && j < MAX_CLOSE_PROPS; i++) 
                    {
                        //set the stuff
                        StaticGameObject tree = chunks[cx][cy].props[i];
                        if (tree.type == MODEL_GRASS || tree.type == MODEL_GRASS_THICK || tree.type == MODEL_GRASS_LARGE) { continue; }
                        Vector3 ps = { 0 };
                        if (donnyMode) { ps = don.pos; }
                        else if (vehicleMode) { ps = truckPosition; }
                        else { ps = camera.position; }
                        //test closeness
                        if (Vector3DistanceSqr(ps, tree.pos) < MAX_CLOSE_PROPS_DISTANCE * MAX_CLOSE_PROPS_DISTANCE)
                        {
                            CloseProps[j] = &chunks[cx][cy].props[i];
                            j++;
                        }
                    }
                }
            }
            numCloseProps = j;
        }
        //document active bg
        if (onLoad && loop_counter % 17 == 0)
        {
            int j = 0;
            for (int b = 0; b < bg_count && j < (MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT); b++)
            {
                if (bg[b].active) 
                {
                    act_bg[j] = b;
                    j++;
                }
            }
            act_bg_count = j;
        }
        if (loop_counter % 169 == 0)
        {
            TraceLog(LOG_INFO,"numCloseProps = %d", numCloseProps);
            TraceLog(LOG_INFO, "act_bg_count = %d", act_bg_count);
            TraceLog(LOG_INFO, "num_close_map_items = %d", num_close_map_items);
        }
        // init disable roll (if we touch a wall, do not allow roll)
        bool disableRoll = false;
        //auto flip day and night
        if (onLoad)
        {
            if (!nightTimer.running) { StartTimer(&nightTimer); }
            if (HasTimerElapsed(&nightTimer))
            {
                dayTime = !dayTime;
                ResetTimer(&nightTimer);
            }
        }
        if (onLoad && donnyMode)
        {
            don.oldPos = don.pos;
            Don_RecordPositionHistory(&don);
            if (Don_HistoryIsWiggingOut(&don))
            {
                disableDonInputNextFrame = true;
                don.rollVel = (Vector3){ 0 };
                don.velXZ = (Vector3){ 0 };
            }
        }
        //controller and truck stuff
        havePad = ReadControllerWindows(0, &gpad);
        UpdateTreeOfLifeBloomSpell(&don, &gpad, GetFrameTime());
        UpdateTreeOfLifeBloomGeneration();
        if (!onLoad) 
        { 
            float s_rx = gpad.normRX;
            float s_ry = gpad.normRY;
            gpad = (ControllerData){ 0 }; //dont let the player move around until the screen is loaded, but let them look
            gpad.normRX = s_rx;
            gpad.normRY = s_ry;
        }
        else if (HasTimerElapsed(&gGame.menuTimer) && gpad.btnStart) { Menu_Toggle(&gGame); StartTimer(&gGame.menuTimer); } //toggle the menu
        else if (HasTimerElapsed(&gGame.menuTimer) && gpad.btnSelect) { RecordPositionForPlacement(&don); StartTimer(&gGame.menuTimer); } //toggle the menu
        if (gGame.invY) { gpad.ry = -gpad.ry;  gpad.normRY = -gpad.normRY;}
        if (gGame.invX) { gpad.rx = -gpad.rx;  gpad.normRX = -gpad.normRX; }
        int tri = gpad.btnTriangle > 0;
        int cross = gpad.btnCross > 0;
        //music/start menu selection
        if (onLoad)
        {
            // --- D-pad album/song navigation (edge-triggered) ---
            int dUp = gpad.dpad_up > 0;     // adapt names if your ControllerData differs
            int dDown = gpad.dpad_down > 0;
            int dLeft = gpad.dpad_left > 0;
            int dRight = gpad.dpad_right > 0;
            if (don.isTalking && don.who == TALK_TYPE_STORE)
            {
                if (dUp && !prevDpadUp) Store_MoveSel(-1);
                if (dDown && !prevDpadDown) Store_MoveSel(1);

                if (cross && !prevCross)
                {
                    Store_BuySelected(&don);
                }

                if (tri && !prevTri)
                {
                    FinishTalking(&don);
                }
            }
            else if (don.isTalking)
            {
                // do nothing here; normal talk is handled in the talking block
            }
            else if (!Menu_IsOpen(&gGame))
            {
                if (dUp && !prevDpadUp)     Audio_SelectAlbumRelative(-1);
                if (dDown && !prevDpadDown) Audio_SelectAlbumRelative(+1);
                if (dLeft && !prevDpadLeft) Audio_SelectSongRelative(-1);
                if (dRight && !prevDpadRight) Audio_SelectSongRelative(+1);
            }
            else
            {
                if (dUp && !prevDpadUp)     Menu_OnUp(&gGame);
                if (dDown && !prevDpadDown) Menu_OnDown(&gGame);
                if (dLeft && !prevDpadLeft) Menu_OnLeft(&gGame);
                if (dRight && !prevDpadRight) Menu_OnRight(&gGame);
                if (cross && !prevCross)    Menu_OnCross(&gGame, &don);
                if (tri && !prevTri)        Menu_OnTriangle(&gGame);
                StartTimer(&truckInteractTimer);
            }

            prevDpadUp = dUp;
            prevDpadDown = dDown;
            prevDpadLeft = dLeft;
            prevDpadRight = dRight;
            prevDpadUp = dUp; prevDpadDown = dDown; prevDpadLeft = dLeft; prevDpadRight = dRight;
        }
        if (vehicleMode) 
        { 
            don.pos = truckPosition;
            donnyMode = false; 
        }//just make sure this is always exlusive or, one or the other, never both, and update his position for npc culling, so they appear
        if (!vehicleMode && donnyMode && onLoad)
        {
            if (!don.hasWrench && CheckCollisionBoxes(wrenchBox,don.outerBox)) //get the wrench
            { 
                don.hasWrench = true;
                toast = "Got the Wrench!";
                StartTimer(&toastTimer);
                PlaySoundVol(wrenchSound);
                don.xp += 50;
            }
            bool inBowCam = (don.bowMode || (don.bowReleaseCamHold > 0.0f)) && don.state != DONOGAN_STATE_BOW_EXIT;
            float dt = GetFrameTime();
            //as soon as we have dt, increment rotor spin
            // Right stick controls camera orbit (mouse RMB fallback also works)
            float rsx = havePad ? gpad.normRX : 0.0f;
            float rsy = havePad ? gpad.normRY : 0.0f;
            const float camStickSens = don.bowMode? 0.76f : 1.6f; // tweak as desired

            yaw += rsx * camStickSens * dt;
            float invert = contInvertY ? 1.0f : -1.0f;
            pitch += rsy * invert * camStickSens * dt;
            pitch = Clampf(pitch, -1.2f, 1.2f);

            // after you compute yaw/pitch for the camera:
            don.camPitch = pitch;
            // While aiming, lock the shot direction to the camera yaw
            //if (don.bowMode) {don.yawY = -yaw;}


            // Update camera position from yaw/pitch/radius
            // Defaults
            float baseRadius = 15.3f;
            float baseFov = 80.0f;
            float homeRad = 10.2f;

            // -- much smaller zoom while aiming --
            float zoomRadius = 13.2f;   // was 10.0f
            float zoomFov = 74.0f;   // was 62.0f

            float followRate = don.bowMode ? 10.0f : 6.0f;
            float zoomRate = 6.0f;
            float fovRate = 6.0f;

            // Base target = head
            Vector3 desiredTarget = (Vector3){ don.pos.x, don.pos.y + 2.6f, don.pos.z };

            if (inBowCam) {
                desiredTarget = (Vector3){ don.pos.x, don.pos.y + 0.555f, don.pos.z };
                // 1) compute spawn + aim
                Vector3 spawn = Vector3Add(don.pos, RotYawOffset(don.arrowOffset, don.yawY, 1, false));
                float gySpawn = GetTerrainHeightFromMeshXZ(spawn.x, spawn.z);
                if (spawn.y < gySpawn + 0.05f) spawn.y = gySpawn + 0.05f;

                Vector3 aimDir = DonAimForward(&don, 1.0f);

                // 2) focus near along aim (NOT the far hit)
                const float bowFocusMeters = 5.0f; // ~how far in front to focus
                const float bowAimInfluence = 0.30f; // how much to bias toward that focus
                Vector3 aimPointNear = Vector3Add(spawn, Vector3Scale(aimDir, bowFocusMeters));
                desiredTarget = Vector3Lerp(desiredTarget, aimPointNear, bowAimInfluence);

                // 3) composition: fixed meters in camera-space (not scaled by radius)
                float cy = cosf(yaw), sy = sinf(yaw);
                Vector3 camRight = (Vector3){ cy, 0.0f, -sy };
                Vector3 camUp = (Vector3){ 0.0f, 1.0f,  0.0f };

                const float compMetersX = 3.0f;   // push view right ⇒ Donny appears left
                const float compMetersY = 1.2f;  // push view down  ⇒ Donny appears lower
                desiredTarget = Vector3Add(desiredTarget,
                    Vector3Add(Vector3Scale(camRight, +compMetersX),
                        Vector3Scale(camUp, +compMetersY)));
            }

            // Smooth settle
            camera.target = Vector3Lerp(camera.target, desiredTarget, 1.0f - expf(-followRate * dt));

            don.camPitch = pitch;
            // While aiming, lock the shot direction to the camera yaw
            if (inBowCam) { don.yawY = yaw; }
            else if (don.state == DONOGAN_STATE_BOW_EXIT) { don.yawY = yaw + PI; don.bowMode = false; }

            float desiredRadius = inBowCam ? zoomRadius : don.inHome ? homeRad : baseRadius;
            float desiredFov = inBowCam ? zoomFov : baseFov;

            radius = Lerp(radius, desiredRadius, 1.0f - expf(-zoomRate * dt));
            camera.fovy = Lerp(camera.fovy, desiredFov, 1.0f - expf(-fovRate * dt));

            // orbit from target (unchanged)
            camera.position.x = camera.target.x + radius * cosf(pitch) * sinf(yaw);
            camera.position.y = camera.target.y + radius * sinf(pitch);
            camera.position.z = camera.target.z + radius * cosf(pitch) * cosf(yaw);
            if (!inBowCam && !caveMode)
            {
                float camGroundY = GetTerrainHeightFromMeshXZ(camera.position.x, camera.position.z);
                if (camGroundY + 0.6f > camera.position.y)
                {
                    camera.position.y = camGroundY + 0.6f;
                }
                if (!don.inWater && !don.gluedToPlatform && camera.position.y < don.pos.y)//prevent clipping(from backface culling) a bit more
                {
                    camera.position.y = don.pos.y;
                }
            }

            if (tri && !prevTri && !Menu_IsOpen(&gGame))//handle triangle interactions here
            {
                if (!don.isTalking && Machine_TryInteract(&don, don.pos, don.hasWrench) >= 0)
                {
                    int mi = Machine_FindInteractable(don.pos, MACHINE_INTERACT_DISTANCE);
                    if (mi >= 0)
                    {
                        Vector3 toMachine = Vector3Subtract(gMachines[mi].pos, don.pos);
                        toMachine.y = 0.0f;

                        if (Vector3LengthSqr(toMachine) > 0.0001f)
                        {
                            don.yawY = atan2f(toMachine.x, toMachine.z);
                        }
                        //section to handle enabling platforms that are disabled or other actions, whatever the mill powers
                        if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_01) { plats[1].disabled = false; } //castle close by itself, 1st one
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_10) { plats[21].disabled = false; plats[36].disabled = false;plats[40].disabled = false;} //by barn
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_02) { plats[56].disabled = false; } //by donogans
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_03) { plats[57].disabled = false; } //farside castle spaceshipmode
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_08) { plats[65].disabled = false; } //treehouse by windmill
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_04) { plats[70].disabled = false; } //pair
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_05) { plats[71].disabled = false; } //pair
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_06) { plats[76].disabled = false; plats[111].disabled = false; } //bridge
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_09) { cottageDoorOpen = true; gEnvBoundingBoxes[26].disable = true; } //yetimt
                        else if (gMachines[mi].scene_type == SCENE_HOME_WINDMILL_11) { plats[123].disabled = false; } //waterwheel
                    }
                    // placeholder toast / sound / animation trigger //play wrenchSound
                    if (HasTimerElapsed(&toastTimer))//do not overwrite other messages with this guy
                    {
                        toast = "Machine activated!"; 
                        StartTimer(&toastTimer);
                    }
                    //trigger animation, also create animation procedurally, and then put the wrench in his hand (not attached to a bone)
                    DonSetState(&don, DONOGAN_STATE_MACHINE_TURN);
                    don.velXZ = (Vector3){ 0 };
                    don.rollVel = (Vector3){ 0 };
                    don.bowMode = false;
                    //play sound
                    PlaySoundVol(wrenchSound);
                }
                if (!don.isTalking
                    && don.unlockedTruck
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_TRUCK].pos, don.pos) < 12.4 * 12.4
                    && HasTimerElapsed(&truckInteractTimer))
                {
                    StartTimer(&truckInteractTimer);
                    vehicleMode = true; donnyMode = false;
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking 
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_TREE_OF_LIFE].pos, don.pos) < 13*13
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    don.who = TALK_TYPE_TOL;
                    if (gBloom.bloomed)
                    {
                        don.who = TALK_TYPE_TOL_2;
                    }
                    StartTimer(&don.talkStartTimer);
                    Talk_Reset(don.who);
                    if (!missions[MISSION_FIND_TOL].complete)
                    {
                        toast = "Completed mission! You found The Tree of Life!";
                        StartTimer(&toastTimer);
                        don.xp += 150;
                        don.money += 500;
                        missions[MISSION_FIND_TOL].complete = true;
                    }
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_WIZARD].pos, don.pos) < 13*13
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    don.who = TALK_TYPE_WIZARD;
                    Talk_Reset(don.who);
                    StartTimer(&don.talkStartTimer);
                    npcs[NPC_WIZARD].state = WIZARD_STATE_TALK;
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_ATREYU].pos, don.pos) < 11.02 * 12
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    don.who = TALK_TYPE_ATREYU;
                    if (!missions[MISSION_FIND_ATREYU].complete)
                    {
                        don.who = TALK_TYPE_ATREYU_BOW;
                        toast = "Completed mission! You found Atreyu! Got the Bow!";
                        don.hasBow = true;
                        StartTimer(&toastTimer);
                        don.xp += 150;
                        don.money += 500;
                        missions[MISSION_FIND_ATREYU].complete = true;
                    }
                    StartTimer(&don.talkStartTimer);
                    Talk_Reset(don.who);
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_DARREL].pos, don.pos) < 11.44f * 12
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    don.who = TALK_TYPE_DARREL;
                    npcs[NPC_DARREL].state = DARREL_STATE_TALK;
                    StartTimer(&don.talkStartTimer);
                    Talk_Reset(don.who);
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_ROGER].pos, don.pos) < 11.44f * 12
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    don.who = TALK_TYPE_ROGER;
                    StartTimer(&don.talkStartTimer);
                    Talk_Reset(don.who);
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_GEOFF].pos, don.pos) < 11.44f * 12
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    don.who = TALK_TYPE_GEOFF;
                    StartTimer(&don.talkStartTimer);
                    Talk_Reset(don.who);
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_MARY].pos, don.pos) < 11.44f * 12
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    don.who = TALK_TYPE_MARY;
                    StartTimer(&don.talkStartTimer);
                    Talk_Reset(don.who);
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (Vector3DistanceSqr(*InteractivePoints[POI_TYPE_NICK].pos, don.pos) < 144
                    && !don.isTalking) //check !isTalking because we want to make sure we hit the exit talk routine if don is talking
                {
                    if (npcs[NPC_NICK].r_state == RESCUE_STATE_SCARED) //begin run
                    {
                        npcs[NPC_NICK].r_state = RESCUE_STATE_RUN;
                        npcs[NPC_NICK].state = DARREL_STATE_RUN;
                        npcs[NPC_NICK].targetPos = (Vector3){ 2846.52, 323.76, -615.60 };
                        //jump right to the run start frame
                        npcs[NPC_NICK].curAnim = npcs[NPC_NICK].state;
                        npcs[NPC_NICK].animFrame = 0.0f;
                        //mission stuff for rescusing nick
                        toast = "Completed mission! You Rescued Nick!";
                        StartTimer(&toastTimer);
                        don.xp += 100;
                        don.money += 700;
                        missions[MISSION_RESCUE_NICK].complete = true;
                    }
                    else if (npcs[NPC_NICK].r_state == RESCUE_STATE_SAFE //talk
                        && !don.isTalking 
                        && HasTimerElapsed(&don.talkStartTimer))
                    {
                        //interact talk stuff for nick
                        don.isTalking = true;
                        don.who = TALK_TYPE_NICK;
                        npcs[NPC_NICK].state = DARREL_STATE_TALK;
                        StartTimer(&don.talkStartTimer);
                        Talk_Reset(don.who);
                    }
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_LUCY].pos, don.pos) < 144
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;
                    if (Vector3DistanceSqr(npcs[NPC_CHICKEN].pos, npcs[NPC_LUCY].pos) < 40*40 || missions[MISSION_CLARENCE_CHICKEN].complete)
                    {
                        if (!missions[MISSION_CLARENCE_CHICKEN].complete)
                        {
                            toast = "Completed mission! Clarence is home with Lucy";
                            StartTimer(&toastTimer);
                            don.xp += 100;
                            don.money += 500;
                            missions[MISSION_CLARENCE_CHICKEN].complete = true;
                            //make clarence not follow us anymore
                            npcs[NPC_CHICKEN].state = CHICKEN_STATE_PLAN;
                            npcs[NPC_CHICKEN].tether = npcs[NPC_CHICKEN].pos;
                        }
                        don.who = TALK_TYPE_LUCY_TWO;
                        Talk_Reset(don.who);
                    }
                    else
                    {
                        don.who = TALK_TYPE_LUCY_ONE;
                        Talk_Reset(don.who);
                    }
                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                    npcs[NPC_LUCY].state = LUCY_STATE_TALK;
                    StartTimer(&don.talkStartTimer);//debounce triangle
                }
                else if (!missions[MISSION_CLARENCE_CHICKEN].complete //clarence the chicken mission
                    && HasTimerElapsed(&don.interactionLimitTimer)
                    && HasTimerElapsed(&don.talkStartTimer) //make sure talking to lucy doesnt trigger this on the next pass
                    && (npcs[NPC_CHICKEN].state == CHICKEN_STATE_FOLLOW || Vector3DistanceSqr(*InteractivePoints[POI_TYPE_CHICKEN].pos, don.pos) < 144))
                {
                    StartTimer(&don.interactionLimitTimer);//make sure we dont toggle the state of the chicken really fast
                    if (npcs[NPC_CHICKEN].state != CHICKEN_STATE_FOLLOW)
                    {
                        npcs[NPC_CHICKEN].state = CHICKEN_STATE_FOLLOW;//enter follow mode
                        toast = "Clarence is following you";
                        StartTimer(&toastTimer);
                        TraceLog(LOG_INFO, "Chicken follow");
                    }
                    else //exit follow mode
                    {
                        toast = "Clarence is no longer following you";
                        StartTimer(&toastTimer);
                        npcs[NPC_CHICKEN].state = CHICKEN_STATE_PLAN;
                        npcs[NPC_CHICKEN].tether = npcs[NPC_CHICKEN].pos;
                        TraceLog(LOG_INFO, "Chicken tenders .... mmmm");
                    }
                }
                else if (!don.isTalking
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_ABBY].pos, don.pos) < 144
                    && HasTimerElapsed(&don.talkStartTimer))
                {
                    don.isTalking = true;

                    // First mission: electricity/windmill
                    if (!missions[MISSION_ABBY_LIGHT].complete)
                    {
                        if (Scenes[SCENE_HOME_WINDMILL_07].active)
                        {
                            toast = "Completed mission! Abby has Electricity.";
                            StartTimer(&toastTimer);

                            don.xp += 100;
                            don.money += 5; //abby is poor

                            missions[MISSION_ABBY_LIGHT].complete = true;

                            don.who = TALK_TYPE_ABBY_2; // now she asks for medicine
                        }
                        else
                        {
                            don.who = TALK_TYPE_ABBY; // asks for windmill
                        }
                    }
                    // Second mission already complete
                    else if (missions[MISSION_ABBY_RX].complete)
                    {
                        don.who = TALK_TYPE_ABBY_4;
                    }
                    // Has medicine: ask yes/no
                    else if (inventory[INV_RX].count > 0)
                    {
                        don.who = TALK_TYPE_ABBY_3;
                    }
                    // No medicine yet: ask player to find it
                    else
                    {
                        don.who = TALK_TYPE_ABBY_2;
                    }

                    Talk_Reset(don.who);

                    prevTalkTri = gpad.btnTriangle;
                    prevTalkX = gpad.btnCross;
                    StartTimer(&don.talkStartTimer);
                }
                else if (!don.isTalking
                    && InteractivePoints[POI_TYPE_GALADRIEL].pos
                    && Vector3DistanceSqr(*InteractivePoints[POI_TYPE_GALADRIEL].pos, don.pos) < 144
                    && HasTimerElapsed(&don.talkStartTimer))
                    {
                        don.isTalking = true;

                        don.who = Galadriel_GetTalkType(&don);

                        Talk_Reset(don.who);

                        prevTalkTri = gpad.btnTriangle;
                        prevTalkX = gpad.btnCross;
                        StartTimer(&don.talkStartTimer);
                        }
                if (!don.isTalking && HasTimerElapsed(&don.talkStartTimer))
                { //stores
                    NPC* clerk = NULL;

                    for (int i = 0; i < NPC_TOTAL; i++)
                    {
                        if (npcs[i].modelType != NPC_MODEL_TYPE_CLERK) { continue; }

                        if (Vector3DistanceSqr(npcs[i].pos, don.pos) < 144)
                        {
                            clerk = &npcs[i];
                            break;
                        }
                    }

                    if (clerk)
                    {
                        don.isTalking = true;
                        don.who = TALK_TYPE_STORE;

                        storeSel = INV_HEALTH;

                        StartTimer(&don.talkStartTimer);
                        Talk_Reset(don.who);

                        prevTalkTri = gpad.btnTriangle;
                        prevTalkX = gpad.btnCross;
                    }
                }
                //fire places/pits
                for (int i = 0; i < FIREPIT_TOTAL_COUNT; i++)
                {
                    if (Vector3DistanceSqr(don.pos,fires[i].pos)<200)
                    {
                        TraceLog(LOG_INFO, "%s firepit lit!", fires[i].name);
                        fires[i].lit = true;
                    }
                }
                //berries pick
                for (int i = 0; i < numCloseProps; i++)
                {
                    if (CloseProps[i]->type == MODEL_TREE_2                      // gate by tree type
                        && CloseProps[i]->hasBerries                             // must be enabled
                        && CloseProps[i]->berryCount > 0                         // must have some to collect
                        && Vector3DistanceSqr(don.pos, CloseProps[i]->pos) < 144) // in range
                    {
                        CloseProps[i]->berriesSpawned = false;
                        if (gGame.diff == DIFF_EASY)
                        {
                            inventory[INV_BERRY].count += CloseProps[i]->berryCount;
                        }
                        else
                        {
                            inventory[INV_BERRY].count++; //if berries
                        }
                        
                        CloseProps[i]->berryCount = 0;
                        CloseProps[i]->hasBerries = 0;
                        PlaySoundVol(pick);
                    }
                }
                // apples
                for (int i = 0; i < MAX_APPLES_TOTAL; ++i) {
                    Apple* a = &apples[i];
                    if (!a->spawned || !a->fallen) { continue; }
                    if (Vector3DistanceSqr(don.pos, a->pos) < 7.3*8) {
                        inventory[INV_APPLE].count++;   // apple exists ready to consume baby!
                        a->spawned = false;             // free slot
                        a->fallen = false;
                        a->falling = false; // to be safe
                        PlaySoundVol(pick);
                        // optional toast
                    }
                }
            }
            // --->>> SUMMON (R3 press to start/cancel)
            {
                static int prevR3 = 0;
                int r3 = havePad ? (gpad.btnR3 > 0) : 0;  // ControllerData should already have btnR3 like btnL3

                if (r3 && !prevR3) {
                    if (don.unlockedTruck && !truckSummonActive)
                    {
                        TruckSummonStart(); StartTimer(&gGame.HonkedHornRecently);//horn honks in summon start
                    }
                    else
                    {
                        TruckSummonCancel(); // press again to cancel
                    }
                }
                prevR3 = r3;
            }
        }
        else if (don.unlockedTruck && vehicleMode)
        {
            bool r3 = havePad ? (gpad.btnR3 > 0) : false;
            if (don.canHasCheeseburger && r3 && !prevHoverR3)
            {
                hoverMode = !hoverMode;
                hoverLiftTarget = hoverMode ? 3.5f : 0.0f;
                toast = hoverMode ? "Hover Mode!" : "Truck Mode!";
                StartTimer(&toastTimer);
            }
            prevHoverR3 = r3;
            if (hoverMode && gpad.btnCross) //&& !prevCross, I dont want pump, I want press
            {
                truckAirState = AIRBORNE;
                gravityCollected = -2.8f;   // upward burst; tune
                truckSpeed += 128 * GetFrameTime();
            }
            if (hoverMode && truckAirState == AIRBORNE) //glide
            {
                truckPitch += 0.00004f * GetFrameTime();

                truckPosition.y -= GetFrameTime() * GRAVITY * gravityCollected * 0.35f;
                gravityCollected += GetFrameTime() * GRAVITY * 0.35f;

                // when near hover height again, land back into hover
                float gy = GetTerrainHeightFromMeshXZ(truckPosition.x, truckPosition.z);
                float hoverY = gy + TRUCK_Y_OFFSET_DRAW + hoverLift;
                if (hoverY < WHALE_SURFACE + 3.2) { hoverY = WHALE_SURFACE + 3.2; }
                if (truckPosition.y <= hoverY)
                {
                    truckPosition.y = hoverY;
                    truckAirState = GROUND;
                    gravityCollected = 0.0f;
                }
            }
            else if (truckAirState == AIRBORNE) //gravity
            {
                truckPitch += 0.0001 * GetFrameTime();//dip it slightly down
                if (truckPitch > PI / 7.0f) { truckPitch = PI / 7.0f; }
                truckPosition.y -= GetFrameTime() * GRAVITY * gravityCollected;
                gravityCollected += GetFrameTime() * GRAVITY;
                truckForward.y = Lerp(truckForward.y, 0, GetFrameTime() * GRAVITY * gravityCollected);
                //update tricks - dont shut them off here, just update them
                if (doing360) { truckTrickYaw += GetFrameTime() * 16.0f; }
                if (doingFlip) { truckTrickPitch += GetFrameTime() * 7.6f; }
                if (doingRoll) { truckTrickRoll += GetFrameTime() * 13.666f; }
                if (doingBonkers)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        tireOffsets[i] = LerpVector3(tireOffsets[i], bonkersPeeked ? bonkersStartOffsets[i] : bonkersPeekOffsets[i], GetFrameTime() * 8.0f);
                    }
                }
            }
            else if (truckAirState == LANDING)
            {
                truckAirState = GROUND;
                truckForward.y = 0.0f;
                gravityCollected = 0.0f;//temp, dont know what goes here, or if this is valid at all
                bounceCollector += fabs(GetFrameTime() * (maxSpeed - truckSpeed + 0.014f)); //maxSpeed - truckSpeed (0->1.5, 1->0.5, 1.5->0 ? +delta)
                //bounceCollector+=fabs(GetFrameTime() * truckSpeed); //or maybe we want abs value of truck speed ... ?
                if (bounceCollector > 0.18f)
                {
                    truckAirState = GROUND;
                    bounceCollector = 0;
                }
            }
            else //GROUND
            {
                truckForward.y = 0.0f;
                gravityCollected = 0.0f;
            }
            //shut off tricks
            if (doing360 && (truckAirState != AIRBORNE || truckTrickYaw >= 2.0f * PI)) //if weve gone more than two pi, 360!
            {
                doing360 = false;
                truckTrickYaw = 0.0f;
                if (truckAirState == AIRBORNE) { points += 100; }//points 
            }
            if (doingFlip && (truckAirState != AIRBORNE || truckTrickPitch >= 2.0f * PI)) //if weve gone more than two pi, Back Flip!
            {
                doingFlip = false;
                truckTrickPitch = 0.0f;
                if (truckAirState == AIRBORNE) { points += 400; }//points 
            }
            if (doingRoll && (truckAirState != AIRBORNE || truckTrickRoll >= 2.0f * PI)) //if weve gone more than two pi, Kick Flip!
            {
                doingRoll = false;
                truckTrickRoll = 0.0f;
                if (truckAirState == AIRBORNE) { points += 150; }//points 
            }
            if (doingBonkers) //this one is alittle different because of how we identify the completion
            {
                bool phase = true;
                for (int i = 0; i < 4; i++)
                {
                    phase = fabsf(tireOffsets[i].x - (bonkersPeeked ? bonkersStartOffsets[i] : bonkersPeekOffsets[i]).x) < 0.1f;
                    phase = fabsf(tireOffsets[i].y - (bonkersPeeked ? bonkersStartOffsets[i] : bonkersPeekOffsets[i]).y) < 0.1f;
                    phase = fabsf(tireOffsets[i].z - (bonkersPeeked ? bonkersStartOffsets[i] : bonkersPeekOffsets[i]).z) < 0.1f;
                }
                if (phase && !bonkersPeeked) { phase = false; bonkersPeeked = true; }//1st phase complete
                if (truckAirState != AIRBORNE || (bonkersPeeked && phase)) //if we hit the ground or completed the trick
                {
                    doingBonkers = false;
                    bonkersPeeked = false;
                    if (truckAirState == AIRBORNE) { points += 650; }//points 
                }
            }
            if (!Menu_IsOpen(&gGame) && gpad.btnTriangle > 0 && HasTimerElapsed(&truckInteractTimer) && truckAirState != AIRBORNE)
            {
                StartTimer(&truckInteractTimer);
                vehicleMode = false; 
                donnyMode = true;
                don.xp += (int)(points / 1000);
                don.money += ((float)points/1000.0);
                points = 0;
                don.pos = Vector3Add(truckPosition, (Vector3) {6,1,-5});//todo: why did I put a one here for y?
            }
        }
        prevCross = cross; prevTri = tri; //set after this large block
        // --->>> SUMMON (autopilot)
        if (truckSummonActive && !vehicleMode) {
            float dt = GetFrameTime();

            // Vector to Donny on XZ plane
            Vector3 to = YOnly(Vector3Subtract(don.pos, truckPosition));
            float dist = Vector3Length(to);

            // Arrived?
            if (dist <= truckSummonStopRadius) {
                // Smooth stop and end
                float brake = truckSummonAccel * 2.0f * dt;
                if (truckSpeed > brake) truckSpeed -= brake;
                else if (truckSpeed < -brake) truckSpeed += brake;
                else                          truckSpeed = 0.0f;

                if (fabsf(truckSpeed) < 0.01f) {
                    TruckSummonCancel();
                }
            }
            else {
                // Desired yaw: forward is +Z in your world, so atan2(x,z)
                Vector3 dir = Vector3Scale(to, 1.0f / (dist + 1e-6f));
                float targetYaw = atan2f(dir.x, dir.z);
                float yawErr = WrapAngle(targetYaw - truckAngle);

                // Turn toward target with capped yaw rate
                float maxTurn = truckSummonSteerRate * dt;
                if (yawErr > maxTurn) yawErr = maxTurn;
                if (yawErr < -maxTurn) yawErr = -maxTurn;
                truckAngle += yawErr;

                // Speed target: slow down when turning hard
                float turnFactor = 1.0f - fminf(fabsf(yawErr) / (PI * 0.5f), 1.0f);
                float want = truckSummonMaxSpeed * (0.25f + 0.75f * turnFactor);

                // Approach want speed with accel clamp (also works as brake)
                float dv = want - truckSpeed;
                float lim = truckSummonAccel * dt;
                if (dv > lim) dv = lim;
                if (dv < -lim) dv = -lim;
                truckSpeed += dv;
            }
        }

        //old important stuff
        float time = GetTime();
        SetShaderValue(waterShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(grassInstancingLightShader, grassTimeLoc, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(ghostShader, locGhostTime, &time, SHADER_UNIFORM_FLOAT);
        SetShaderValue(fireShader, fireTimeLoc, &time, SHADER_UNIFORM_FLOAT);
        bool reportOn = false;
        int tileTriCount = 0;
        int tileBcCount = 0;
        int treeTriCount = 0;
        int treeBcCount = 0;
        int chunkTriCount = 0;
        int chunkBcCount = 0;
        int totalTriCount = 0;
        int totalBcCount = 0;
        float dt = GetFrameTime();
        //idk, for pbr;
        // float cameraPosVecF[3] = {camera.position.x, camera.position.y, camera.position.z};
        // SetShaderValue(lightningBugShader, lightningBugShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPosVecF, SHADER_UNIFORM_VEC3);
        // SetShaderValue(starShader, starShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPosVecF, SHADER_UNIFORM_VEC3);
      
        //main thread of the file management system, needed for GPU operations
        if (wasTilesDocumented)
        {
            //setup close tiles for start processing
            for (int f = 0; f < MAX_TO_PROCESS; f++)
            {
                closestTiles[f].index = 0;
                closestTiles[f].distance = 999999; //small?
            }
            int gx, gy;
            int processed = 0;

            float time = GetTime();
            SetShaderValue(starShader, GetShaderLocation(starShader, "u_time"), &time, SHADER_UNIFORM_FLOAT);

            GetGlobalTileCoords(camera.position, &gx, &gy);
            //tiles
            //first pass, just document what needs to get uploaded
            for (int te = 0; te < foundTileCount && GetFPS() > 54; te++)
            {
                TileEntry* t = &foundTiles[te];
                if (chunks[t->cx][t->cy].lod == LOD_64 && t->state == TS_OPENED_NOT_GPU) //if we need it at some point as of now
                {
                    int dist = TileDistSq(t,closestTX,closestTY);
                    for (int f = 0; f < MAX_TO_PROCESS; f++)
                    {
                        if (closestTiles[f].distance > dist)
                        {
                            closestTiles[f].index = te;
                            closestTiles[f].distance = dist;
                            break;
                        }
                    }
                }
            }
            //second pass, load close things
            for (int f = 0; f < MAX_TO_PROCESS; f++)
            {
                if (closestTiles[f].index == 0) { continue; }
                TileEntry* t = &foundTiles[closestTiles[f].index];
                bool needed = (chunks[t->cx][t->cy].lod == LOD_64);

                MUTEX_LOCK(mutex);
                if (needed && t->state == TS_OPENED_NOT_GPU && processed < MAX_TO_PROCESS)
                {
                    TraceLog(LOG_INFO, "uploading tile to GPU: %d", closestTiles[f].index);

                    UploadMesh(&t->model.meshes[0], false);
                    t->box = GetModelBoundingBox(t->model);
                    t->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LowFiStaticObjectModelTextures[t->type];

                    t->state = TS_IN_GPU;
                    processed++;
                }
                MUTEX_UNLOCK(mutex);
            }
            //pass 3, unload
            for (int te = 0; te < foundTileCount && GetFPS() > 54; te++)
            {
                TileEntry* t = &foundTiles[te];
                bool needed = (chunks[t->cx][t->cy].lod == LOD_64);

                MUTEX_LOCK(mutex);
                // Reverse stage: GPU -> CPU only
                if (!needed && t->state == TS_IN_GPU)
                {
                    UnloadModel(t->model);
                    memset(&t->model, 0, sizeof(Model));
                    memset(&t->mesh, 0, sizeof(Mesh));
                    t->state = TS_UNCOMP_RAM;
                }
                MUTEX_UNLOCK(mutex);
            }
            if (processed > 0)
            {
                TraceLog(LOG_INFO, "processed %d tiles this loop", processed);
            }
        }
        //tiles and chunks?
        for (int cy = 0; cy < CHUNK_COUNT; cy++) {
            for (int cx = 0; cx < CHUNK_COUNT; cx++) {
                if (chunks[cx][cy].isReady && !chunks[cx][cy].waterLoaded)//water
                {
                    MUTEX_LOCK(mutex);
                    for (int w = 0; w < chunks[cx][cy].waterCount; w++)
                    {
                        UploadMesh(&chunks[cx][cy].water[w].model.meshes[0], false);
                    }
                    chunks[cx][cy].waterLoaded = true;
                    MUTEX_UNLOCK(mutex);
                }
                if(chunks[cx][cy].isTextureReady && !chunks[cx][cy].isTextureLoaded)
                {
                    MUTEX_LOCK(mutex);
                    TraceLog(LOG_INFO, "loading chunk textures: %d,%d", cx, cy);
                    Texture2D texture = LoadTextureFromImage(chunks[cx][cy].img_tex); //using slope and color avg right now
                    Texture2D textureBig = LoadTextureFromImage(chunks[cx][cy].img_tex_big);
                    Texture2D textureFull = LoadTextureFromImage(chunks[cx][cy].img_tex_full);
                    Texture2D textureDamn = LoadTextureFromImage(chunks[cx][cy].img_tex_damn);
                    SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);
                    SetTextureWrap(textureBig, TEXTURE_WRAP_CLAMP);
                    SetTextureWrap(textureFull, TEXTURE_WRAP_CLAMP);
                    SetTextureWrap(textureDamn, TEXTURE_WRAP_CLAMP);
                    GenTextureMipmaps(&textureFull);  // <-- this generates mipmaps
                    SetTextureFilter(textureFull, TEXTURE_FILTER_TRILINEAR); // use a better filter
                    GenTextureMipmaps(&textureBig);  // <-- this generates mipmaps
                    SetTextureFilter(textureBig, TEXTURE_FILTER_TRILINEAR); // use a better filter
                    GenTextureMipmaps(&texture);  // <-- this generates mipmaps
                    SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR); // use a better filter
                    GenTextureMipmaps(&textureDamn);  // <-- this generates mipmaps
                    SetTextureFilter(textureDamn, TEXTURE_FILTER_TRILINEAR); // use a better filter
                    chunks[cx][cy].texture = texture;  // Copy contents
                    chunks[cx][cy].textureBig = textureBig;
                    chunks[cx][cy].textureFull = textureFull;
                    chunks[cx][cy].textureDamn = textureDamn;
                    chunks[cx][cy].isTextureLoaded = true;
                    MUTEX_UNLOCK(mutex);
                }
                else if (chunks[cx][cy].isTextureLoaded && chunks[cx][cy].isReady && !chunks[cx][cy].isLoaded) {
                    MUTEX_LOCK(mutex);
                    TraceLog(LOG_INFO, "loading chunk model: %d,%d", cx, cy);

                    // Upload meshes to GPU
                    UploadMesh(&chunks[cx][cy].model.meshes[0], false);
                    UploadMesh(&chunks[cx][cy].model32.meshes[0], false);
                    UploadMesh(&chunks[cx][cy].model16.meshes[0], false);
                    UploadMesh(&chunks[cx][cy].model8.meshes[0], false);

                    // Load GPU models
                    chunks[cx][cy].model = LoadModelFromMesh(chunks[cx][cy].model.meshes[0]);
                    chunks[cx][cy].model32 = LoadModelFromMesh(chunks[cx][cy].model32.meshes[0]);
                    chunks[cx][cy].model16 = LoadModelFromMesh(chunks[cx][cy].model16.meshes[0]);
                    chunks[cx][cy].model8 = LoadModelFromMesh(chunks[cx][cy].model8.meshes[0]);

                    //apply transform to vertices based on world position -- and of course it does not work because we are using a custom shader now
                    // chunks[cx][cy].model.transform = MatrixTranslate(chunks[cx][cy].position.x, chunks[cx][cy].position.y, chunks[cx][cy].position.z);
                    // chunks[cx][cy].model32.transform = MatrixTranslate(chunks[cx][cy].position.x, chunks[cx][cy].position.y, chunks[cx][cy].position.z);
                    // chunks[cx][cy].model16.transform = MatrixTranslate(chunks[cx][cy].position.x, chunks[cx][cy].position.y, chunks[cx][cy].position.z);
                    // chunks[cx][cy].model.transform = MatrixTranslate(chunks[cx][cy].position.x, chunks[cx][cy].position.y, chunks[cx][cy].position.z);
                    //apply shader to 64 chunk
                    chunks[cx][cy].model.materials[0].shader = heightShaderLight;
                    chunks[cx][cy].model32.materials[0].shader = heightShaderLight;//only do this for reltively close things, not 8 and 16
                    // Apply textures
                    chunks[cx][cy].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].textureDamn;
                    chunks[cx][cy].model32.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].textureFull;
                    chunks[cx][cy].model16.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].textureBig;
                    chunks[cx][cy].model8.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].texture;

                    // Setup bounding box
                    chunks[cx][cy].origBox = ScaleBoundingBox(GetModelBoundingBox(chunks[cx][cy].model), MAP_SCALE);
                    chunks[cx][cy].box = UpdateBoundingBox(chunks[cx][cy].origBox, chunks[cx][cy].center);

                    chunks[cx][cy].isLoaded = true;
                    TraceLog(LOG_INFO, "loaded chunk model -> %d,%d", cx, cy);
                    MUTEX_UNLOCK(mutex);
                }
            }
        }

        FindClosestChunkAndAssignLod(vehicleMode?truckPosition:camera.position); //Im not sure If I need this here, but things work okay so...?

        // Mouse look
        Vector2 mouse = (Vector2){ 0 };
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            mouse = GetMouseDelta();
            yaw -= mouse.x * 0.003f;
            pitch -= mouse.y * 0.003f;
        }
        
        pitch = Clamp(pitch, -PI/2.0f, PI/2.0f);
        Vector3 forward = {
            cosf(pitch) * cosf(yaw) * MAP_SCALE,
            sinf(pitch) * MAP_SCALE,
            cosf(pitch) * sinf(yaw) * MAP_SCALE
        };
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));

        Vector3 move = { 0 };
        float goku = false;
        float spd = MOVE_SPEED;
        if (!donnyMode || !don.isTalking)
        {
            //if (IsKeyPressed(KEY_C)) { DisableCursor(); }
            if (IsKeyPressed(KEY_X)) { EnableCursor(); }
            if (IsKeyPressed(KEY_Y)) { contInvertY = !contInvertY; }
            if (IsKeyPressed(KEY_M)) { showMap = !showMap; } // Toggle map
            //if (IsKeyDown(KEY_EQUAL)) mapZoom += 0.01f;  // Zoom in (+ key)
            //if (IsKeyDown(KEY_MINUS)) mapZoom -= 0.01f;  // Zoom out (- key)
            //mapZoom = Clamp(mapZoom, 0.5f, 4.0f);
            //end map input
            if (onLoad && IsKeyPressed(KEY_V) && don.unlockedTruck)
            { 
                vehicleMode = !vehicleMode; 
                donnyMode = !vehicleMode;
                if (!vehicleMode)
                {
                    don.xp += (int)(points / 1000);
                    points = 0;
                }
                EnableCursor(); 
            }
            if (IsKeyPressed(KEY_B)) { displayBoxes = !displayBoxes; }
            if (IsKeyPressed(KEY_L)) { displayLod = !displayLod; }
            if (IsKeyPressed(KEY_F12)) { TakeScreenshotWithTimestamp(); }
            //if (IsKeyPressed(KEY_F11)) { reportOn = true; }
            //if (IsKeyPressed(KEY_F10)) { MemoryReport(); }
            //if (IsKeyPressed(KEY_F9)) { GridChunkReport(); }
            //if (IsKeyPressed(KEY_F8)) { GridTileReport(); }
            if (IsKeyPressed(KEY_F2)) { devDisplay = !devDisplay; }
            if (IsKeyPressed(KEY_F1)) {
                ToggleFullscreen();
                if (IsWindowFullscreen()) {
                    int m = GetCurrentMonitor();
                    SetWindowSize(GetMonitorWidth(m), GetMonitorHeight(m));
                }
                else {
                    SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT); // your preferred windowed size
                }
            }
            if (IsKeyPressed(KEY_Z)) { dayTime = !dayTime; }
        }
        else if (don.isTalking)
        {
            bool xPressed = (gpad.btnCross && !prevTalkX);
            bool triPressed = (gpad.btnTriangle && !prevTalkTri);

            prevTalkX = gpad.btnCross;
            prevTalkTri = gpad.btnTriangle;

            TalkResult talkResult = Talk_UpdateController(xPressed, triPressed);

            if (talkResult == TALK_RESULT_YES)
            {
                if (don.who == TALK_TYPE_ABBY_3)
                {
                    Abby_GiveMedicine(&don);
                }
                else if (don.who == TALK_TYPE_GAL_2)
                {
                    Galadriel_GiveBooks(&don);
                }
            }
            else if (talkResult == TALK_RESULT_NO || talkResult == TALK_RESULT_FINISHED)
            {
                FinishTalking(&don);
            }
        }
        
        //handle controller input
        if (vehicleMode)
        {
            if(gpad.btnR1 > 0 && truckAirState==AIRBORNE)
            {
                doing360 = true;
            }
            if(gpad.btnR2 > 0 && truckAirState==AIRBORNE) //one trick at a time? NO! Lots of tricks at once!!!
            {
                doingFlip = true;
            }
            if(gpad.btnL1 > 0 && truckAirState==AIRBORNE) //one trick at a time? NO! Lots of tricks at once!!
            {
                doingRoll = true;
            }
            if(gpad.btnL2 > 0 && truckAirState==AIRBORNE) //one trick at a time? NO! Lots of tricks at once!!!!
            {
                doingBonkers = true;
            }
            // Deadzone
            if (fabsf(gpad.normLY) > 0.1f)
            {
                if(truckAirState!=AIRBORNE)
                {
                    //float ratio = fminf(fabsf(truckSpeed) / maxSpeed, 1.0f) + 1.0f; //use this version for exponential acceleration (I like it less)
                    //truckSpeed += -gpad.normLY * acceleration * ratio * ratio * ratio * GetFrameTime() * 12.0f;
                    truckSpeed += -gpad.normLY * acceleration * dt * 62.0f;
                }
                //printf("speed=%f",truckSpeed);
            }
            else if(!truckCruise)
            {
                //update truck with friction
                if (truckSpeed > 0.0f) { //friction
                    truckSpeed *= friction;
                    if (truckSpeed < 0.000001f) {truckSpeed = 0.0f;}  // Clamp to zero
                }
                else if (truckSpeed < 0.0f) {
                    truckSpeed *= friction;
                    if (truckSpeed > -0.00000001f) {truckSpeed = 0.0f;}  // Clamp to zero
                }
            }
            else if(truckCruise)
            {
                truckSpeed = LerpFloat(&truckCruiseTarget, &truckSpeed, &dt);
            }
            bool truckL3Pressed = (gpad.btnL3 && !prevTruckL3);
            prevTruckL3 = gpad.btnL3;

            if (truckL3Pressed)
            {
                truckCruise = !truckCruise;
                if (truckCruise)
                {
                    truckCruiseTarget = truckSpeed;
                    if (truckCruiseTarget < 2.0f) truckCruiseTarget = 2.0f; // don't cruise at near-zero
                    toast = "Cruise On";
                    StartTimer(&toastTimer);
                }
                else
                {
                    toast = "Cruise Off";
                    StartTimer(&toastTimer);
                }
            }
            if (gpad.btnSquare>0)//square
            {
                truckCruise = false;
                if(truckAirState!=AIRBORNE && fabs(truckSpeed)>0)
                {
                    truckSpeed -= deceleration * (truckSpeed>0?1:-1);
                    truckPitch += GetFrameTime();//dip it forward when hitting the breaks
                    printf("Square was pressed and is having an effect!\n");
                    if(isTruckSliding){truckSlidePeek=true;}
                }
            }
            if(gpad.btnCircle > 0)
            {
                PlaySoundVol(carHorn);
                StartTimer(&gGame.HonkedHornRecently);
            }
            //some extra stuff for the truck - steering
            steerInput = gpad.normLX * GetFrameTime();
            float turnMax = 0.8f;
            if(steerInput>turnMax){steerInput=turnMax;}
            if(steerInput<-turnMax){steerInput=-turnMax;}
            float delta = (steerInput * steeringSpeed);
            //truckAngle -= delta;//the beast
            if (truckSpeed > 0.01f||truckSpeed < -0.01f)//make sure we dont turn if we are not moving
            {
                truckAngle -= delta;
            }
            //tire spin
            float spin = truckSpeed * spinRate * GetFrameTime() * -66.0f;//gpad.normLY * spinRate * GetFrameTime();//only when contrller input
            for(int t=0; t<4; t++)
            {
                tireSpinDelta[t] = spin;
                if (tireSpinDelta[t] > PI/16.0f) {tireSpinDelta[t] = PI/16.0f;}
                if (tireSpinDelta[t] < -PI/16.0f) {tireSpinDelta[t] = -PI/16.0f;}
                tireTurnDelta[t] = delta;
                if (tireTurnDelta[t] > PI/16.0f) {tireTurnDelta[t] = PI/16.0f;}
                if (tireTurnDelta[t] < -PI/16.0f) {tireTurnDelta[t] = -PI/16.0f;}
                if(truckSpeed > 0.1f||truckSpeed < -0.1f){tireSpinPos[t] += tireSpinDelta[t];}
                if(truckSpeed > 0.1f||truckSpeed < -0.1f){tireTurnPos[t] += tireTurnDelta[t];}
                if(tireTurnPos[t]>(PI/32.0f)){tireTurnPos[t]=(PI/32.0f);}
                if(tireTurnPos[t]< -(PI/32.0f)){tireTurnPos[t]= -(PI/32.0f);}
            }
            
            //more steering - for the camera tho
            float sensitivity = 90.0f;  // degrees per second max
            float deadzone = 8.0f;

            float realRy = gpad.ry;
            if(contInvertY){realRy = 255 - realRy;}

            // if (fabsf(rxNorm) > deadzone / 127.0f) {camYaw += rxNorm * sensitivity * GetFrameTime();}
            // if (fabsf(ryNorm) > deadzone / 127.0f) {camPitch -= ryNorm * sensitivity * GetFrameTime();}

            if (fabsf(gpad.normRX) > deadzone / 127.0f) {
                relativeYaw += gpad.normRX * sensitivity * GetFrameTime();
            }
            if (fabsf(gpad.normRY) > deadzone / 127.0f) {
                relativePitch += -gpad.normRY * sensitivity * GetFrameTime();
            }

            // Clamp pitch so the camera doesn't go under or flip
            if (relativePitch < 5.0f) relativePitch = 5.0f;
            if (relativePitch > 85.0f) relativePitch = 85.0f;
        }
        else if (donnyMode && don.state != DONOGAN_STATE_SLIDE)
        {
            // -------- Character movement (camera-relative) --------
            float lx = havePad ? gpad.normLX : 0.0f;
            float ly = havePad ? gpad.normLY : 0.0f;
            moveMag = sqrtf(lx * lx + ly * ly);

            // Camera forward/right on the XZ plane
            Vector3 camFwd = { sinf(yaw), 0.0f, cosf(yaw) };
            Vector3 camRight = { cosf(yaw), 0.0f, -sinf(yaw) };

            // Left stick up = forward (-ly), right = +lx
            donMove = Vector3Add(Vector3Scale(camRight, lx), Vector3Scale(camFwd, ly));
            if (moveMag > 0.001f) { donMove = Vector3Normalize(donMove); }
            don.velXZ = (Vector3){ donMove.x, 0, donMove.z };
            //DonUpdate(&don, havePad ? &gpad : NULL, dt);
        }
        if (disableDonInputNextFrame)
        {
            disableDonInputNextFrame = false;
            donMove = (Vector3){ 0 };
            moveMag = 0.0f;
        }
        if (vehicleMode)
        {
            //truck position
        // Clamp speed
            if (truckAirState == GROUND) //handle rolling down hills
            {
                if (truckPitch >= PI / 15.0f && (fabsf(gpad.normLY) < 0.1f)) //roll down hills
                {
                    truckSpeed += GetFrameTime() * GRAVITY * (1.0f / 16.0f) * (1 + truckSpeed) * (1 + truckSpeed);
                }
                else if (truckPitch <= -PI / 15.0f && (fabsf(gpad.normLY) < 0.1f))//reverse
                {
                    truckSpeed -= GetFrameTime() * GRAVITY * (1.0f / 16.0f) * (1 + truckSpeed) * (1 + truckSpeed);
                }
                else
                {
                    truckForward.y = 0;
                }
            }
            float maxHoverSpeed = 2.3210456;
            if (hoverMode && truckSpeed > maxHoverSpeed) { truckSpeed = maxHoverSpeed; }
            else if (truckSpeed > maxSpeed) { truckSpeed = maxSpeed; }
            if (truckSpeed < maxSpeedReverse) { truckSpeed = maxSpeedReverse;}
            if (hoverMode && truckSpeed < 0) { truckSpeed = 0; }

            //sliding
            if (truckAirState == GROUND && truckSlideSpeed >= 0) // sliding, shut off if not on the ground or the slide is complete
            {
                //sliding 
                if (truckSpeed > 1.23 && fabsf(gpad.normLX) > 0.56f && !isTruckSliding)//trigger slide
                {
                    //printf("sliding ... \n");
                    isTruckSliding = true;
                    truckSlidePeek = false; // we just started
                    truckSlideSpeed += GetFrameTime();
                    rotSlide = gpad.normLX * -PI / 1.8f;//gpad.normLX>0?-PI/2.0f:PI/2.0f; //use this to control which way we slide
                    truckSlideForward = RotateY(truckForward, rotSlide);
                }
                else if (isTruckSliding && fabsf(gpad.normLX) > 0.12)
                {
                    rotSlide = gpad.normLX * -PI / 1.8f;//gpad.normLX>0?-PI/2.0f:PI/2.0f; //use this to control which way we slide
                    truckSlideForward = RotateY(truckForward, rotSlide);
                    if (truckSlidePeek) { truckSlideSpeed -= GetFrameTime(); }
                    else { truckSlideSpeed += GetFrameTime() * truckSpeed; }
                    if (truckSlideSpeed > 0.71f && fabsf(gpad.normLX) < 0.88f) { truckSlidePeek = true; printf("sliding peeked (%f).... \n", fabsf(gpad.normLX)); }
                    if (truckSlideSpeed > 1.20f) { truckSlideSpeed = 1.200001f; }
                    truckSpeed -= GetFrameTime();
                    //truckSlideForward = RotateY(truckForward,rotSlide); //try with and without this line
                }
                else
                {
                    //printf("fin \n");
                    //turn off slide if not on ground
                    isTruckSliding = false;
                    truckSlidePeek = false;
                    truckSlideSpeed = 0;
                    rotSlide = 0;
                }
            }
            else
            {
                if (isTruckSliding)
                {
                    printf("!\n");//this should never happen
                    //turn off slide if not on ground
                    isTruckSliding = false;
                    truckSlidePeek = false;
                    truckSlideSpeed = 0;
                    rotSlide = 0;
                }
            }
        }

        if (!vehicleMode && !truckSummonActive) { truckSpeed = 0; }
        if (don.unlockedTruck)
        {
            truckForward = (Vector3){ sinf(truckAngle), sinf(-truckPitch), cosf(truckAngle) };
            Vector3 tempTruck = Vector3Scale(truckForward, truckSpeed);
            if (isTruckSliding)
            {
                tempTruck = Vector3Add(Vector3Scale(truckForward, truckSpeed), Vector3Scale(truckSlideForward, truckSlideSpeed));
            }
            //ugh!?
            truckPosition = Vector3Add(truckPosition, tempTruck);
        }
        truckOrigin = Vector3Add(truckPosition, rearAxleOffset);

        if (!vehicleMode && Vector3Length(move) > 0.01f) {
            move = Vector3Normalize(move);
            move = Vector3Scale(move, goku ? spd : spd * dt);
            camera.position = Vector3Add(camera.position, move);
        }
        FindClosestChunkAndAssignLod(vehicleMode?truckPosition:camera.position);
        //----------------------------------------------------------------------------------------------------
        //fade to black, end scene...
        if (dayTime) {
            skyboxTint = LerpColor(skyboxTint, skyboxDay, 0.02f); //dont scale this one
            backGroundColor = LerpColor(backGroundColor, backgroundDay, 0.004f * scaleNightTransition);
            LightPosDraw = LerpVector3(LightPosDraw, LightPosTargetDay, 0.04f * scaleNightTransition);
            LightTargetDraw = LerpVector3(LightTargetDraw, LightTargetTargetDay, 0.04f * scaleNightTransition);
            lightColorDraw = LerpColor(lightColorDraw, lightColorTargetDay, 0.05f * scaleNightTransition);
            instanceLight.position = LightPosDraw;
            instanceLight.target = LightTargetDraw;
            instanceLight.color = lightColorDraw;
            UpdateLightValues(instancingLightShader,instanceLight);
            lightDir = LerpVector3(lightDir,(Vector3){ -10.2f, -100.0f, -10.3f },0.02f * scaleNightTransition);
            SetShaderValue(heightShaderLight, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
            lightTileColor = LerpColor(lightTileColor, (Color){160,180,200,254}, 0.02f * scaleNightTransition);
        }
        else { //night time
            skyboxTint = LerpColor(skyboxTint, skyboxNight, 0.002f); //dont scale this one
            backGroundColor = LerpColor(backGroundColor, backgroundNight, 0.002f * scaleNightTransition);
            LightPosDraw = LerpVector3(LightPosDraw, LightPosTargetNight, 0.04f * scaleNightTransition);
            LightTargetDraw = LerpVector3(LightTargetDraw, LightTargetTargetNight, 0.04f * scaleNightTransition);
            lightColorDraw = LerpColor(lightColorDraw, lightColorTargetNight, 0.05f * scaleNightTransition);
            instanceLight.position = LightPosDraw;
            instanceLight.target = LightTargetDraw;
            instanceLight.color = lightColorDraw;
            UpdateLightValues(instancingLightShader,instanceLight);
            lightDir = LerpVector3(lightDir,(Vector3){ -5.2f, -70.0f, 15.3f },0.02f * scaleNightTransition);
            SetShaderValue(heightShaderLight, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
            lightTileColor = LerpColor(lightTileColor, (Color){50,50,112,180}, 0.005f * scaleNightTransition);
            if(onLoad && !bugGenHappened)
            {
                TraceLog(LOG_INFO, "bug gen");
                bugs = GenerateLightningBugs(camera.position, BUG_COUNT, 256.0256f);
                bugGenHappened=true;
            }
            else if (bugGenHappened && Vector3DistanceSqr(camera.position,lastLBSpawnPosition)>360.12f*360)
            {
                TraceLog(LOG_INFO, "bug re-gen");
                RegenerateLightningBugs(bugs, camera.position, BUG_COUNT, 256.0256f);
            }
            if(onLoad && !starGenHappened)
            {
                stars = GenerateStars(STAR_COUNT);
                starGenHappened = true;
            }
        }
        //collision section----------------------------------------------------------------
        bool alreadyHandledY = false;
        if (donnyMode && !vehicleMode)
        {
            //rocket
            if (!don.canHasCheeseburger && CheckCollisionBoxes(rocketBox, don.outerBox))
            {
                don.canHasCheeseburger = true;
                toast = "Unlocked Hover mode for the Truck!";
                StartTimer(&toastTimer);
                don.xp += 100;
            }
            //are we in water?
            //bool inWater = don.pos.y < PLAYER_FLOAT_Y_POSITION;
            float feetY = DonFeetWorldY(&don);         // from donogan.h
            bool inWater = (feetY <= PLAYER_FLOAT_Y_POSITION + 0.01f); // tiny epsilon
            if (onLoad)//he floats underwater
            {
                if (closestCX < 0 || closestCY < 0 || closestCX >= CHUNK_COUNT || closestCY >= CHUNK_COUNT) {TraceLog(LOG_INFO, "Outside of world bounds: %d,%d", closestCX, closestCY);}
                else
                {
                    float groundY = GetTerrainHeightFromMeshXZ(don.pos.x, don.pos.z);
                    if (groundY < -9000.0f) { groundY = don.pos.y; } // if we error, dont change y
                    if (inWater)
                    {
                        don.seabedY = groundY;
                        if (don.pos.y < groundY)
                        {
                            don.groundY = groundY;
                        }
                    }
                    else if(!caveMode)
                    {
                        don.groundY = groundY;
                        alreadyHandledY = true;
                        // right after setting don.groundY
                        Vector3 nrm = GetTerrainNormalFromMeshXZ(don.pos.x, don.pos.z);
                        if (nrm.x == 0 && nrm.y == 0 && nrm.z == 0) nrm = (Vector3){ 0,1,0 }; // fallback
                        don.groundNormal = nrm;
                    }
                    else if (caveMode)
                    {
                        // Cave mode: do not let terrain force Donogan back up.
                        // Keep gravity/freefall working by placing fake ground far below him.
                        don.groundY = don.pos.y - 500.0f;
                        alreadyHandledY = false;
                        don.groundNormal = (Vector3){ 0, 1, 0 };
                    }
                }
            }
            //water
            // Edge transitions (use current stick magnitude & run-held for sensible initial state)
            if (!don.inWater && inWater) {
                TraceLog(LOG_INFO, "Entering Water");
                StartTimer(&don.swimEnterToExitLock);
                DonEnterWater(&don, moveMag);
            }
            else if (don.inWater && (don.groundY > PLAYER_FLOAT_Y_POSITION + 0.01f) && HasTimerElapsed(&don.swimEnterToExitLock)) {
                ResetTimer(&don.swimEnterToExitLock);
                bool runHeld = gpad.btnL3;
                TraceLog(LOG_INFO, "Exiting Water");
                DonExitWater(&don, moveMag, runHeld);
            }
        }
        if(onLoad) //truck, time to rock and roll!
        {
            bool hitRock[4] = { false , false , false , false };
            bool anyHitRock = false;
            bool hitTree = false;
            if (don.unlockedTruck && vehicleMode)
            {
                //building/scene collision for truck
                for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
                {
                    if (i == SCENE_HOME_CABIN_01
                        || (i == SCENE_HOME_NICE_01 && truckPosition.x < 1899 && truckPosition.x > 1846)
                        || (i == SCENE_HOME_NICE_02 && truckPosition.x < -607 && truckPosition.x > -662))
                    {
                        for (int i = 0; i < gEnvBoundingBoxCount; i++)//todo: if this list ever gets big add culling
                        {
                            if (CheckCollisionBoxes(gEnvBoundingBoxes[i].box, TruckBoxFront)
                                || CheckCollisionBoxes(gEnvBoundingBoxes[i].box, TruckBoxBack)
                                || CheckCollisionBoxes(gEnvBoundingBoxes[i].box, TruckBoxLeft)
                                || CheckCollisionBoxes(gEnvBoundingBoxes[i].box, TruckBoxRight))
                            {
                                if (gEnvBoundingBoxes[i].type == EBBT_GROUND){continue;}
                                else if (gEnvBoundingBoxes[i].type == EBBT_WALL)
                                {
                                    //stuff goes here
                                    // --- env wall collision response (MIT/MTD) ---
                                    const float SKIN = 0.02f;

                                    BoundingBox wall = gEnvBoundingBoxes[i].box;
                                    BoundingBox body[4] = { TruckBoxFront, TruckBoxBack, TruckBoxLeft, TruckBoxRight };

                                    Vector3 bestPush = (Vector3){ 0 };
                                    float   bestScore = 1e30f;

                                    for (int b = 0; b < 4; ++b)
                                    {
                                        BoundingBox bb = body[b];

                                        // per-axis overlaps (positive if overlapping on that side)
                                        float left = wall.max.x - bb.min.x;  // push +X
                                        float right = bb.max.x - wall.min.x; // push -X
                                        float bottom = wall.max.y - bb.min.y;  // push +Y
                                        float top = bb.max.y - wall.min.y; // push -Y
                                        float back = wall.max.z - bb.min.z;  // push +Z
                                        float front = bb.max.z - wall.min.z; // push -Z

                                        // safety: if not overlapping on some axis, skip
                                        if (left <= 0 || right <= 0 || bottom <= 0 || top <= 0 || back <= 0 || front <= 0) continue;

                                        // signed overlaps (pick smaller magnitude, keep sign)
                                        float ox = (left < right) ? left : -right;
                                        float oy = (bottom < top) ? bottom : -top;
                                        float oz = (back < front) ? back : -front;

                                        float ax = fabsf(ox), ay = fabsf(oy), az = fabsf(oz);
                                        Vector3 push = (Vector3){ 0 };

                                        // choose smallest absolute axis and add a tiny skin
                                        if (ax <= ay && ax <= az)      push.x = (ox > 0 ? ox + SKIN : ox - SKIN);
                                        else if (ay <= ax && ay <= az) push.y = (oy > 0 ? oy + SKIN : oy - SKIN);
                                        else                           push.z = (oz > 0 ? oz + SKIN : oz - SKIN);

                                        float score = fabsf(push.x) + fabsf(push.y) + fabsf(push.z);
                                        if (score < bestScore) { bestScore = score; bestPush = push; }
                                    }

                                    if (bestScore < 1e30f)
                                    {
                                        // Don’t yank the truck downward; allow stepping up but not down
                                        if (bestPush.y < 0.0f) bestPush.y = 0.0f;
                                        truckPosition = Vector3Add(truckPosition, bestPush);
                                        truckOrigin = Vector3Add(truckOrigin, bestPush);
                                        truckSpeed *= 0.6f;   // soften the impact a bit
                                        UpdateTruckBoxes();
                                    }
                                    //truckSpeed = 0;//for sticky collision instead of above, keep this one, its a good example!
                                }
                            }
                        }
                    }
                    else if (CheckCollisionBoxes(Scenes[i].box, TruckBoxFront)
                        || CheckCollisionBoxes(Scenes[i].box, TruckBoxBack)
                        || CheckCollisionBoxes(Scenes[i].box, TruckBoxLeft)
                        || CheckCollisionBoxes(Scenes[i].box, TruckBoxRight))
                    {
                        //stuff...thanks to chatGPT hahahaha muh hahahahah muh hahahah!!!! (hey look a nickel?)
                        // --- scene/building collision response (MIT/MTD push) ---
                        const float SKIN = 0.02f;
                        BoundingBox prop = Scenes[i].box;

                        Vector3 bestPush = (Vector3){ 0 };
                        float   bestScore = FLT_MAX;

                        BoundingBox body[4] = { TruckBoxFront, TruckBoxBack, TruckBoxLeft, TruckBoxRight };

                        for (int b = 0; b < 4; ++b)
                        {
                            BoundingBox bb = body[b];

                            // Signed overlaps (positive means we need to push +axis, negative -> push -axis)
                            float left = prop.max.x - bb.min.x;  // +X push
                            float right = bb.max.x - prop.min.x; // -X push
                            float bottom = prop.max.y - bb.min.y;  // +Y push
                            float top = bb.max.y - prop.min.y; // -Y push
                            float back = prop.max.z - bb.min.z;  // +Z push
                            float front = bb.max.z - prop.min.z; // -Z push

                            // If any pair doesn't overlap, skip (extra safety)
                            if (left <= 0 || right <= 0 || bottom <= 0 || top <= 0 || back <= 0 || front <= 0) continue;

                            float ox = (left < right) ? left : -right;
                            float oy = (bottom < top) ? bottom : -top;
                            float oz = (back < front) ? back : -front;

                            // Choose smallest absolute axis; add a tiny skin to avoid re-penetration next frame
                            float ax = fabsf(ox), ay = fabsf(oy), az = fabsf(oz);
                            Vector3 push = { 0 };

                            if (ax <= ay && ax <= az)      push.x = (ox > 0 ? ox + SKIN : ox - SKIN);
                            else if (ay <= ax && ay <= az) push.y = (oy > 0 ? oy + SKIN : oy - SKIN);
                            else                           push.z = (oz > 0 ? oz + SKIN : oz - SKIN);

                            // Score this candidate (L1 is fine here; L2 also ok)
                            float score = fabsf(push.x) + fabsf(push.y) + fabsf(push.z);
                            if (score < bestScore) { bestScore = score; bestPush = push; }
                        }

                        if (bestScore < FLT_MAX)
                        {
                            // Bias: don't yank the truck downward (lets you "step up" but not get slammed down)
                            if (bestPush.y < 0.0f) bestPush.y = 0.0f;

                            truckPosition = Vector3Add(truckPosition, bestPush);
                            truckOrigin = Vector3Add(truckOrigin, bestPush);

                            truckSpeed *= 0.6f;   // soften impact a bit
                            disableRoll = true;   // stabilize chassis for this frame
                            UpdateTruckBoxes();

                            // Optional: if one building hit is enough per frame, early-out the scene loop
                            // break;
                        }
                    }
                }

                //truck static props collision
                for (int i = 0; i < numCloseProps; i++)
                {
                    StaticGameObject tree = (*CloseProps[i]);
                    if (tree.type == MODEL_GRASS || tree.type == MODEL_GRASS_THICK || tree.type == MODEL_GRASS_LARGE) { continue; }
                    // keep boxes fresh (we rely on tire/body world AABBs)

                    const float SKIN = 0.01f;

                    // ---- ROCKS: roll over (lift) ----
                    if (tree.type == MODEL_ROCK || tree.type == MODEL_ROCK2 || tree.type == MODEL_ROCK3
                        || tree.type == MODEL_ROCK4 || tree.type == MODEL_ROCK5)
                    {
                        bool lifted = false;
                        for (int t = 0; t < 4; ++t)
                        {
                            BoundingBox tb = TruckBoxTires[t];

                            if (!CheckCollisionBoxes(tb, tree.outerBox)) continue;

                            // if the tire bottom is below the rock top, lift the truck so it rests on it
                            float tireBottom = tb.min.y;
                            float desiredBottom = tree.outerBox.max.y;
                            if (tireBottom < desiredBottom)
                            {
                                float dy = desiredBottom - tireBottom;
                                //truckPosition.y += dy;
                                //truckOrigin.y += dy;      // keep origin consistent for anything using it
                                //tireYPos[t] += dy;
                                Vector3 localOffset = RotateY(tireOffsets[t], -truckAngle);
                                Vector3 pos = Vector3Add(truckOrigin, localOffset);
                                float groundYy = desiredBottom;
                                if (groundYy < -9000.0f) { groundYy = pos.y; } // if we error, dont change y
                                pos.y = groundYy + 1.2;//this actually works well, adding 1.2 here
                                //tireYPos[t] = pos.y;
                                tireYPos[t] = Lerp(tireYPos[t], pos.y, 10.0f * dt);
                                tireYOffset[t] -= (tireBottom - groundYy) * dt;
                                if (tireYOffset[t] > 0.2f) { tireYOffset[t] = 0.2f; }
                                if (tireYOffset[t] < -0.12f) { tireYOffset[t] = -0.12f; }
                                lifted = true;
                                hitRock[t] = true;
                                anyHitRock = true;
                                TraceLog(LOG_INFO, "tire %d hit rock!", t);
                            }
                        }
                        if (lifted) { UpdateTruckBoxes(); }// keep boxes in sync
                        // Rocks do not block—rolling over is handled by vertical lift only.
                    }
                    else
                    {
                        // ---- EVERYTHING ELSE: MIT/MTD push like Donny ----
                        // narrow-phase against the actual prop box
                        BoundingBox prop = tree.box;
                        Vector3 push = (Vector3){ 0 };
                        bool hit = false;

                        // test each body box; sum axis-minimal pushes
                        BoundingBox body[4] = { TruckBoxFront, TruckBoxBack, TruckBoxLeft, TruckBoxRight };
                        for (int b = 0; b < 4; ++b)
                        {
                            if (!CheckCollisionBoxes(body[b], prop)) continue;

                            // per-axis overlaps (positive => penetration depth)
                            float left = prop.max.x - body[b].min.x; // +X push
                            float right = body[b].max.x - prop.min.x;    // -X push
                            float bottom = prop.max.y - body[b].min.y; // +Y (landing on top)
                            float top = body[b].max.y - prop.min.y;    // -Y (hitting underside)
                            float back = prop.max.z - body[b].min.z; // +Z push
                            float front = body[b].max.z - prop.min.z;    // -Z push
                            // pick smallest on each pair -> signed overlaps
                            float ox = (left < right) ? left : -right;
                            float oy = (bottom < top) ? bottom : -top;
                            float oz = (back < front) ? back : -front;  // formulation like Donny’s MIT resolver  :contentReference[oaicite:2]{index=2}

                            // choose axis with smallest absolute overlap for this box
                            float ax = fabsf(ox), ay = fabsf(oy), az = fabsf(oz);
                            if (ax <= ay && ax <= az)      push.x += (ox > 0 ? ox + SKIN : ox - SKIN);
                            else if (ay <= ax && ay <= az) push.y += (oy > 0 ? oy + SKIN : oy - SKIN);
                            else                           push.z += (oz > 0 ? oz + SKIN : oz - SKIN);

                            hit = true;
                        }

                        if (hit)
                        {
                            // bias: don’t yank the truck downward; allow stepping up, not down
                            if (push.y < 0.0f) push.y = 0.0f;

                            truckPosition = Vector3Add(truckPosition, push);
                            truckOrigin = Vector3Add(truckOrigin, push);
                            truckSpeed *= 0.6f;        // soften impact
                            disableRoll = true;        // lock roll for this frame on hard contact
                            UpdateTruckBoxes();
                            hitTree = true;
                        }
                    }
                }
            }
            bool rebuildFromTires = false;
            if (closestCX < 0 || closestCY < 0 || closestCX >= CHUNK_COUNT || closestCY >= CHUNK_COUNT) {
                // Outside world bounds
                TraceLog(LOG_INFO, "Truck. Outside of world bounds: %d,%d", closestCX, closestCY);
            }
            else
            {
                front = Vector3Add(truckPosition, RotateY(RotateX((Vector3){ 0.0f, 0.0f, truckFrontDim }, truckPitch), -truckAngle));//
                back  = Vector3Add(truckPosition, RotateY(RotateX((Vector3){ 0.0f, 0.0f, truckBackDim }, truckPitch), -truckAngle));//

                float frontY = GetTerrainHeightFromMeshXZ(front.x, front.z);
                float backY  = GetTerrainHeightFromMeshXZ(back.x, back.z);
                if(truckAirState!=AIRBORNE && frontY > -9000.0f && backY > -9000.0f)
                {
                    front.y = frontY;
                    back.y = backY;
                    float deltaY = frontY - backY;
                    float deltaZ = truckLength;  // Distance between front and back
                    float pitch = -atanf(deltaY / deltaZ);  // In radians
                    truckPitch = pitch;//set it directly here
                }
                if (truckAirState==AIRBORNE && !hoverMode) {
                    verticalVelocity -= GRAVITY * GetFrameTime();  // e.g. gravity = 9.8f
                    truckPosition.y += verticalVelocity * truckSpeed * GetFrameTime();

                    // Check for landing
                    float groundY = GetTerrainHeightFromMeshXZ(truckPosition.x, truckPosition.z);
                    if(groundY < -9000.0f){groundY=truckPosition.y;} 
                    if (truckPosition.y <= groundY) {
                        if (!anyHitRock) {
                            truckPosition.y = groundY;
                        }
                        verticalVelocity = 0;
                        truckAirState=LANDING;
                    }
                    //tireYOffsets
                    for(int i=0; i<4; i++)
                    {
                        Vector3 localOffset = RotateY(RotateX(tireOffsets[i], truckPitch), -truckAngle);
                        Vector3 pos = Vector3Add(truckOrigin, localOffset);
                        float groundYy = GetTerrainHeightFromMeshXZ(pos.x, pos.z);
                        if(groundYy < -9000.0f){groundYy=pos.y;} // if we error, dont change y
                        if(pos.y < groundYy)//tire hit the ground
                        {
                            if (hitRock[i]) continue;
                            truckAirState=LANDING;
                            tireYPos[i] = groundYy;
                            tireYOffset[i] -= (groundY - groundYy) * GetFrameTime();//move the tire up proportional to the difference between the truck y and tire y
                            if(tireYOffset[i]>0.12f){tireYOffset[i]=0.12f;}
                            if(tireYOffset[i]<-0.23f){tireYOffset[i]=-0.23f;}
                            rebuildFromTires = true;
                        }
                    }
                } 
                else if (!hoverMode)
                { //not airborne, either landing or ground
                    if(!Menu_IsOpen(&gGame) && gpad.btnCross>0)
                    {
                        truckAirState=AIRBORNE;
                        truckPosition.y+=1.28;
                        verticalVelocity = 16.0f * truckSpeed * truckSpeed; //burst
                    }
                    else
                    {
                        float groundY = GetTerrainHeightFromMeshXZ(truckPosition.x, truckPosition.z);
                        //TraceLog(LOG_INFO, "setting camera y: (%d,%d){%f,%f,%f}[%f]", closestCX, closestCY, camera.position.x, camera.position.y, camera.position.z, groundY);
                        if(groundY < -9000.0f){groundY=truckPosition.y;} // if we error, dont change y
                        if(truckAirState==GROUND && truckPosition.y>groundY)
                        {
                            if(truckPitch<-PI/4.0f && truckSpeed > 1.01f && !isTruckSliding) //not while sliding, this is basically shut off for now
                            {
                                //here, take off!
                                truckAirState=AIRBORNE;
                                verticalVelocity=3.2f * truckSpeed * dt; //natural
                            }
                        }
                        else//LANDING
                        {
                            if (!anyHitRock) {
                                truckPosition.y = groundY;
                            }
                        }
                        //tireYOffsets
                        for(int i=0; i<4; i++)
                        {
                            if (hitRock[i]) { continue; }
                            Vector3 localOffset = RotateY(tireOffsets[i], -truckAngle);
                            Vector3 pos = Vector3Add(truckOrigin, localOffset);
                            float groundYy = GetTerrainHeightFromMeshXZ(pos.x, pos.z);
                            if(groundYy < -9000.0f){groundYy=pos.y;} // if we error, dont change y
                            pos.y = groundYy;
                            tireYPos[i] = pos.y;
                            tireYOffset[i] -= (groundY - groundYy) * dt;
                            if(tireYOffset[i]>0.2f){tireYOffset[i]=0.2f;}
                            if(tireYOffset[i]<-0.12f){tireYOffset[i]=-0.12f;}
                            //TraceLog(LOG_INFO,"NOT HIT ROCK!!!");
                        }
                        rebuildFromTires = true;
                    }
                }
            }
            hoverLift = Lerp(hoverLift, hoverLiftTarget, dt * 3.5f);
            hoverTireFold = Lerp(hoverTireFold, hoverMode ? 1.0f : 0.0f, dt * 5.0f);

            if (hoverMode)
            {
                rebuildFromTires = false;
                if (truckAirState != AIRBORNE)
                {
                    float groundY = GetTerrainHeightFromMeshXZ(truckPosition.x, truckPosition.z);
                    if (groundY > -9000.0f)
                    {
                        float desiredY = groundY + TRUCK_Y_OFFSET_DRAW + hoverLift;
                        float minHoverY = WHALE_SURFACE + 3.2f;

                        if (desiredY < minHoverY)
                        {
                            desiredY = minHoverY;
                        }

                        truckPosition.y = Lerp(truckPosition.y, desiredY, dt * 6.0f);
                        truckAirState = GROUND;
                        gravityCollected = 0.0f;
                        truckForward.y = 0.0f;
                    }
                }
            }
            if(rebuildFromTires && !hoverMode)
            {
                // - rebuild truck from tires
                // Get corrected tire heights
                float fl = tireYPos[0];// + tireYOffset[0]; // front-left .. I think these need to be tracked but not used her
                float fr = tireYPos[1];// + tireYOffset[1]; // front-right
                float bl = tireYPos[2];// + tireYOffset[2]; // back-left
                float br = tireYPos[3];// + tireYOffset[3]; // back-right
                float maxTireY = fmax(fmax(fmax(fl, fr), bl), br);

                // 1. Truck vertical position (Y) — we want the max tire value//-average-of-all-tires-
                //truckPosition.y = Lerp(truckPosition.y , (fl + fr + bl + br) / 4.0f, GetFrameTime()*16.0f);
                if (don.unlockedTruck && !anyHitRock) { truckPosition.y = Lerp(truckPosition.y, maxTireY, GetFrameTime() * 16.0f); }

                // 2. Pitch (X-axis rotation, nose up/down)
                // front height vs back height
                float frontAvg = (fl + fr) / 2.0f;
                float backAvg  = (bl + br) / 2.0f;
                truckPitch = Lerp(truckPitch, -atan2f(frontAvg - backAvg, truckLength),GetFrameTime()*(hoverMode?0.01f:16.0f));  // pitch is positive when nose is up

                // 3. Roll (Z-axis rotation, lean left/right)
                // left height vs right height
                float leftAvg  = (fl + bl) / 2.0f;
                float rightAvg = (fr + br) / 2.0f;
                truckRoll = Lerp(truckRoll, -atan2f(rightAvg - leftAvg, truckWidth),GetFrameTime()*16.0f);//todo: do we need to lerp this, does it make it better or worse
                //--rebuild end ...
            }
            if (hoverMode)
            {
                float airBankMax = 38.0f * DEG2RAD;
                // negative or positive may need flipped depending on your feel
                float targetAirRoll = gpad.normLX * airBankMax;
                truckRoll = Lerp(truckRoll, targetAirRoll, GetFrameTime() * 5.5f);
            }
            if(truckRoll>PI){truckRoll=PI;}
            if(truckRoll<-PI){truckRoll=-PI;}
            if (hoverMode && truckPitch > 0){truckPitch = 0;}
            else if(truckPitch>PI){truckPitch=PI;}
            if(truckPitch<-PI){truckPitch=-PI;}
            if (vehicleMode)
            {
                camYaw = truckAngle * RAD2DEG + relativeYaw;
                float radYaw = camYaw * DEG2RAD;
                //float radPitch = relativePitch * DEG2RAD;
                Vector3 n = GetTerrainNormalFromMeshXZ(truckPosition.x, truckPosition.z);
                float upDot = Vector3DotProduct(n, (Vector3) { 0, 1, 0 });
                float slope01 = Clamp(1.0f - upDot, 0.0f, 0.35f);
                float hillCamDistance = camDistance + slope01 * 35.0f;
                float radPitch = relativePitch * DEG2RAD;
                //end new section
                float followSpeed = 5.0f * GetFrameTime();
                /*Vector3 offset = {
                    camDistance * cosf(radPitch) * sinf(radYaw),
                    camDistance * sinf(radPitch),
                    camDistance * cosf(radPitch) * cosf(radYaw)
                };*/
                Vector3 offset = {
                    hillCamDistance * cosf(radPitch) * sinf(radYaw),
                    hillCamDistance * sinf(radPitch),
                    hillCamDistance * cosf(radPitch) * cosf(radYaw)
                };

                Vector3 desiredCameraPos = Vector3Add(truckPosition, offset);
                camera.position = Vector3Lerp(camera.position, desiredCameraPos, followSpeed);

                Vector3 desiredTarget = Vector3Add(
                    Vector3Add(truckPosition,
                        RotateY((Vector3) { 0.0f, 0.0f, truckFrontDim - 0.8f },
                            -truckAngle)),
                    (Vector3) {
                    0.0f, 2.0f, 0.0f
                });
                camera.target = Vector3Lerp(camera.target, desiredTarget, followSpeed);
            }
        }
        if (onLoad && donnyMode) //donny collision (probably not all of it, ugh!)
        {
            bool hitEnvWall = false; //todo: do I actually want hitEnvWall, if not, just delete this and clean up.
            //env boxes (aka duct tape)
            for (int i = 0; i < gEnvBoundingBoxCount; i++)//todo: if this list ever gets big add culling
            {
                if (gEnvBoundingBoxes[i].disable) { continue; }
                if (CheckCollisionBoxes(don.box, gEnvBoundingBoxes[i].box))
                {
                    if (gEnvBoundingBoxes[i].type == EBBT_GROUND)
                    {
                        don.groundY = gEnvBoundingBoxes[i].box.max.y;
                        alreadyHandledY = true;
                    }
                    else if (gEnvBoundingBoxes[i].type == EBBT_WALL)
                    {
                        disableRoll = true;
                        hitEnvWall = true;
                        // Push Donogan out of WALL along min horizontal penetration (X/Z)
                        BoundingBox a = don.box;                         // player box
                        BoundingBox b = gEnvBoundingBoxes[i].box;        // wall box

                        // Centers and half-sizes
                        Vector3 ac = { (a.min.x + a.max.x) * 0.5f, (a.min.y + a.max.y) * 0.5f, (a.min.z + a.max.z) * 0.5f };
                        Vector3 bc = { (b.min.x + b.max.x) * 0.5f, (b.min.y + b.max.y) * 0.5f, (b.min.z + b.max.z) * 0.5f };
                        Vector3 ah = { (a.max.x - a.min.x) * 0.5f, (a.max.y - a.min.y) * 0.5f, (a.max.z - a.min.z) * 0.5f };
                        Vector3 bh = { (b.max.x - b.min.x) * 0.5f, (b.max.y - b.min.y) * 0.5f, (b.max.z - b.min.z) * 0.5f };

                        // Overlap depths
                        Vector3 d = { ac.x - bc.x, ac.y - bc.y, ac.z - bc.z };
                        float penX = (ah.x + bh.x) - fabsf(d.x);
                        float penY = (ah.y + bh.y) - fabsf(d.y);  // computed but ignored for WALLs
                        float penZ = (ah.z + bh.z) - fabsf(d.z);

                        // Choose X or Z (horizontal) axis with smaller penetration
                        Vector3 push = { 0 };
                        if (penX < penZ) {
                            push.x = (d.x > 0.0f) ? penX : -penX;   // push out along +X or -X
                            // if you track velocity, you can zero X here: // don.vel.x = 0;
                        }
                        else {
                            push.z = (d.z > 0.0f) ? penZ : -penZ;   // push out along +Z or -Z
                            // if you track velocity, you can zero Z here: // don.vel.z = 0;
                        }

                        // Apply correction to position and boxes (so subsequent walls see updated box)
                        don.pos.x += push.x;  don.pos.z += push.z;
                    }
                }
            }
            don.box = UpdateBoundingBox(don.origBB, don.pos);
            //door
            bool doorCollide = false;
            if (cottageDoorSlide < 0.98f && CheckCollisionBoxes(don.box, cottageDoorBox))
            {
                BoundingBox a = don.box;
                BoundingBox b = cottageDoorBox;

                Vector3 ac = { (a.min.x + a.max.x) * 0.5f, (a.min.y + a.max.y) * 0.5f, (a.min.z + a.max.z) * 0.5f };
                Vector3 bc = { (b.min.x + b.max.x) * 0.5f, (b.min.y + b.max.y) * 0.5f, (b.min.z + b.max.z) * 0.5f };
                Vector3 ah = { (a.max.x - a.min.x) * 0.5f, (a.max.y - a.min.y) * 0.5f, (a.max.z - a.min.z) * 0.5f };
                Vector3 bh = { (b.max.x - b.min.x) * 0.5f, (b.max.y - b.min.y) * 0.5f, (b.max.z - b.min.z) * 0.5f };

                Vector3 d = { ac.x - bc.x, ac.y - bc.y, ac.z - bc.z };
                float penX = (ah.x + bh.x) - fabsf(d.x);
                float penZ = (ah.z + bh.z) - fabsf(d.z);

                if (penX < penZ) don.pos.x += (d.x > 0.0f) ? penX : -penX;
                else             don.pos.z += (d.z > 0.0f) ? penZ : -penZ;
                /*don.pos.x -= 2;*/
                don.box = UpdateBoundingBox(don.origBB, don.pos);
                doorCollide = true;
            }
            //home collision
            don.inHome = false;
            for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
            {
                for (int a = 0; a < MAX_ARROWS; a++)//arrows
                {
                    if (!don.arrows[a].alive || don.arrows[a].stuck) { continue; }
                    if (!CheckCollisionBoxes(don.box, Scenes[i].box)) //if we are outside the building
                    {
                        if (CheckCollisionBoxes(don.arrows[a].box, Scenes[i].box))//stuck once in the box
                        {
                            don.arrows[a].stuck = true;
                        }
                    }
                    else // if we are inside the building
                    {
                        if (!CheckCollisionBoxes(don.arrows[a].box, Scenes[i].box))//stuck when out of the box
                        {
                            don.arrows[a].stuck = true;
                        }
                    }
                }
                if (CheckCollisionBoxes(don.box, Scenes[i].box))//donny home collision initial
                {
                    if (Scenes[i].modelType != MODEL_HOME_CASTLE) { don.inHome = true; } //exception for the castle, its wide open mostly
                    // classify slope: anything flatter than ~50° treated as ground
                    const float groundSlopeCos = DEFAULT_GROUND_SLOPE_COS; // or cosf(DEG2RAD*50.0f);
                    for (int it = 0; it < 3; ++it)
                    {
                        MeshBoxHit hit = CollideAABBWithMeshTriangles(don.outerBox, &HomeModels[Scenes[i].modelType].meshes[0], Scenes[i].pos, Scenes[i].scale, Scenes[i].yaw, groundSlopeCos, false);
                        if (hit.hitGround) {
                            // snap to ground and re-make AABB
                            if (!alreadyHandledY || don.groundY < hit.groundY)
                            {
                                don.groundY = hit.groundY; //overwrites ground collision
                            }
                        }
                        else if (hit.hitWall && !hitEnvWall) // if we already hit env bounding box, use that instead
                        {
                            disableRoll = true;

                            const float EPS = 0.0001f;
                            const float MAX_PUSH = 0.5f;
                            const float GOOD_DOT = 0.25f;

                            DebugLogMeshBoxHit("HOME", i, don.box, don.pos, hit, Scenes[i].pos, Scenes[i].scale);

                            // Candidate push from collider.
                            Vector3 p = hit.push;
                            p.y = 0.0f;

                            float pLen = Vector3Length(p);
                            if (pLen <= EPS)
                            {
                                break;
                            }

                            if (pLen > MAX_PUSH)
                            {
                                p = Vector3Scale(p, MAX_PUSH / pLen);
                                pLen = MAX_PUSH;
                            }

                            // -----------------------------------------------------------------
                            // Walk backward through Don's history and find the newest position
                            // that does NOT hit this same scene mesh.
                            // -----------------------------------------------------------------
                            Vector3 safePos = don.pos;
                            bool foundSafePos = false;

                            for (int hb = 1; hb <= DON_POS_HISTORY_MAX; hb++)
                            {
                                Vector3 testPos = Don_GetHistoryPosition(&don, hb);

                                // Keep current Y so this does not cancel jumping/falling.
                                testPos.y = don.pos.y;

                                BoundingBox testOuter = UpdateBoundingBox(don.origOuterBB, testPos);

                                MeshBoxHit testHit = CollideAABBWithMeshTriangles(
                                    testOuter,
                                    &HomeModels[Scenes[i].modelType].meshes[0],
                                    Scenes[i].pos,
                                    Scenes[i].scale,
                                    Scenes[i].yaw,
                                    groundSlopeCos,
                                    true
                                );

                                if (!testHit.hit || testHit.hitGround)
                                {
                                    safePos = testPos;
                                    foundSafePos = true;
                                    break;
                                }
                            }

                            if (foundSafePos)
                            {
                                Vector3 toSafe = Vector3Subtract(safePos, don.pos);
                                toSafe.y = 0.0f;

                                float toSafeLen = Vector3Length(toSafe);

                                // First teleport/rollback to the newest known non-hitting position.
                                don.pos = safePos;

                                // Then only apply p if it mostly points toward that safe direction.
                                // If p is wrong, replace it with same-sized push toward safe position.
                                if (toSafeLen > EPS && pLen > EPS)
                                {
                                    Vector3 toSafeDir = Vector3Scale(toSafe, 1.0f / toSafeLen);
                                    Vector3 pDir = Vector3Scale(p, 1.0f / pLen);

                                    float dot = Vector3DotProduct(pDir, toSafeDir);

                                    if (dot < GOOD_DOT)
                                    {
                                        p = Vector3Scale(toSafeDir, pLen);
                                    }

                                    don.pos = Vector3Add(don.pos, p);
                                }

                                TraceLog(LOG_WARNING,
                                    "[HOME HIST SAFE] scene=%d safe=(%.2f %.2f %.2f) p=(%.3f %.3f %.3f)",
                                    i,
                                    safePos.x, safePos.y, safePos.z,
                                    p.x, p.y, p.z
                                );

                                // Stop carried movement from shoving him right back in.
                                don.rollVel = (Vector3){ 0 };
                                don.velXZ = (Vector3){ 0 };
                            }
                            else
                            {
                                if (Scenes[i].type == SCENE_CINDER || Scenes[i].type == SCENE_CINDER_CAVE)
                                {
                                    TraceLog(LOG_WARNING, "[HOME HIST NO SAFE] cave: ignoring wall push");
                                    break;
                                }

                                TraceLog(LOG_INFO, "applying p fallback: %f %f %f", p.x, p.y, p.z);
                                don.pos = Vector3Add(don.pos, p);
                            }

                            Don_UpdateBoxes(&don);
                            break;
                        }
                    }
                }
            }
            // tune these a bit for your game feel
            const float STEP_HEIGHT = 0.45f;   // how high he can "step up" onto props
            const float SKIN_EPS = 0.001f;  // tiny bias to avoid re-penetration
            const float MIN_PUSH_EPS = 0.00001f;

            // previous-frame delta and box, used to know "from above" vs "from side"
            Vector3 deltaFrame = (Vector3){ don.pos.x - don.oldPos.x, don.pos.y - don.oldPos.y, don.pos.z - don.oldPos.z };
            BoundingBox prevBox = don.outerBox;
            prevBox.min.x -= deltaFrame.x; prevBox.max.x -= deltaFrame.x;
            prevBox.min.y -= deltaFrame.y; prevBox.max.y -= deltaFrame.y;
            prevBox.min.z -= deltaFrame.z; prevBox.max.z -= deltaFrame.z;

            //arrows and apples
            for (int a = 0; a < MAX_ARROWS; a++)
            {
                if (!don.arrows[a].alive || don.arrows[a].stuck) { continue; }
                for (int i = 0; i < MAX_APPLES_TOTAL; i++) {
                    if (!apples[i].spawned || apples[i].falling || apples[i].fallen) { continue; }
                    if (CheckCollisionBoxes(don.arrows[a].box, apples[i].box)) {
                        TraceLog(LOG_INFO, "Arrow hit apple %d -> falling", i);
                        don.arrows[a].stuck = true;
                        apples[i].falling = true;
                        apples[i].vel = (Vector3){ 0, -0.1f, 0 }; // initial drop
                        // Optionally nudge sideways from arrow direction
                    }
                }
            }
            
            // --- static prop collision donny---
            for (int i = 0; i < numCloseProps; i++)
            {
                StaticGameObject tree = (*CloseProps[i]);
                // broad-phase: ignore grass + outerBox cull
                if (tree.type == MODEL_GRASS || tree.type == MODEL_GRASS_THICK || tree.type == MODEL_GRASS_LARGE) { continue; }
                for (int a = 0; a < MAX_ARROWS; a++)
                {
                    if (!don.arrows[a].alive || don.arrows[a].stuck) { continue; }
                    if (CheckCollisionBoxes(don.arrows[a].box, tree.box))
                    {
                        don.arrows[a].stuck = true;
                    }
                }
                if (!CheckCollisionBoxes(don.outerBox, tree.outerBox)) continue;

                // narrow-phase: collide with the actual prop box
                if (!CheckCollisionBoxes(don.outerBox, tree.box)) continue;

                // compute per-axis overlaps (positive means penetration depth)
                float left = tree.box.max.x - don.outerBox.min.x; // push +X
                float right = don.outerBox.max.x - tree.box.min.x; // push -X
                float bottom = tree.box.max.y - don.outerBox.min.y; // push +Y (landing on top)
                float top = don.outerBox.max.y - tree.box.min.y; // push -Y (hitting underside)
                float back = tree.box.max.z - don.outerBox.min.z; // push +Z
                float front = don.outerBox.max.z - tree.box.min.z; // push -Z

                float ox = (left < right) ? left : -right;   float absOx = fabsf(ox);
                float oy = (bottom < top) ? bottom : -top;   float absOy = fabsf(oy);
                float oz = (back < front) ? back : -front;   float absOz = fabsf(oz);

                // Are we coming down from above and close enough to stand?
                bool descending = (deltaFrame.y <= 0.0f);
                bool wasAbove = (prevBox.min.y >= tree.box.max.y - 0.01f);
                bool canStep = ((don.outerBox.min.y - tree.box.max.y) <= STEP_HEIGHT + SKIN_EPS);

                // Prefer resolving vertically onto the top when appropriate
                if (descending && wasAbove && canStep && (absOy <= absOx) && (absOy <= absOz) && oy > MIN_PUSH_EPS) {
                    // snap feet onto the prop top
                    float snapUp = bottom + SKIN_EPS; // bottom is positive penetration to push +Y
                    don.pos.y += snapUp;
                    don.outerBox.min.y += snapUp;
                    don.outerBox.max.y += snapUp;

                    // optional: if you track vertical velocity, zero it here
                    // don.vel.y = 0.0f;

                    // you may also want to mark grounded: don.onGround = true;
                }
                else {
                    // side hit: push along the smallest horizontal penetration
                    if (absOx < absOz) {
                        // push in X
                        float pushX = (fabsf(ox) > MIN_PUSH_EPS) ? ox + ((ox > 0) ? SKIN_EPS : -SKIN_EPS) : 0.0f;
                        don.pos.x += pushX;
                        don.outerBox.min.x += pushX;
                        don.outerBox.max.x += pushX;

                        // optional: kill x-velocity into the wall if you track velocity
                        // if ((pushX > 0 && don.vel.x < 0) || (pushX < 0 && don.vel.x > 0)) don.vel.x = 0.0f;
                    }
                    else {
                        // push in Z
                        float pushZ = (fabsf(oz) > MIN_PUSH_EPS) ? oz + ((oz > 0) ? SKIN_EPS : -SKIN_EPS) : 0.0f;
                        don.pos.z += pushZ;
                        don.outerBox.min.z += pushZ;
                        don.outerBox.max.z += pushZ;
                    }
                }
            }
        }
        //platforms
        if (onLoad && donnyMode)
        {
            for (int i = 0; i < NUM_PLATS; i++)
            {
                if (Vector3DistanceSqr(plats[i].pos, don.pos) > 200*200) { continue; }
                Platform_CollideAndRide(&plats[i], &don, dt, plats);
            }
        }
        //collision for bad guys and attacks
        // //arrows
        for (int a = 0; a < MAX_ARROWS; a++)
        {
            if (!don.arrows[a].alive || don.arrows[a].stuck) { continue; }
            for (int i = 0; i < act_bg_count; i++)
            {
                int b = act_bg[i];
                if (!bg[b].active || bg[b].type == BG_GHOST) { continue; }
                if (CheckCollisionBoxes(don.arrows[a].box, bg[b].box))
                {
                    don.arrows[a].stuck = true;
                    if (bg[b].type == BG_YETI)
                    {
                        bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_ARROW, bg[b].type);
                        Yeti_KnockBackFromDonogan(&bg[b], &don);
                        BG_SetAnim(&bg[b], ANIM_YETI_ROAR, false);
                    }
                    else if (bg[b].type == BG_ROBO)
                    {
                        bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_ARROW, bg[b].type);
                        bg[b].state = ROBO_STATE_PLAN;
                    }
                    else if (bg[b].type == BG_PUMPKIN_HOPPER)
                    {
                        bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_ARROW, bg[b].type);
                        Hopper_KnockBackFromDonogan(&bg[b], &don);
                    }
                }
            }
        }
        //donogans spell balls
        for (int i = 0; i < MAX_BALLS; i++)
        {
            if (!balls[i].alive) { continue; }
            for (int k = 0; k < act_bg_count; k++)//todo: culling of some sort on this...
            {
                int b = act_bg[k];
                if (!bg[b].active) { continue; }
                if (bg[b].type == BG_GHOST && bg[b].state != GHOST_STATE_HIT && Vector3Distance(balls[i].pos, bg[b].pos) < balls[i].radius + 1.6f)//little outside the radius, hit!
                {
                    bg[b].state = GHOST_STATE_HIT;
                } 
                else if (bg[b].type == BG_YETI 
                    && bg[b].state != YETI_STATE_HIT && bg[b].state != YETI_STATE_DYING && bg[b].state != YETI_STATE_DEAD
                    && CheckCollisionBoxSphere(bg[b].box, balls[i].pos, balls[i].radius))//little outside the radius, hit!
                {
                    bg[b].state = YETI_STATE_HIT;
                    BG_SetAnim(&bg[b], ANIM_YETI_ROAR, false);
                    balls[i].alive = false;
                    bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_BALL, bg[b].type);
                }
                else if (bg[b].type == BG_ROBO && CheckCollisionBoxSphere(bg[b].box, balls[i].pos, balls[i].radius))
                {
                    bg[b].state = ROBO_STATE_PLAN;
                    balls[i].alive = false;
                    bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_BALL, bg[b].type); // I think this will kill it, low health on these guys...
                }
                else if (bg[b].type == BG_PUMPKIN_HOPPER && CheckCollisionBoxSphere(bg[b].box, balls[i].pos, balls[i].radius))
                {
                    bg[b].state = HOPPER_STATE_DEAD;
                    balls[i].alive = false;
                    bg[b].health = 0; // just kill hoppers
                }
            }
        }
        //donny collision with bg
        if (donnyMode)
        {
            for (int i = 0; i < act_bg_count; i++)//maybe we need to one time loop and find all active, and only do that every other loop or so
            {
                int b = act_bg[i];
                if (!bg[b].active) { continue; }
                //if (!CheckCollisionBoxes(bg[b].box, don.outerBox)) { continue; }
                bool punching = don.punching;
                bool bodyHit = CheckCollisionBoxes(bg[b].box, don.outerBox);
                bool punchHit = punching && CheckCollisionBoxes(bg[b].box, don.punchBox);
                bool wrenchHit = DonIsWrenchSwinging(&don) && CheckCollisionBoxes(bg[b].box, don.punchBox);
                DonAttackType atk = wrenchHit ? ATTACK_THROW : ATTACK_PUNCH;

                if (!bodyHit && !punchHit) { continue; }
                /*bool punching = (don.state == DONOGAN_STATE_PUNCH_CROSS_ENTER
                    || don.state == DONOGAN_STATE_PUNCH_JAB_ENTER
                    || don.state == DONOGAN_STATE_PUNCH_CROSS
                    || don.state == DONOGAN_STATE_PUNCH_JAB);*/
                if (bg[b].type == BG_GHOST && HasTimerElapsed(&don.hitTimer))
                {
                    //hit don
                    TraceLog(LOG_INFO, "ouch!");
                    don.health -= 5;
                    DonSetState(&don, DONOGAN_STATE_HIT);
                    StartTimer(&don.hitTimer);
                }
                else if (bg[b].type == BG_YETI && bg[b].state != YETI_STATE_HIT)//want this extra check here, so neither happens
                {
                    if (punchHit) {
                        // punched a yeti!
                        TraceLog(LOG_INFO, "punched a yeti!");
                        bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_PUNCH, bg[b].type);
                        Yeti_KnockBackFromDonogan(&bg[b], &don);
                        if (wrenchHit) {
                            Vector3 dir = Vector3Subtract(bg[b].pos, don.pos);
                            dir.y = 0.35f;
                            dir = Vector3Normalize(dir);

                            bg[b].vel = Vector3Scale(dir, 55.0f);
                            bg[b].vel.y = 22.0f;
                            bg[b].state = YETI_STATE_ATTACK; // crude but gives airborne physics
                        }
                        BG_SetAnim(&bg[b], ANIM_YETI_ROAR, false);
                    }
                    else if (HasTimerElapsed(&don.hitTimer)) {
                        // yeti damages Don (cooldown applies here only)
                        TraceLog(LOG_INFO, "ouch! yeti oof!");
                        don.health -= 10;
                        DonSetState(&don, DONOGAN_STATE_HIT);
                        StartTimer(&don.hitTimer);
                    }
                }
                else if (bg[b].type == BG_ROBO)
                {
                    if (wrenchHit)
                    {
                        bg[b].health -= GetDamageDone(&gGame, &don, atk, bg[b].type);
                        Vector3 dir = Vector3Subtract(bg[b].pos, don.pos);
                        dir.y = 0.35f;
                        dir = Vector3Normalize(dir);

                        bg[b].vel = Vector3Scale(dir, 55.0f);
                        bg[b].vel.y = 22.0f;
                        bg[b].state = ROBO_STATE_PLAN;
                    }
                    else if (punchHit)
                    {
                        bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_PUNCH, bg[b].type);
                    }
                    else
                    {
                        bg[b].health -= 1;
                        bg[b].state = ROBO_STATE_PLAN;
                    }
                }
                else if (bg[b].type == BG_PUMPKIN_HOPPER)
                {
                    if (don.state == DONOGAN_STATE_JUMPING || don.state == DONOGAN_STATE_JUMP_LAND)
                    {
                        // hopped a hopper!
                        TraceLog(LOG_INFO, "hopped a hopper!");
                        bg[b].health = 0;
                        bg[b].state = HOPPER_STATE_DEAD;
                        don.velY = don.jumpSpeed;   // or *1.1f for extra juice
                        don.state = DONOGAN_STATE_JUMPING;
                        don.onGround = false;
                    }
                    else if (wrenchHit)
                    {
                        bg[b].health -= GetDamageDone(&gGame, &don, atk, bg[b].type);
                        Vector3 dir = Vector3Subtract(bg[b].pos, don.pos);
                        dir.y = 0.35f;
                        dir = Vector3Normalize(dir);

                        bg[b].vel = Vector3Scale(dir, 55.0f);
                        bg[b].vel.y = 22.0f;
                        bg[b].state = HOPPER_STATE_HURT;
                    }
                    else if (punchHit)
                    {
                        TraceLog(LOG_INFO, "punched a hopper!");
                        bg[b].health -= GetDamageDone(&gGame, &don, ATTACK_PUNCH, bg[b].type);
                        Hopper_KnockBackFromDonogan(&bg[b], &don);
                    }
                    else if (HasTimerElapsed(&don.hitTimer)) {
                        TraceLog(LOG_INFO, "ouch! hopper oof!");
                        don.health -= 10;
                        DonSetState(&don, DONOGAN_STATE_HIT);
                        StartTimer(&don.hitTimer);
                    }
                }
            }
        }
        // bad guy vs home/building boxes
        if (onLoad)
        {
            for (int bi = 0; bi < act_bg_count; bi++)
            {
                int b = act_bg[bi];

                if (!bg[b].active) continue;
                if (bg[b].dead) continue;
                if (bg[b].type == BG_GHOST) continue; // ghosts are not corporeal
                if (bg[b].gbm_index < 0) continue;

                for (int s = 0; s < SCENE_TOTAL_COUNT; s++)
                {
                    if (!CheckCollisionBoxes(bg[b].box, Scenes[s].box)) continue;

                    // Orbs die when they hit a building
                    if (bg[b].type == BG_ROBO)
                    {
                        bg[b].health = 0;
                        bg[b].vel = (Vector3){ 0.0f, -4.0f, 0.0f };
                        bg[b].state = ROBO_STATE_DYING;
                        break;
                    }

                    // Yeti/hopper: push straight away from building center, no sliding
                    Vector3 sceneCenter = {
                        (Scenes[s].box.min.x + Scenes[s].box.max.x) * 0.5f,
                        (Scenes[s].box.min.y + Scenes[s].box.max.y) * 0.5f,
                        (Scenes[s].box.min.z + Scenes[s].box.max.z) * 0.5f
                    };

                    Vector3 away = Vector3Subtract(bg[b].pos, sceneCenter);
                    away.y = 0.0f;

                    if (Vector3Length(away) < 0.001f)
                    {
                        away = Vector3Subtract(bg[b].pos, bg[b].spawnPoint);
                        away.y = 0.0f;
                    }

                    if (Vector3Length(away) < 0.001f)
                    {
                        away = (Vector3){ 0.0f, 0.0f, 1.0f };
                    }

                    away = Vector3Normalize(away);

                    bg[b].pos = Vector3Add(bg[b].pos, Vector3Scale(away, 4.0f));
                    bg[b].targetPos = bg[b].pos;
                    bg[b].vel = (Vector3){ 0 };

                    float gy = BG_GroundY(bg[b].pos);
                    if (gy > -9000.0f) bg[b].pos.y = gy;

                    bg[b].box = UpdateBoundingBox(bgModelBorrower[bg[b].gbm_index].origBox, bg[b].pos);

                    if (bg[b].type == BG_YETI)
                    {
                        bg[b].state = YETI_STATE_PLANNING;
                        BG_SetAnim(&bg[b], ANIM_YETI_WALK, false);
                    }
                    else if (bg[b].type == BG_PUMPKIN_HOPPER)
                    {
                        bg[b].state = HOPPER_STATE_WAIT;
                        ResetTimer(&bg[b].interactionTimer);
                        StartTimer(&bg[b].interactionTimer);
                    }

                    break;
                }
            }
        }
        // hopper vs still platforms
        for (int b = 0; b < bg_count; b++)
        {
            if (!bg[b].active) continue;
            if (bg[b].type != BG_PUMPKIN_HOPPER) continue;
            if (bg[b].dead) continue;

            bg[b].groundY = GetTerrainHeightFromMeshXZ(bg[b].pos.x, bg[b].pos.z);
            bg[b].onPlatform = false;

            for (int p = 0; p < NUM_PLATS; p++)
            {
                if (plats[p].disabled) continue;
                if (plats[p].type != PLATFORM_STILL) continue;

                const float skin = 0.4f;

                bool xz =
                    bg[b].pos.x >= plats[p].box.min.x - skin &&
                    bg[b].pos.x <= plats[p].box.max.x + skin &&
                    bg[b].pos.z >= plats[p].box.min.z - skin &&
                    bg[b].pos.z <= plats[p].box.max.z + skin;

                if (!xz) continue;

                float topY = plats[p].box.max.y;

                bool nearTop =
                    bg[b].pos.y >= topY - 3.0f &&
                    bg[b].pos.y <= topY + 12.0f;

                if (!nearTop) continue;

                bg[b].groundY = topY;
                bg[b].onPlatform = true;
                break;
            }
        }
        //end collision section -----------------------------------------------------------------------------------------------------------------

        //updates before drawing--------------------------------------------------------
        // Lock sky to the real camera’s yaw, ignore pitch and translation
        Vector3 camDir = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        Vector3 skyDir = (Vector3){ camDir.x, camDir.y, camDir.z };           // yaw-only? maybe not
        if (Vector3Length(skyDir) < 1e-4f) skyDir = (Vector3){ 0,0,1 };     // fallback
        skyCam.position = (Vector3){ 0,0,0 };                                // remove translation
        skyCam.target = Vector3Add(skyCam.position, Vector3Normalize(skyDir));
        skyCam.up = (Vector3){ 0,1,0 };
        if (donnyMode && don.shook>0)//apply shook jitter
        {
            float amp = don.shook; // *don.shook;
            // Smooth “fake noise” = sum of a few sines at different freqs
            float ox = sinf(17.0f * amp) + sinf(13.0f * amp * 1.31f);
            float oy = sinf(19.0f * amp * 0.91f) + sinf(23.0f * amp * 1.07f);
            float oz = sinf(29.0f * amp * 1.19f) + sinf(31.0f * amp * 0.83f);

            // Max offsets in world units; keep these small
            const Vector3 maxOfs = { 0.25f, 0.15f, 0.25f };

            camera.position.x += amp * 0.5f * ox * maxOfs.x;
            camera.position.y += amp * 0.5f * oy * maxOfs.y;
            camera.position.z += amp * 0.5f * oz * maxOfs.z;
            don.shook += dt;
            if (don.shook > 1.2)
            {
                don.shook = false;
            }
        }
        // BeginMode3D(skyCam) ... draw panels around (0,0,0) as you already do
        //UpdateCamera(&camera, vehicleMode||donnyMode?CAMERA_THIRD_PERSON:CAMERA_FIRST_PERSON);
        //UpdateCamera(&skyCam, CAMERA_FIRST_PERSON);
        // -------- State + animation update from controller --------
        if (!vehicleMode && donnyMode)
        {
            // Pick speed from Donogan state (will be set by DonUpdate), account for swimming
            float speed = (don.state == DONOGAN_STATE_RUN) ? don.runSpeed :(don.state == DONOGAN_STATE_WALK) ? don.walkSpeed : 0.0f;

            if (don.state != DONOGAN_STATE_SLIDE)
            {
                // Integrate position
                don.pos = Vector3Add(don.pos, Vector3Scale(donMove, speed * dt));
            }

            // Face movement direction when moving (turn smoothly)
            if (moveMag > 0.1f && don.state != DONOGAN_STATE_SLIDE) {
                float targetYaw = atan2f(donMove.x, donMove.z);
                float dy = targetYaw - don.yawY;
                // wrap shortest path
                while (dy > PI) dy -= 2.0f * PI;
                while (dy < -PI) dy += 2.0f * PI;
                float maxTurn = (don.bowMode? don.bowTurnSpeed : don.turnSpeed) * dt;
                if (dy > maxTurn) dy = maxTurn;
                else if (dy < -maxTurn) dy = -maxTurn;
                don.yawY += dy;
            }

            // Apply facing rotation into model.transform (Y) on top of baked X fix
            Matrix rotY = MatrixRotateY(don.bowMode ? don.yawY + PI/2.0f: don.yawY);
            don.model.transform = MatrixMultiply(MatrixRotateX(DEG2RAD * don.modelYawX), rotY);
        }
        if (don.inWater)
        {
            don.camPitch = pitch;    // your orbit pitch
        }
        if (vehicleMode) { UpdateTruckBoxes(); }
        //update bg
        if (onLoad && donnyMode && loop_counter%69==0 && CheckSpawnAndActivateNext(don.pos)) //hopefully we support short circuiting, I would assume
        { 
            TraceLog(LOG_INFO, "Uh Oh! Here Comes Trouble...!"); 
        }
        if (onLoad)
        {
            for (int i = 0; i < NPC_TOTAL; i++)
            {
                if (Vector3DistanceSqr(don.pos, npcs[i].pos) > 1000.0f*1000) { continue; }
                NPC_Update(&npcs[i], &don, GetFrameTime());
            }
            BG_UpdateAll(&don, dt);
        }
        don.drawColor = LerpColor(don.drawColor,!HasTimerElapsed(&don.hitTimer)?targetHitColor:WHITE , dt);
        if (HasTimerElapsed(&don.hitTimer)) { don.drawColor.a = 255; }
        DonUpdate(&don, havePad ? &gpad : NULL, dt, vehicleMode, disableRoll);
        Garden_Update(&don, gpad.btnSquare);
        wasCaveMode = caveMode;
        caveMode = donnyMode && IsInCaveMode(&don, (caveMode || wasCaveMode));

        if (caveMode && !wasCaveMode)
        {
            TraceLog(LOG_INFO, "Entering cave mode");
        }

        if (!caveMode && wasCaveMode)
        {
            TraceLog(LOG_INFO, "Leaving cave mode");
        }
        // safety: if Donny is floating after truck/warp/load, force normal falling
        if (onLoad && donnyMode && !vehicleMode && !don.inWater && !don.gluedToPlatform && !don.inHome)
        { //todo: this is to handle bugs with floating at a certain hieght like when exiting the truck, does it work tho?
            float feetY = DonFeetWorldY(&don);
            float airGap = feetY - don.groundY;

            if (airGap > 1.25f &&
                (don.state == DONOGAN_STATE_IDLE ||
                don.state == DONOGAN_STATE_WALK ||
                don.state == DONOGAN_STATE_RUN))
            {
                don.onGround = false;
                don.velY = 0.0f;
                DonSetState(&don, DONOGAN_STATE_JUMPING);
            }
        }
        UpdateApples(dt);
        ConsumeMaps(&don);
        //machines
        Machine_Update(dt, &don, &truckPosition);
        if (donnyMode && !vehicleMode)
        {
            PushDonnyOutOfMachineWorldBoxes(&don);
            disableRoll = true;
        }
        WaterWheel_Update(dt);
        if (cottageDoorOpen)
        {
            cottageDoorSlide += GetFrameTime() * 0.7f;
            if (cottageDoorSlide > 1.0f) cottageDoorSlide = 1.0f;
        }
        Vector3 doorPos = Vector3Lerp(cottageDoorClosed, cottageDoorOpenPos, cottageDoorSlide);
        cottageDoorBox = MakeCottageDoorBox(doorPos);
        //shark
        Shark_Update(&shark, &don, dt);
        //punchin and fightin and cusin
        /*don.punching = DonIsPunching(&don);
        don.punchBox = DonMakePunchBox(&don);*/
        don.punching = DonIsPunching(&don) || DonIsWrenchSwinging(&don);
        if (DonIsWrenchSwinging(&don)) don.punchBox = DonMakeWrenchBox(&don);
        else                           don.punchBox = DonMakePunchBox(&don);
        //whale farts
        if (onLoad && !missions[MISSION_FART_WHALE].complete)
        {
            for (int w = 0; w < numWhales; w++)
            {
                bool hitWhale = false;

                for (int b = 0; b < DON_MAX_BUBBLES; b++)
                {
                    if (!don.bubbles[b].alive) { continue; }

                    if (CheckCollisionBoxes(don.bubbles[b].box, whales[w].box))
                    {
                        hitWhale = true;
                        don.bubbles[b].alive = 0; // optional, prevents repeated hits
                        break;
                    }
                }

                if (hitWhale)
                {
                    missions[MISSION_FART_WHALE].complete = true;
                    toast = "Completed mission! You farted on a whale!";
                    StartTimer(&toastTimer);
                    don.xp += 100;
                    don.money += 500;
                    break;
                }
            }
        }
        // Update the light shader with the camera view position
        SetShaderValue(lightningBugShader, lightningBugShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
        SetShaderValue(instancingLightShader, instancingLightShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
        if (onLoad && gGame.currentMusicLoaded) 
        {
            // Keep music stream ticking
            UpdateMusicStream(gGame.currentMusic);
            // End-of-track detection
            const float len = GetMusicTimeLength(gGame.currentMusic);
            const float played = GetMusicTimePlayed(gGame.currentMusic);
            const float EPS = 0.02f; // seconds threshold near the end
            // 3 ways we consider a song "finished":
            // 1) Crossed into the last EPS window this frame
            bool crossedEnd = (len > 0.0f) && (gSongPrevPlayed < (len - EPS)) && (played >= (len - EPS));
            // 2) Stream wrapped back to small time (in case driver/codec loops internally)
            bool wrapped = (len > 0.0f) && (gSongPrevPlayed > 1.0f) && (played < 0.05f);
            // 3) Stream reports stopped (extra guard)
            bool stopped = !IsMusicStreamPlaying(gGame.currentMusic) && (gSongPrevPlayed > 0.1f);
            if (crossedEnd || wrapped || stopped) {
                Audio_SelectSongRelative(+1);   // next song; modulo wrap keeps album looping
                // gSongPrevPlayed reset happens inside Audio_SelectSongRelative()
            }
            else {
                gSongPrevPlayed = played;
            }
        }
        //-------------------------------------------------------------------------------
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        BeginDrawing();
        ClearBackground(backGroundColor);
        //skybox separate scene
        BeginMode3D(skyCam);
            //skybox stuff
            rlDisableDepthMask();
            Vector3 cam = skyCam.position;
            float dist = 60.0f;
            float size = dist * 2.0f; //has to be double to line up
            // FRONT (+Z)
            DrawSkyboxPanelFixed(skyboxPanelFrontModel, (Vector3) { cam.x, cam.y, cam.z - dist }, 0.0f, (Vector3) { 0, 1, 0 }, size);
            // BACK (-Z)
            DrawSkyboxPanelFixed(skyboxPanelBackModel, (Vector3) { cam.x, cam.y, cam.z + dist }, 180.0f, (Vector3) { 0, 1, 0 }, size);
            // LEFT (-X)
            DrawSkyboxPanelFixed(skyboxPanelLeftModel, (Vector3) { cam.x - dist, cam.y, cam.z }, 90.0f, (Vector3) { 0, 1, 0 }, size);
            // RIGHT (+X)
            DrawSkyboxPanelFixed(skyboxPanelRightModel, (Vector3) { cam.x + dist, cam.y, cam.z }, -90.0f, (Vector3) { 0, 1, 0 }, size);
            // UP (+Y)
            DrawSkyboxPanelFixed(skyboxPanelUpModel, (Vector3) { cam.x, cam.y + dist, cam.z }, 90.0f, (Vector3) { 1, 0, 0 }, size);
            rlEnableDepthMask();
        EndMode3D();
        //regular scene of the map
        BeginMode3D(camera);
            if(onLoad){SetCustomCameraProjection(camera, 45.0f, (float)sw/sh, 0.3f, 5000.0f);} // Near = 1, Far = 4000
            //rlDisableBackfaceCulling();
            bool loadedEem = true;
            int loadCnt = 0;
            //int loadTileCnt = 0; -- this one needs to be global so we can update it while loading tiles
            //get frustum
            Matrix view = MatrixLookAt(camera.position, camera.target, camera.up);
            Matrix proj = MatrixPerspective(DEG2RAD * camera.fovy, sw / (float)sh, 0.1f, 5200.1f);
            Matrix projChunk8 = MatrixPerspective(DEG2RAD * camera.fovy, sw / (float)sw, 0.1f, 16384.0f);//for far away chunks
            Matrix vp = MatrixMultiply(view, proj);
            Matrix vpChunk8 = MatrixMultiply(view, projChunk8);
            Frustum frustum = ExtractFrustum(vp);
            Frustum frustumChunk8 = ExtractFrustum(vpChunk8);
            //IsBoxInFrustum
            FindClosestChunkAndAssignLod(vehicleMode?truckPosition:camera.position);
            int gx, gy;
            GetGlobalTileCoords(vehicleMode?truckPosition:camera.position, &gx, &gy);
            int playerTileX  = gx % TILE_GRID_SIZE; //tile number local to current chunk
            int playerTileY  = gy % TILE_GRID_SIZE; //tile number local to current chunk
            //truck
            //Draw the truck **********
            truckPitchYOffset = (sinf(truckPitch) * (truckLength / 2.0f))+(sinf(truckRoll) * (truckWidth / 2.0f));//set this every time tight before draw
            if(truckPitch>PI/2.0f){truckPitch=PI/2.0f;}//straight up
            if(truckPitch<-PI/2.0f){truckPitch=-PI/2.0f;}//staight down
            //DrawModel(truck, Vector3Add(truckPosition, truckBedPosition), 4.8f, WHITE);
            float truckYOffsetDraw = 1.62f;
            truckOrigin.y+=truckYOffset;//draw above ground
            //printf("truckAngle: %f\n", truckAngle);
            Matrix scaleTruckMatrix = MatrixScale(4.8f,4.8f,4.8f);
            float finalTruckYaw = truckAngle+truckTrickYaw;
            if (hoverMode){truckPitch /= 2;}
            float finalTruckPitch = truckPitch - truckTrickPitch;
            float finalTruckRoll = truckRoll+truckTrickRoll;
            Quaternion qYaw   = QuaternionFromAxisAngle((Vector3){ 0, 1, 0 }, finalTruckYaw);
            Quaternion qPitch = QuaternionFromAxisAngle((Vector3){ 1, 0, 0 }, finalTruckPitch);
            Quaternion qRoll  = QuaternionFromAxisAngle((Vector3){ 0, 0, 1 }, finalTruckRoll);
            Quaternion q = QuaternionMultiply(qYaw, QuaternionMultiply(qPitch, qRoll));
            Matrix rotationTruck = QuaternionToMatrix(q);
            //Quaternion q = QuaternionMultiply(QuaternionMultiply(qRoll, qPitch), qYaw);
            rotationTruck = MatrixMultiply(scaleTruckMatrix,rotationTruck);
            // Step 3: Apply position translation
            rotationTruck.m12 = truckOrigin.x;
            rotationTruck.m13 = Lerp(truckOrigin.y + truckYOffsetDraw, truckOrigin.y + truckYOffsetDraw + truckPitchYOffset, 0.01f); //!!!!SPACE TRUCK!!!!
            rotationTruck.m14 = truckOrigin.z;
            //donogan
            if (onLoad && donnyMode && !don.eatenByShark) //do not draw once he was eaten
            {
                //rocket
                if (!don.canHasCheeseburger) { DrawModel(rocketModel, rocketPos, 1, WHITE); }
                // Draw Donogan
                DrawModel(don.model, don.pos, don.scale, don.drawColor); // uses model.transform for rotation
                if (displayBoxes) 
                { 
                    DrawBoundingBox(don.box, RED); 
                    DrawBoundingBox(don.outerBox, GREEN);
                    DrawBoundingBox(don.innerBox, YELLOW);
                    for (int i = 0; i < gEnvBoundingBoxCount; i++)//env boxes, duct tape
                    {
                        DrawBoundingBox(gEnvBoundingBoxes[i].box, MAGENTA);
                    }
                    if (don.punching) { DrawBoundingBox(don.punchBox, ORANGE); }
                }

                //bow stuff
                if (don.bowMode)
                {
                    // --- Bow draw snippet ---
                    // Local bow rotation from bowEulerDeg
                    Quaternion qLocal = QuaternionFromEuler(
                        DEG2RAD * don.bowEulerDeg.x,
                        DEG2RAD * don.bowEulerDeg.y,
                        DEG2RAD * don.bowEulerDeg.z
                    );
                    Matrix Rlocal = QuaternionToMatrix(QuaternionNormalize(qLocal));
                    // Local bow offset
                    Matrix Toffset = MatrixTranslate(don.bowOffset.x, don.bowOffset.y, don.bowOffset.z);
                    // Character transform pieces
                    Matrix Rchar = don.model.transform;                               // yaw + baked X-fix
                    Matrix Schar = MatrixScale(don.scale, don.scale, don.scale);
                    Matrix Sbow = MatrixScale(don.bowScale, don.bowScale, don.bowScale);
                    Matrix Tchar = MatrixTranslate(don.pos.x, don.pos.y, don.pos.z);
                    // Compose final: Scale * (CharRot * BoneRot * LocalRot * Offset) * WorldTranslate
                    Matrix finalM = MatrixMultiply(MatrixMultiply(Sbow,Schar),MatrixMultiply(MatrixMultiply(MatrixMultiply(Rlocal, Toffset), Rchar),Tchar));
                    /*Matrix finalM = MatrixMultiply(MatrixMultiply(Sbow, Schar),
                        MatrixMultiply(MatrixMultiply(MatrixMultiply(Rlocal, Toffset), MatrixMultiply(Rchar, Rbone)),
                            Tchar));*/
                    // Draw
                    DrawMesh(don.bowModel.meshes[0], don.bowModel.materials[0], finalM);
                    /*don.bowModel.transform = finalM;
                    DrawModel(don.bowModel, (Vector3) { 0 }, 1.0f, WHITE);*/
                }

                // wrench stuff
                if (don.hasWrench && (don.curAnimId == DONOGAN_ANIM_PROC_MACHINE_TURN || don.curAnimId == DONOGAN_ANIM_PROC_WRENCH_SWING))
                {
                    // hard-coded wrench placement/tuning
                    Vector3 wrenchOffset = { -0.67f, 3.33f, 1.2f };
                    Vector3 wrenchEulerDeg = { 90.0f, 0.0f, 180.0f };
                    float wrenchScale = 0.25f;   // start big so we can SEE it first
                    if (don.curAnimId == DONOGAN_ANIM_PROC_WRENCH_SWING)
                    {
                        float t = don.animTime / 0.40f;
                        if (t < 0.0f) t = 0.0f;
                        if (t > 1.0f) t = 1.0f;

                        Vector3 p0 = { -1.35f, 3.15f, 0.35f }; // back/outside wind-up
                        Vector3 p1 = { -0.35f, 3.55f, 1.65f }; // forward strike
                        Vector3 p2 = { 0.95f, 3.75f, 1.10f }; // across body
                        Vector3 p3 = { -0.67f, 3.33f, 1.20f }; // recover/default

                        if (t < 0.33f) {
                            float u = t / 0.33f;
                            wrenchOffset = Vector3Lerp(p0, p1, u);
                        }
                        else if (t < 0.66f) {
                            float u = (t - 0.33f) / 0.33f;
                            wrenchOffset = Vector3Lerp(p1, p2, u);
                        }
                        else {
                            float u = (t - 0.66f) / 0.34f;
                            wrenchOffset = Vector3Lerp(p2, p3, u);
                        }

                        // optional: rotate it through the swing too
                        Vector3 r0 = { 90.0f, 0.0f, 220.0f };
                        Vector3 r1 = { 70.0f, 0.0f, 145.0f };
                        Vector3 r2 = { 105.0f, 0.0f, 80.0f };
                        Vector3 r3 = { 90.0f, 0.0f, 180.0f };

                        if (t < 0.33f) {
                            float u = t / 0.33f;
                            wrenchEulerDeg = Vector3Lerp(r0, r1, u);
                        }
                        else if (t < 0.66f) {
                            float u = (t - 0.33f) / 0.33f;
                            wrenchEulerDeg = Vector3Lerp(r1, r2, u);
                        }
                        else {
                            float u = (t - 0.66f) / 0.34f;
                            wrenchEulerDeg = Vector3Lerp(r2, r3, u);
                        }
                    }
                    // local wrench rotation from hard-coded euler
                    Quaternion qLocal = QuaternionFromEuler(
                        DEG2RAD * wrenchEulerDeg.x,
                        DEG2RAD * wrenchEulerDeg.y,
                        DEG2RAD * wrenchEulerDeg.z
                    );
                    Matrix Rlocal = QuaternionToMatrix(QuaternionNormalize(qLocal));

                    // local wrench offset
                    Matrix Toffset = MatrixTranslate(
                        wrenchOffset.x,
                        wrenchOffset.y,
                        wrenchOffset.z
                    );

                    // same style as bow
                    Matrix Rchar = don.model.transform;
                    Matrix Schar = MatrixScale(don.scale, don.scale, don.scale);
                    Matrix Swrench = MatrixScale(wrenchScale, wrenchScale, wrenchScale);
                    Matrix Tchar = MatrixTranslate(don.pos.x, don.pos.y, don.pos.z);

                    Matrix finalM = MatrixMultiply(
                        MatrixMultiply(Swrench, Schar),
                        MatrixMultiply(
                            MatrixMultiply(Rlocal, Toffset),
                            MatrixMultiply(Rchar, Tchar)
                        )
                    );

                    DrawMesh(don.wrenchModel.meshes[0], don.wrenchModel.materials[0], finalM);
                }
                DonDrawArrows(&don);
                DrawBalls(camera, ball, lightningBall);
                DrawLasers();
                if (!caveMode) { DrawDonShadow(&don); }
                //bubbles
                if (don.inWater) { DonDrawBubbles(&don, displayBoxes); }
            }
            if (onLoad)
            {
                Garden_Draw(&don, frustum);
                if (Vector3DistanceSqr(tolPos, don.pos) < 2048*3000)
                {
                    DrawTreeOfLifeBloom();
                }
                //tree of life
                if (IsPointInFrustum(tolPos, frustumChunk8))
                {
                    DrawModel(tol, tolPos, 8.0f, WHITE); //(Color) {160,100,220,255}//purple lol!
                }
                //atreyu
                if (IsPointInFrustum(atreyuPos, frustumChunk8))
                {
                    DrawModel(atreyu, atreyuPos, 2.8f, WHITE);
                }
                //atreyu's canoe
                if (IsPointInFrustum(canoePos, frustumChunk8))
                {
                    DrawModel(canoe, canoePos, 9.0f, WHITE);
                }
                //wrench if don does not have it yet, in the barn
                if (!don.hasWrench && IsPointInFrustum(wrenchPos, frustumChunk8))
                {
                    DrawModel(wrenchModel, wrenchPos, 1.2f, WHITE);
                }
                //npcs
                if (onLoad)
                {
                    for (int i = 0; i < NPC_TOTAL; i++)
                    {
                        if (Vector3DistanceSqr(don.pos, npcs[i].pos) > 600.0f*600) { continue; } //todo: add frustum culling here also
                        NPC_Draw(&npcs[i]);
                    }
                }
            }
            //fireplaces
            if (onLoad)
            {
                for (int i = 0; i < FIREPIT_TOTAL_COUNT; i++)
                {
                    if (!dropped_firepits)
                    {
                        fires[i].pos.y = GetTerrainHeightFromMeshXZ(fires[i].pos.x, fires[i].pos.z) + 0.8f;//+offset
                    }
                    if (Vector3DistanceSqr(don.pos, fires[i].pos) > 800*900) { continue; }
                    DrawModel(firepit,fires[i].pos, 3, WHITE);
                    if (fires[i].lit)//if its lit, draw flame
                    {
                        BeginBlendMode(BLEND_ADDITIVE);

                        Vector3 P = fires[i].pos;
                        P.y += 0.35f; // small lift if needed

                        // Core (hot, narrow)
                        {
                            float v = 0.00f;
                            SetShaderValue(fireShader, fireVariantLoc, &v, SHADER_UNIFORM_FLOAT);
                            DrawModelEx(fireModel, P, (Vector3) { 0, 1, 0 }, 0.0f,
                                (Vector3) {
                                0.8f, 0.9f, 0.8f
                            }, WHITE);
                        }

                        // Left/Right tongue (different phase & taller)
                        {
                            float v = 1.37f;
                            SetShaderValue(fireShader, fireVariantLoc, &v, SHADER_UNIFORM_FLOAT);
                            DrawModelEx(fireModel, P, (Vector3) { 0, 1, 0 }, 0.0f,
                                (Vector3) {
                                0.65f, 1.4f, 0.65f
                            }, WHITE);
                        }

                        // Outer soft body (wider, shorter, redder via rampShift)
                        {
                            float v = 2.73f;
                            SetShaderValue(fireShader, fireVariantLoc, &v, SHADER_UNIFORM_FLOAT);
                            DrawModelEx(fireModel, P, (Vector3) { 0, 1, 0 }, 0.0f,
                                (Vector3) {
                                1.5f, 1.1f, 1.5f
                            }, WHITE);
                        }
                        EndBlendMode();
                    }
                }
                dropped_firepits = true;
            }
            //machines
            Machine_DrawAll(camera.position, frustum);
            if (Vector3DistanceSqr(don.pos,Scenes[SCENE_HOME_CABIN_01].pos) < 300*300) { Machine_DrawTruckLift(); }
            
            //items and maps
            if (onLoad) 
            { 
                DrawItems(displayBoxes); 
                DrawUncollectedMaps(displayBoxes);//todo: culling
            }
            //homes
            if (onLoad)
            {
                Vector3 doorPos = Vector3Lerp(cottageDoorClosed, cottageDoorOpenPos, cottageDoorSlide);
                if (Vector3DistanceSqr(doorPos,don.pos)<100*101) // cottage sliding door draw
                {
                    DrawCube(doorPos, 16.0f, 16.0f, 2.5f, BROWN);
                    if (displayBoxes)
                    {
                        DrawBoundingBox(cottageDoorBox, RED);
                    }
                }
                WaterWheel_Draw(&don);
                int milCnt = 0;
                rlDisableBackfaceCulling();
                for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
                {
                    if (!IsBoxInFrustum(Scenes[i].box, frustumChunk8)) { continue; }
                    DrawModelEx(HomeModels[Scenes[i].modelType], Scenes[i].pos, 
                        (Vector3) { 0, 1, 0 }, Scenes[i].yaw * RAD2DEG,
                        (Vector3) { Scenes[i].scale , Scenes[i].scale, Scenes[i].scale}, 
                        WHITE);
                    if (Scenes[i].modelType == MODEL_HOME_WINDMILL)
                    {
                        if (Scenes[i].active) { milCnt++; }
                        // Inputs we already have
                        float yaw = Scenes[i].yaw;                 // radians (Scene uses radians)
                        float spin = Scenes[i].active ? rotorSpin * DEG2RAD : 0;           // keep your existing rotorSpin but use radians
                        Vector3 pos = Scenes[i].pos;                 // home world position

                        // --- Build transforms (no quaternions) ---
                        Matrix Srot = MatrixScale(16.0f, 16.0f, 16.0f);        // rotor mesh scale (kept same as before)
                        Matrix Rspin = MatrixRotateZ(spin);                    // rotor roll about its local +Z
                        Matrix Toff = MatrixTranslate(rotorOffset.x, rotorOffset.y, rotorOffset.z); // hub offset in home-local space
                        Matrix Ryaw = MatrixRotateY(yaw);                     // home yaw
                        Matrix Thome = MatrixTranslate(pos.x, pos.y, pos.z);   // home world translation

                        // Order:  Scale · (Spin · Offset) · (HomeYaw · HomeTranslate)
                        Matrix rotorM = MatrixMultiply(
                            MatrixMultiply(MatrixMultiply(Srot, Rspin), Toff),
                            MatrixMultiply(Ryaw, Thome));

                        // Draw rotor with explicit matrix
                        DrawMesh(rotor.meshes[0], rotor.materials[0], rotorM);
                    }

                    if (displayBoxes) { DrawBoundingBox(Scenes[i].box, PURPLE); }
                }
                rlEnableBackfaceCulling();
                if (!missions[MISSION_START_ALL_MILLS].complete && milCnt >= MACHINE_COUNT_WINDMILL) //number of mills
                {
                    missions[MISSION_START_ALL_MILLS].complete = true;
                    toast = "All Windmills Activated!";
                    don.xp += 500;
                    don.money += 500;
                    StartTimer(&toastTimer);
                }
            }
            //plats
            if (onLoad)
            {
                for (int i = 0; i < NUM_PLATS; i++)
                {
                    if (Vector3DistanceSqr(don.pos,plats[i].pos) > 600*600 || !IsBoxInFrustum(plats[i].box, frustum)) { continue; }
                    Platform_Draw(&plats[i], displayBoxes);
                }
            }
            //bg
            if (onLoad && !caveMode)
            {
                for (int i = 0; i < act_bg_count; i++)
                {
                    int b = act_bg[i];
                    if (!bg[b].active) { continue; }
                    if (!IsBoxInFrustum(bg[b].box, frustum)) { continue; }
                    rlDisableBackfaceCulling();
                    DrawBadGuy(&bg[b]);
                    rlEnableBackfaceCulling();
                    if (displayBoxes) { DrawBoundingBox(bg[b].box, PURPLE); }
                }
            }
            //whales and fish
            if (onLoad && !caveMode)
            {
                //shark
                Shark_Draw(&shark, &don);
                //whales
                for (int i = 0; i < numWhales; i++)
                {
                    if (!IsPointInFrustum(whales[i].pos, frustum)) { continue; }
                    FSM_Tick(&whales[i], (float)GetTime(), GetFrameTime());
                    Quaternion qFinal = BuildWorldQuat(&whales[i]);
                    Matrix R = QuaternionToMatrix(qFinal);
                    Matrix T = MatrixTranslate(whales[i].pos.x, whales[i].pos.y, whales[i].pos.z);
                    Matrix S = MatrixScale(10, 10, 10);
                    Matrix whaleXform = MatrixMultiply(S, MatrixMultiply(R,T));
                    DrawMesh(whales[i].model.meshes[0], whales[i].model.materials[0], whaleXform);
                    if (displayBoxes) { DrawBoundingBox(whales[i].box, BLUE); }
                    //DrawSphere(whales[i].pos, 4.0f, RED);
                }
                //fish
                // //fish movmemnt
                // === FISH UPDATE + DRAW ======================================================
                for (int s = 0; s < numSchools; s++)
                {
                    if (Vector3DistanceSqr(donnyMode?don.pos:camera.position, fish[s].fishTarget) > 890*900) { continue; }//good culling on fish because they are expensive
                    // 1) steer the school target a bit each frame (orbit + optional player nudge)
                    static float schoolTheta = 0.0f;
                    float dt = GetFrameTime();
                    schoolTheta += dt * 0.4f; // slow orbit
                    fish[s].fishTarget.x = fish[s].center.x + sinf(schoolTheta) * (fish[s].schoolRadius * 0.6f);
                    fish[s].fishTarget.z = fish[s].center.z + cosf(schoolTheta) * (fish[s].schoolRadius * 0.6f);

                    // 2) boids-lite update (moves & turns each fish)
                    UpdateSchool(fish[s].fish, fish[s].schoolCount, fish[s].fishTarget, dt);

                    // 3) build per-instance transforms for this school
                    static Matrix* schoolMatrices = NULL;
                    static int maxSchoolCount = 0;
                    if (schoolCount > maxSchoolCount) {
                        if (schoolMatrices) { MemFree(schoolMatrices); }
                        schoolMatrices = MemAlloc(sizeof(Matrix) * schoolCount);
                        maxSchoolCount = schoolCount;
                    }
                    int localSchoolCount = 0;
                    for (int i = 0; i < schoolCount; i++) {
                        Fish* f = &fish[s].fish[i];
                        if (!IsPointInFrustum(f->pos, frustumChunk8)) { continue; }
                        Matrix rot = MatrixRotateY(DEG2RAD * f->yawDeg);
                        Matrix sca = MatrixScale(f->scale, f->scale, f->scale);
                        Matrix tra = MatrixTranslate(f->pos.x, f->pos.y, f->pos.z);
                        schoolMatrices[i] = MatrixMultiply(MatrixMultiply(sca, rot), tra);
                        localSchoolCount++;
                    }

                    // 4) draw entire school in one GPU instancing call
                    DrawMeshInstanced(
                        fishModel.meshes[0],
                        fishModel.materials[0],
                        schoolMatrices,
                        localSchoolCount
                    );
                    //if (schoolMatrices) { MemFree(schoolMatrices); } //this fails for me, not sure what to do with it...memory leak? todo
                }
                // ============================================================================
            }
            if (onLoad && !caveMode)
            {
                DrawMesh(truck.meshes[0], truckMaterial, rotationTruck);
                for (int i = 0; i < 4; i++)
                {
                    float tireAngleQ = -(tireTurnPos[i]);//fabsf//
                    float tireAngleDelta = 0.0f;//float tireAngleDelta = tireAngleQ;  // Default for rear tires
                    // Compute tire-specific spin and steering
                    float steerAngle = 0.0f;
                    if (i < 2 && vehicleMode) {
                        // Front tires only — steer left/right
                        steerAngle = PI / 8.0f * gpad.normLX; // tweak max angle
                    }
                    //if (hoverMode)
                    //{
                    //    truckPitch /= 2;
                    //    //truckPitch = Lerp(truckPitch, 0.0f, dt * 4.0f);
                    //    //truckRoll = Lerp(truckRoll, 0.0f, dt * 4.0f);
                    //}
                    // First apply spin around X (wheel axis), then steering around Y
                    // Step 1: Create rotation matrices for yaw (Y), pitch (X), and roll (Z)
                    //printf("steerAngle : %f\n",steerAngle);
                    Matrix yawMatrix = MatrixRotateY((hoverMode?0: truckAngle - steerAngle));     // Turn left/right
                    //Matrix yawMatrix   = MatrixRotateY(tireAngleDelta);
                    float tireRotAngle = hoverMode ? 90.0f : 0;
                    Matrix pitchMatrix = MatrixRotateX(-tireSpinPos[i]);   // Tilt forward/back //sinf(truckAngle)
                    Matrix rollMatrix = MatrixRotateZ(tireRotAngle);    // Lean left/right
                    //truckTireOffsetMatrix
                    Vector3 tireSpace = RotateY(RotateX(RotateZ(tireOffsets[i], truckRoll + truckTrickRoll), truckPitch - truckTrickPitch), -truckAngle - truckTrickYaw);
                    /*if (hoverMode)
                    {
                        tireSpace = RotateY(RotateX(RotateZ(tireOffsets[i], 0.0f),0.0f),-truckAngle - truckTrickYaw);
                    }*/
                    // Step 2: Combine them in the proper order:
                    // Yaw → Pitch → Roll (you can change order depending on your feel/needs)
                    Matrix rotation = MatrixMultiply(pitchMatrix, MatrixMultiply(yawMatrix, rollMatrix));//neo where are you!
                    // Step 3: Apply position translation
                    rotation.m12 = truckOrigin.x + tireSpace.x;
                    rotation.m13 = truckOrigin.y + tireSpace.y - tireYOffset[i]; //!!!!SPACE TIRES!!!!
                    rotation.m14 = truckOrigin.z + tireSpace.z;
                    Vector3 tireWorldPos = { rotation.m12, rotation.m13, rotation.m14 };
                    DrawMesh(tire.meshes[0], tireMaterial, rotation);
                    if (hoverMode)
                    {
                        Vector3 flamePos = tireWorldPos;
                        flamePos.y -= 0.8f;

                        DrawHoverFlameShadered(
                            fireModel,
                            fireShader,
                            fireVariantLoc,
                            flamePos,
                            (float)GetTime() + i * 1.37f
                        );
                    }
                }
                if (displayBoxes)
                {
                    DrawBoundingBox(TruckBoxFront, BLUE);
                    DrawBoundingBox(TruckBoxBack, BLUE);
                    DrawBoundingBox(TruckBoxLeft, BLUE);
                    DrawBoundingBox(TruckBoxRight,BLUE);
                    DrawBoundingBox(TruckBoxTires[0], RED);
                    DrawBoundingBox(TruckBoxTires[1], RED);
                    DrawBoundingBox(TruckBoxTires[2], RED);
                    DrawBoundingBox(TruckBoxTires[3], RED);
                }
            }
            //lightning bugs &&&&&&&&&
            if(!dayTime && !caveMode)
            {
                if(onLoad) //fire flies
                {
                    UpdateLightningBugs(bugs, BUG_COUNT, dt * 0.0073f);//I think this is wrong, but it works out better this way
                    int bugsAdded = 0;
                    int starsAdded = 0;
                    //- loop through all of the static props that are int he active active tile zone
                    Matrix transforms[BUG_COUNT] = {0};
                    float blinkValues[BUG_COUNT] = {0};
                    for (int i = 0; i < BUG_COUNT; i++)
                    {
                        if(!IsBoxInFrustum(bugs[i].box , frustumChunk8)){continue;}
                        //first update the bugs positions
                        blinkValues[bugsAdded] = bugs[i].alpha;
                        //get ready to draw
                        Vector3 _p = bugs[i].pos;
                        Matrix translation = MatrixTranslate(_p.x, _p.y, _p.z);
                        Vector3 toCamera = Vector3Subtract(camera.position, bugs[i].pos);
                        toCamera.y = 0; // Optional: lock to horizontal billboard
                        toCamera = Vector3Normalize(toCamera);
                        Vector3 axis = (Vector3){0,1,0};//Vector3Normalize((Vector3){ (float)GetRandomValue(0, 360), (float)GetRandomValue(0, 360), (float)GetRandomValue(0, 360) });
                        float angle = -bugs[i].angle+PI/2.8f;//float angle = 0.0f;//(float)GetRandomValue(0, 180)*DEG2RAD;
                        Matrix rotation = MatrixRotate(axis, angle);
                        transforms[bugsAdded] = MatrixMultiply(rotation, translation);//todo: add rotations and such
                        bugsAdded++;
                    }   
                    // Before drawing:
                    int blinkAttribLoc = GetShaderLocationAttrib(lightningBugShader, "instanceBlink");
                    SetShaderValueV(lightningBugShader, blinkAttribLoc, blinkValues, SHADER_ATTRIB_FLOAT, bugsAdded);
                    float time = GetTime(); // Raylib built-in
                    int timeLoc = GetShaderLocation(lightningBugShader, "u_time");
                    SetShaderValue(lightningBugShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
                    DrawMeshInstanced(
                            sphereMesh, 
                            sphereMaterial, 
                            transforms, 
                            bugsAdded
                    );//windows
                    //stars ** ** ** ** ** **** ** ** ** ** **** ** ** ** ** **** ** ** ** ** **** ** ** ** ** **
                    Matrix starTransforms[STAR_COUNT] = {0};
                    float starBlinkValues[STAR_COUNT] = {0};
                    //UpdateStars(stars,STAR_COUNT);
                    for (int i = 0; i < STAR_COUNT; i++)
                    {
                        //first update the bugs positions
                        starBlinkValues[starsAdded] = stars[i].alpha;
                        // //get ready to draw
                        Vector3 __p = stars[i].pos;
                        // Matrix translation = MatrixTranslate();
                        // starTransforms[starsAdded] = translation;//MatrixMultiply(rotation, translation);//todo: add rotations and such
                        
                        Matrix mat = MatrixTranslate(__p.x, __p.y, __p.z);
                        mat.m15 = (float)i;  // Encode instanceId into the matrix (row 3, column 1)
                        starTransforms[i] = mat;
                        starsAdded++;
                    }   
                    // Before drawing:
                    int blinkStarAttribLoc = GetShaderLocationAttrib(starShader, "instanceBlink");
                    SetShaderValueV(starShader, blinkStarAttribLoc, starBlinkValues, SHADER_ATTRIB_FLOAT, starsAdded);
                    float timeStar = GetTime(); // Raylib built-in
                    int timeStarLoc = GetShaderLocation(starShader, "u_time");
                    SetShaderValue(starShader, timeStarLoc, &timeStar, SHADER_UNIFORM_FLOAT);
                    DrawMeshInstanced(
                            sphereStarMesh, 
                            sphereStarMaterial, 
                            starTransforms, 
                            starsAdded
                    );//windows
                    //** ** ** ** ** **** ** ** ** ** **** ** ** ** ** **** ** ** ** ** **** ** ** ** ** **
                }
            }
            if (!caveMode)
            {
                //TraceLog(LOG_INFO, "-------TILES DRAWING-----------");
                for (int te = 0; te < foundTileCount; te++)
                {
                    if (!wasTilesDocumented) { break; }
                    if (foundTiles[te].state != TS_IN_GPU) { continue; }
                    //TraceLog(LOG_INFO, "TEST - Maybe - Drawing tile model: chunk %02d_%02d, tile %02d_%02d", foundTiles[te].cx, foundTiles[te].cy, foundTiles[te].tx, foundTiles[te].ty);
                    if (chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_64 //this one first because its quick, although it might get removed later
                        && (!IsTileActive(foundTiles[te].cx, foundTiles[te].cy, foundTiles[te].tx, foundTiles[te].ty, gx, gy) || USE_TILES_ONLY)
                        && IsBoxInFrustum(foundTiles[te].box, frustumChunk8))
                    {
                        BeginShaderMode(foundTiles[te].model.materials[0].shader);
                        SetMaterialTexture(&foundTiles[te].model.materials[0], MATERIAL_MAP_DIFFUSE, foundTiles[te].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture);//added this because I was having tiles draw with the wrong texture
                        if (reportOn) { tileBcCount++; tileTriCount += foundTiles[te].model.meshes[0].triangleCount; };
                        DrawModel(foundTiles[te].model, (Vector3) { 0, 0, 0 }, 1.0f, lightTileColor);
                        if (displayBoxes) { DrawBoundingBox(foundTiles[te].box, RED); }
                        EndShaderMode();
                    }
                }
                //TraceLog(LOG_INFO, "-------END TILES DRAWING END-----------");
                for (int cy = 0; cy < CHUNK_COUNT; cy++) {
                    for (int cx = 0; cx < CHUNK_COUNT; cx++) {
                        if (chunks[cx][cy].isLoaded)
                        {
                            loadCnt++;
                            //TraceLog(LOG_INFO, "drawing chunk: %d,%d", cx, cy);
                            if (chunks[cx][cy].lod == LOD_64)
                            {
                                chunkBcCount++;
                                chunkTriCount += chunks[cx][cy].model.meshes[0].triangleCount;
                                Matrix mvp = MatrixMultiply(proj, MatrixMultiply(view, chunks[cx][cy].model.transform));
                                SetShaderValueMatrix(heightShaderLight, mvpLocLight, mvp);
                                //SetShaderValueMatrix(heightShaderLight, modelLocLight, MatrixIdentity());
                                Matrix chunkModelMatrix = MatrixTranslate(chunks[cx][cy].position.x, chunks[cx][cy].position.y, chunks[cx][cy].position.z);
                                SetShaderValueMatrix(heightShaderLight, modelLocLight, chunkModelMatrix);
                                Vector3 camPos = camera.position;
                                SetShaderValue(heightShaderLight, GetShaderLocation(heightShaderLight, "cameraPosition"), &camPos, SHADER_UNIFORM_VEC3);
                                BeginShaderMode(heightShaderLight);
                                DrawModel(chunks[cx][cy].model, chunks[cx][cy].position, MAP_SCALE, WHITE);
                                EndShaderMode();
                                if (onLoad)//only once we have fully loaded everything
                                {
                                    if (USE_GPU_INSTANCING) //GPU INSTANCING FOR CLOSE STATIC PROPS
                                    {
                                        int counter[MODEL_TOTAL_COUNT] = { 0,0 };
                                        //- loop through all of the static props that are in the active active tile zone
                                        for (int pInd = 0; pInd < chunks[cx][cy].treeCount; pInd++)
                                        {
                                            //culling
                                            if ((!IsTreeInActiveTile(chunks[cx][cy].props[pInd].pos, gx, gy) || USE_TILES_ONLY)
                                                || !IsBoxInFrustum(chunks[cx][cy].props[pInd].outerBox, frustum)) {
                                                continue;
                                            }
                                            //get ready to draw
                                            StaticGameObject* obj = &chunks[cx][cy].props[pInd];
                                            Matrix scaleMatrix = MatrixScale(obj->scale, obj->scale, obj->scale);
                                            Matrix pitchMatrix = MatrixRotateX(obj->pitch);
                                            Matrix yawMatrix = MatrixRotateY(obj->yaw);
                                            Matrix rollMatrix = MatrixRotateZ(obj->roll);
                                            Matrix rotationMatrix = MatrixMultiply(MatrixMultiply(pitchMatrix, yawMatrix), rollMatrix);
                                            Matrix transform = MatrixMultiply(scaleMatrix, rotationMatrix);
                                            transform = MatrixMultiply(transform, MatrixTranslate(obj->pos.x, obj->pos.y, obj->pos.z));
                                            HighFiTransforms[chunks[cx][cy].props[pInd].type][counter[chunks[cx][cy].props[pInd].type]] = transform;//well this is kind of insane
                                            counter[chunks[cx][cy].props[pInd].type]++;
                                            if (displayBoxes)
                                            {
                                                DrawBoundingBox(chunks[cx][cy].props[pInd].outerBox, BLUE);
                                                DrawBoundingBox(chunks[cx][cy].props[pInd].box, PINK);
                                            }
                                            if (reportOn) { treeTriCount += HighFiStaticObjectModels[chunks[cx][cy].props[pInd].type].meshes[0].triangleCount; }
                                        }
                                        //draw
                                        for (int mt = 0; mt < MODEL_TOTAL_COUNT; mt++)
                                        {
                                            BeginShaderMode(HighFiStaticObjectMaterials[mt].shader);
                                            //TraceLog(LOG_INFO, "Model %s Texture ID: %d", GetModelName(mt), HighFiStaticObjectModels[mt].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id);
                                            treeBcCount++;
                                            DrawMeshInstanced(
                                                HighFiStaticObjectModels[mt].meshes[0],
                                                HighFiStaticObjectMaterials[mt],
                                                HighFiTransforms[mt],
                                                counter[mt]
                                            );//windows
                                            EndShaderMode();
                                        }
                                    }
                                }
                            }
                            else if (chunks[cx][cy].lod == LOD_32 && IsBoxInFrustum(chunks[cx][cy].box, frustumChunk8)) {
                                chunkBcCount++;
                                chunkTriCount += chunks[cx][cy].model32.meshes[0].triangleCount;
                                Matrix mvp = MatrixMultiply(proj, MatrixMultiply(view, chunks[cx][cy].model.transform));
                                SetShaderValueMatrix(heightShaderLight, mvpLocLight, mvp);
                                BeginShaderMode(heightShaderLight);
                                DrawModel(chunks[cx][cy].model32, chunks[cx][cy].position, MAP_SCALE, displayLod ? BLUE : WHITE);
                                EndShaderMode();
                            }
                            else if (chunks[cx][cy].lod == LOD_16 && IsBoxInFrustum(chunks[cx][cy].box, frustumChunk8) && cx != 0 && cx != 15 && cy != 0 && cy != 15) {
                                chunkBcCount++;
                                chunkTriCount += chunks[cx][cy].model16.meshes[0].triangleCount;
                                DrawModel(chunks[cx][cy].model16, chunks[cx][cy].position, MAP_SCALE, displayLod ? PURPLE : chunk_16_color);
                            }
                            else if ((IsBoxInFrustum(chunks[cx][cy].box, frustumChunk8) || !onLoad) && cx != 0 && cx != 15 && cy != 0 && cy != 15) {
                                chunkBcCount++;
                                chunkTriCount += chunks[cx][cy].model8.meshes[0].triangleCount;
                                DrawModel(chunks[cx][cy].model8, chunks[cx][cy].position, MAP_SCALE, displayLod ? RED : chunk_08_color);
                            }
                            if (displayBoxes) { DrawBoundingBox(chunks[cx][cy].box, YELLOW); }
                        }
                        else { loadedEem = false; }
                    }
                }
                if (onLoad) //handle water last because of its transparency, only draw once loaded
                {
                    //corn
                    DrawCornFields(don.pos, frustumChunk8, displayBoxes);
                    //water
                    for (int cy = 0; cy < CHUNK_COUNT; cy++) {
                        for (int cx = 0; cx < CHUNK_COUNT; cx++) {
                            if (chunks[cx][cy].isLoaded)
                            {
                                //handle water last
                                for (int w = 0; w < chunks[cx][cy].waterCount; w++)
                                {
                                    if (chunks[cx][cy].lod != LOD_64 && !IsBoxInFrustum(chunks[cx][cy].water[w].box, frustumChunk8)) { continue; }
                                    glEnable(GL_POLYGON_OFFSET_FILL);
                                    glPolygonOffset(-1.0f, -1.0f); // Push water slightly forward in Z-buffer
                                    rlDisableBackfaceCulling();
                                    Vector3 cameraPos = camera.position;
                                    Vector3 waterPos = { 0, WATER_Y_OFFSET, 0 };
                                    // Get direction from patch to camera
                                    Vector3 toCamera = Vector3Subtract(waterPos, cameraPos);
                                    // Scale it down to something subtle, like 5%
                                    Vector3 shift = Vector3Scale(toCamera, 0.05f);
                                    // Final draw position is nudged toward the player
                                    Vector3 drawPos = Vector3Add(waterPos, shift);
                                    Vector2 offset = (Vector2){ w * cx, w * cy };
                                    SetShaderValue(waterShader, offsetLoc, &offset, SHADER_UNIFORM_VEC2);
                                    BeginShaderMode(waterShader);
                                    DrawModel(chunks[cx][cy].water[w].model, drawPos, 1.0f, (Color) { 0, 100, 253, 232 });
                                    EndShaderMode();
                                    rlEnableBackfaceCulling();
                                    glDisable(GL_POLYGON_OFFSET_FILL);
                                    if (displayBoxes) { DrawBoundingBox(chunks[cx][cy].water[w].box, VIOLET); }
                                }
                            }
                        }
                    }
                }
            }
            //rlEnableBackfaceCulling();
            if(reportOn) //triangle report
            {
                totalBcCount = tileBcCount + chunkBcCount + treeBcCount;
                totalTriCount = tileTriCount + chunkTriCount + treeTriCount;
                printf("Estimated tile triangles this frame  :  %d\n", tileTriCount);
                printf("Estimated batch calls for tiles      :  %d\n", tileBcCount);
                printf("Estimated tree triangles this frame  :  %d\n", treeTriCount);
                printf("Estimated batch calls for trees      :  %d\n", treeBcCount);
                printf("Estimated chunk triangles this frame :  %d\n", chunkTriCount);
                printf("Estimated batch calls for chunks     :  %d\n", chunkBcCount);
                printf("Estimated TOTAL triangles this frame :  %d\n", totalTriCount);
                printf("Estimated TOTAL batch calls          :  %d\n", totalBcCount);
                printf("Current FPS (so you can document)    :  %d\n", GetFPS());
            }
            if (onLoad)
            {
                for (int i = 0; i < numCloseProps; ++i) {
                    StaticGameObject* g = CloseProps[i];
                    if (g->hasBerries && !g->berriesSpawned) {
                        SpawnBerriesForProp(g);
                    }
                    DrawBerriesForProp(g);
                }
                DrawApples(&don);
                //shark
                Shark_Draw_Extremities(&shark, &don);//trasnparencies in blood
            }
            //DrawGrid(256, 1.0f);
        EndMode3D();
        if (devDisplay)
        {
            //DrawText("WASD to move, mouse to look", 10, 10, 20, BLACK);
            DrawText(TextFormat("Pitch: %.2f  Yaw: %.2f", pitch, yaw), 10, 30, 20, BLACK);
            DrawText(TextFormat("Next Chunk: (%d,%d)", chosenX, chosenY), 10, 50, 20, BLACK);
            DrawText(TextFormat("Current Chunk: (%d,%d), Tile: (%d,%d), Global Tile: (%d,%d)", closestCX, closestCY, playerTileX, playerTileY, gx, gy), 10, 70, 20, BLACK);
            Vector3 disPositionRightHere = donnyMode ? don.pos : camera.position;
            DrawText(TextFormat("X: %.2f  Y: %.2f Z: %.2f", disPositionRightHere.x, disPositionRightHere.y, disPositionRightHere.z), 10, 90, 20, BLACK);
            DrawText(TextFormat("Search Type: %s (%d) [t=toggle,r=search]", GetModelName(modelSearchType), modelSearchType), 10, 110, 20, BLACK);
            if (vehicleMode && onLoad)
            {
                DrawText(TextFormat("Tuck Speed (MPH): %.2f", truckSpeed * 60), 10, 150, 20, BLUE);
                DrawText(TextFormat("Tuck Angle (Rad): %.2f", truckAngle), 10, 170, 20, PURPLE);
                DrawText(TextFormat("Tuck Pitch (Rad): %.2f", truckPitch), 10, 190, 20, PURPLE);
                DrawText(TextFormat("Tuck Roll  (Rad): %.2f", truckRoll), 10, 210, 20, PURPLE);
                DrawText(TextFormat("Points: %d", points), 10, 230, 16, BLACK);
                DrawText(TextFormat("Truck Air State = %d", truckAirState), 10, 250, 16, BLACK);//
                DrawText(TextFormat("F=[%.3f][%.3f]", tireYOffset[0], tireYOffset[1]), 10, 270, 16, GRAY);
                DrawText(TextFormat("B=[%.3f][%.3f]", tireYOffset[2], tireYOffset[3]), 10, 290, 16, GRAY);
            }
            if (donnyMode && onLoad)
            {
                DrawText(TextFormat("%d", don.state), 10, 150, 20, BLUE);
                DrawText(TextFormat("Normal: %.2f %.2f %.2f", don.groundNormal.x, don.groundNormal.y, don.groundNormal.z), 10, 170, 20, PURPLE);
                DrawText(TextFormat("GroundY: %.2f", don.groundY), 10, 190, 20, PURPLE);
            }
        }
        if (donnyMode && onLoad)
        {
            if (don.bowMode && (don.state == DONOGAN_STATE_BOW_PULL || don.state == DONOGAN_STATE_BOW_AIM || don.state == DONOGAN_STATE_BOW_REL))
            {
                Vector2 center = { sw * 0.5f, sh * 0.5f };
                DrawCircleLines((int)center.x, (int)center.y, 10, WHITE);
                DrawLine((int)center.x - 12, (int)center.y, (int)center.x + 12, (int)center.y, WHITE);
                DrawLine((int)center.x, (int)center.y - 12, (int)center.x, (int)center.y + 12, WHITE);
            }
        }
        if (onLoad)
        {
            if (!HasTimerElapsed(&toastTimer))
            {
                DrawText(toast, 24, sh - 60, 16, YELLOW);
            }
            else if (truckSummonActive) { DrawText("SUMMONING...", 24, sh - 60, 16, YELLOW); }
        }
        if (showMap) {
            // Map drawing area (scaled by zoom)
            //
            Rectangle dest = {
                sw - (GAME_MAP_SIZE * mapZoom) - 10, //just calculate this x value every time
                mapViewport.y,
                mapViewport.width * mapZoom,
                mapViewport.height * mapZoom
            };
            DrawTexturePro(mapTexture,
                (Rectangle){ 0, 0, mapTexture.width, mapTexture.height },
                dest,
                (Vector2){ 0, 0 },
                0.0f,
                WHITE);

            // Player marker (assume position normalized to map range)
            float normalizedX = (camera.position.x + (MAX_WORLD_SIZE/2)) / WORLD_WIDTH;
            float normalizedY = (camera.position.z + (MAX_WORLD_SIZE/2)) / WORLD_HEIGHT;
            float normalizedTruckX = (truckPosition.x + (MAX_WORLD_SIZE / 2)) / WORLD_WIDTH;
            float normalizedTruckY = (truckPosition.z + (MAX_WORLD_SIZE / 2)) / WORLD_HEIGHT;
            float normalizedSharkX = (shark.pos.x + (MAX_WORLD_SIZE / 2)) / WORLD_WIDTH;
            float normalizedSharkY = (shark.pos.z + (MAX_WORLD_SIZE / 2)) / WORLD_HEIGHT;
            //maps
            DrawDisplayMaps(dest, &don, truckPosition, whales, numWhales);
            //truck marker
            if (!vehicleMode)
            {
                Vector2 t_marker = {
                dest.x + normalizedTruckX * dest.width,
                dest.y + normalizedTruckY * dest.height
                };
                DrawCircleV(t_marker, 3, BLUE);
            }
            //main marker
            Vector2 marker = {
                dest.x + normalizedX * dest.width,
                dest.y + normalizedY * dest.height
            };
            DrawCircleV(marker, 3, RED);
            //shark
            if (don.inWater)
            {
                Vector2 markerShark = {
                dest.x + normalizedSharkX * dest.width,
                dest.y + normalizedSharkY * dest.height
                };
                DrawCircleV(markerShark, 3, PURPLE);
            }
            // Facing triangle (yellow "nose" pointing the player's yaw)
            float local_yaw = donnyMode ? +(yaw + (2.0f * (PI / 2.0f))) : vehicleMode ? truckAngle : -(yaw + (PI / 2.0f)); //
            float local_x_sign = vehicleMode|| donnyMode ? 1.0f : -1.0f;
            Vector2 dir = (Vector2){ (local_x_sign)*sinf(local_yaw), (local_x_sign)*cosf(local_yaw) }; // yaw in radians
            // Tiny arrow just outside the 3px circle
            float tipLen = 10.0f;   // pixels from center to tip
            float baseAlong = 4.5f;    // how far the base sits from center
            float halfWidth = 3.5f;    // half the triangle base width
            Vector2 tip = (Vector2){ marker.x + dir.x * tipLen,     marker.y + dir.y * tipLen };
            Vector2 baseCenter = (Vector2){ marker.x + dir.x * baseAlong,  marker.y + dir.y * baseAlong };
            // Perpendicular for triangle base
            Vector2 perp = (Vector2){ -dir.y, dir.x };
            Vector2 left = (Vector2){ baseCenter.x + perp.x * halfWidth, baseCenter.y + perp.y * halfWidth };
            Vector2 right = (Vector2){ baseCenter.x - perp.x * halfWidth, baseCenter.y - perp.y * halfWidth };
            DrawTriangle(left, right, marker, YELLOW);
            // Optional crisp outline
            // DrawTriangleLines(left, right, tip, BLACK);
            //health bars
            DrawRectangleLines(sw - (don.maxHealth + 10) - 10, 160, don.maxHealth+4, 10, BLACK);
            DrawRectangle(sw - (don.maxHealth + 10) - 8, 162, don.health, 6, DARKGREEN);
            DrawRectangleLines(sw - (don.maxMana + 10) - 10, 180, don.maxMana+4, 10, BLACK);
            DrawRectangle(sw - (don.maxMana + 10) - 8, 182, don.mana, 6, BLUE);
            if (!devDisplay && onLoad)
            {
                if (donnyMode)
                {
                    DrawText(TextFormat("XP: %d", don.xp), 10, 30, 20, RAYWHITE);
                    DrawText(TextFormat("LEVEL: %d", don.level), 10, 50, 20, RAYWHITE);
                    DrawText(TextFormat("$%.2f", don.money), 10, 70, 20, RAYWHITE);
                }
                else if (vehicleMode)
                {
                    DrawText(TextFormat("POINTS: %d", points), 10, 30, 20, RAYWHITE);
                    if (truckCruise) { DrawText(TextFormat("cruise on", points), 10, 50, 20, RAYWHITE); }
                }
            }
        }
        if (onLoad)
        {
            Menu_DrawOverlay(&gGame, &don);
        }
        if (donnyMode && don.isTalking)
        {
            Rectangle box = {
                40,
                sh - 270,
                sw - 80,
                170
            };

            DrawRectangle(box.x - 4, box.y - 4, box.width + 8, box.height + 8, BLACK);
            DrawRectangle(box.x, box.y, box.width, box.height, RAYWHITE);

            Texture2D talkee = tol_head;

            if (don.who == TALK_TYPE_ATREYU || don.who == TALK_TYPE_ATREYU_BOW) {
                talkee = atreyu_head;
            }
            else if (don.who == TALK_TYPE_DARREL || don.who == TALK_TYPE_NICK) {
                talkee = darrel_head;
            }
            else if (don.who == TALK_TYPE_LUCY_ONE || don.who == TALK_TYPE_LUCY_TWO) {
                talkee = lucy_head;
            }
            else if (don.who == TALK_TYPE_WIZARD) {
                talkee = wiz_head;
            }
            else if (don.who == TALK_TYPE_ABBY || don.who == TALK_TYPE_ABBY_2) {
                talkee = abby_head;
            }
            else if (don.who == TALK_TYPE_STORE) {
                talkee = clerk_head;
            }
            else if (don.who == TALK_TYPE_GAL_1 || don.who == TALK_TYPE_GAL_2 || don.who == TALK_TYPE_GAL_3) {
                talkee = gal_head;
            }
            else if (don.who == TALK_TYPE_ROGER) {
                talkee = roger_head;
            }
            else if (don.who == TALK_TYPE_GEOFF) {
                talkee = geoff_head;
            }
            else if (don.who == TALK_TYPE_MARY) {
                talkee = mary_head;
            }

            Rectangle src = { 0, 0, talkee.width, talkee.height };
            Rectangle dest = { box.x + box.width - 74, box.y + 10, 64, 64 };
            DrawTexturePro(talkee, src, dest, (Vector2) { 0, 0 }, 0.0f, WHITE);

            DrawText(Talk_GetSpeaker(), box.x + 16, box.y + 12, 28, BLACK);

            DrawTextBoxed(
                req_font,
                Talk_GetLine(),
                (Rectangle) {
                box.x + 16, box.y + 52, box.width - 110, 78
            },
                28.0f,
                2.0f,
                true,
                BLACK
            );

            DrawText("X: OK", box.x + 16, box.y + box.height - 32, 22, DARKGRAY);
            DrawText("Triangle: Close", box.x + 110, box.y + box.height - 32, 22, DARKGRAY);
            // special store window while talking to store clerk
            if (don.who == TALK_TYPE_STORE)
            {
                Rectangle storeBox = {
                    box.x + 20,
                    box.y - 245,
                    520,
                    230
                };

                DrawRectangle(storeBox.x - 4, storeBox.y - 4,
                    storeBox.width + 8, storeBox.height + 8, BLACK);

                DrawRectangleRec(storeBox, RAYWHITE);

                DrawText("STORE", storeBox.x + 14, storeBox.y + 10, 26, BLACK);
                DrawText(TextFormat("Money: $%.2f", don.money),
                    storeBox.x + 330, storeBox.y + 14, 20, DARKGREEN);

                int row = 0;

                for (int i = 0; i < INV_TOTAL_TYPES; i++)
                {
                    if (!Store_CanSell(inventory[i].type)) continue;

                    Color c = (i == storeSel) ? RED : BLACK;

                    DrawText(
                        TextFormat("%s%s  $%d  owned:%d",
                            inventory[i].type==INV_APPLE||inventory[i].type==INV_BERRY?"(SELL) ":"",
                            inventory[i].name,
                            Store_GetPrice(inventory[i].type),
                            inventory[i].count),
                        storeBox.x + 18,
                        storeBox.y + 50 + row * 26,
                        20,
                        c
                    );

                    row++;
                }

                DrawText("D-Pad Up/Down: select     X: buy     Triangle: leave",
                    storeBox.x + 18,
                    storeBox.y + storeBox.height - 28,
                    18,
                    GRAY);
            }
        }
        if (onLoad) 
        {
            const Album* a = GM_GetAlbum(&gMusic, gGame.currentAlbumIndex);
            const Song* s = GM_GetSong(a, gGame.currentSongIndex);
            if (a && s) {DrawText(TextFormat("%s - %s  [%s]", a->artist, a->display, s->display), sw - 400, sh - 50, 20, RAYWHITE);}
        }
        if(!loadedEem || !wasTilesDocumented)
        {
            // Outline
            DrawRectangleLines(500, 350, 204, 10, DARKGRAY);
            // Fill
            float chunkPercent = ((float)loadCnt)/(CHUNK_COUNT * CHUNK_COUNT);
            float tilePercent = ((float)loadTileCnt)/manifestTileCount;
            float totalPercent = (chunkPercent+tilePercent)/2.0f;
            int gc = (int)((totalPercent)*255);
            DrawRectangle(502, 352, (int)((200 - 4) * (totalPercent)), 10 - 4, (Color){100,gc,40,255});
        }
        else if(!onLoad)//this used to do something useful, now it does nothing really but snap the player a bit
        {
            onLoad = true;
            float totalY = 0.0f;
            int totalVerts = 0;
            for (int cy = 0; cy < CHUNK_COUNT; cy++) {
                for (int cx = 0; cx < CHUNK_COUNT; cx++) {
                    Mesh mesh = chunks[cx][cy].model.meshes[0];
                    if (mesh.vertexCount == 0 || mesh.vertices == NULL) continue;
                    float *verts = (float *)mesh.vertices;
                    for (int i = 0; i < mesh.vertexCount; i++) {
                        float y = verts[i * 3 + 1];  // Y component
                        totalY += (y * MAP_SCALE);
                    }
                    totalVerts += mesh.vertexCount;
                    if(chunks[cx][cy].treeCount>0){TraceLog(LOG_INFO, "trees (%d,%d) ->  %d", cx,cy,chunks[cx][cy].treeCount);}
                }
            }
            camera.position = Scenes[SCENE_HOME_CABIN_02].pos; //start at home cabin
        }
        DrawFPS(10,10);
        EndDrawing();
    }
    // -----------------------------------------------------------------------------
    // CLEANUP
    // -----------------------------------------------------------------------------
    quitFileManager = true;

    // Since your file worker is detached, give it a moment to notice quitFileManager.
    // Better long-term fix: keep the thread HANDLE and WaitForSingleObject() it.
    sleep_ms(200);

    // ---- audio/music ----
    if (gGame.currentMusicLoaded)
    {
        StopMusicStream(gGame.currentMusic);
        UnloadMusicStream(gGame.currentMusic);
        gGame.currentMusicLoaded = false;
    }

    UnloadSound(carHorn);
    UnloadSound(donScream);
    UnloadSound(menuSelect);
    UnloadSound(menuBack);
    UnloadSound(menuSaveOrLoad);
    UnloadSound(grow);
    UnloadSound(pick);
    UnloadSound(wrenchSound);
    UnloadSound(sharkGulp);

    // ---- main character / actors ----
    FreeDonogan(&don);      // already exists in donogan.h
    FreeShark(&shark);
    Machine_Unload();

    // whales
    if (whales)
    {
        for (int i = 0; i < numWhales; i++)
        {
            if (whales[i].model.meshCount > 0) UnloadModel(whales[i].model);
            if (whales[i].tex.id) UnloadTexture(whales[i].tex);

            if (whales[i].proc.keyframePoses)
            {
                if (whales[i].proc.keyframePoses[0]) MemFree(whales[i].proc.keyframePoses[0]);
                MemFree(whales[i].proc.keyframePoses);
                whales[i].proc.keyframePoses = NULL;
            }
        }

        free(whales);
        whales = NULL;
    }

    // ---- local preview.c models/textures ----
    UnloadModel(rocketModel);
    UnloadModel(rotor);
    UnloadModel(tol);
    UnloadModel(atreyu);
    UnloadModel(canoe);
    UnloadModel(wrenchModel);
    UnloadModel(treeCubeModel);
    UnloadModel(ball);
    UnloadModel(fireModel);

    UnloadTexture(don_head);
    UnloadTexture(tol_head);
    UnloadTexture(atreyu_head);
    UnloadTexture(darrel_head);
    UnloadTexture(lucy_head);

    UnloadTexture(mapTexture);

    // ---- truck ----
    UnloadModel(truck);
    UnloadModel(tire);

    // If these textures were separately loaded into truckMaterial/tireMaterial,
    // unload them too. Your InitTruck loads texture handles into materials.
    if (truckMaterial.maps[MATERIAL_MAP_DIFFUSE].texture.id)
    {
        UnloadTexture(truckMaterial.maps[MATERIAL_MAP_DIFFUSE].texture);
    }
    if (tireMaterial.maps[MATERIAL_MAP_DIFFUSE].texture.id)
    {
        UnloadTexture(tireMaterial.maps[MATERIAL_MAP_DIFFUSE].texture);
    }
    UnloadMaterial(truckMaterial);
    UnloadMaterial(tireMaterial);

    // ---- skybox ----
    UnloadTexture(skyTexFront);
    UnloadTexture(skyTexBack);
    UnloadTexture(skyTexLeft);
    UnloadTexture(skyTexRight);
    UnloadTexture(skyTexUp);

    // ---- generated bug/star buffers ----
    if (bugGenHappened)
    {
        free(bugs);
        bugs = NULL;
    }

    if (starGenHappened)
    {
        free(stars);
        stars = NULL;
    }

    // ---- fonts ----
    UnloadFont(req_font);
    UnloadFont(res_font);
    // do NOT unload default_font from GetFontDefault()

    // ---- shaders ----
    UnloadShader(instancingLightShader);
    UnloadShader(grassInstancingLightShader);
    UnloadShader(lightningBugShader);
    UnloadShader(starShader);
    UnloadShader(lightningBall);
    UnloadShader(ghostShader);
    UnloadShader(fireShader);
    UnloadShader(gWaterShader);

    // ---- static prop models/textures loaded by InitStaticGameProps ----
    for (int i = 0; i < MODEL_TOTAL_COUNT; i++)
    {
        if (StaticObjectModels[i].meshCount > 0) UnloadModel(StaticObjectModels[i]);
        if (HighFiStaticObjectModels[i].meshCount > 0) UnloadModel(HighFiStaticObjectModels[i]);

        if (HighFiStaticObjectModelTextures[i].id) UnloadTexture(HighFiStaticObjectModelTextures[i]);
        if (LowFiStaticObjectModelTextures[i].id) UnloadTexture(LowFiStaticObjectModelTextures[i]);

        // Only unload if you are sure each material owns unique maps.
        // Since these are LoadMaterialDefault copies, this is usually okay:
        //UnloadMaterial(HighFiStaticObjectMaterials[i]);
    }

    // ---- chunks / water / props ----
    if (chunks)
    {
        for (int cy = 0; cy < CHUNK_COUNT; cy++)
        {
            for (int cx = 0; cx < CHUNK_COUNT; cx++)
            {
                Chunk* c = &chunks[cx][cy];

                if (c->isLoaded)
                {
                    if (c->model.meshCount > 0)   UnloadModel(c->model);
                    if (c->model32.meshCount > 0) UnloadModel(c->model32);
                    if (c->model16.meshCount > 0) UnloadModel(c->model16);
                    if (c->model8.meshCount > 0)  UnloadModel(c->model8);

                    if (c->texture.id)     UnloadTexture(c->texture);
                    if (c->textureBig.id)  UnloadTexture(c->textureBig);
                    if (c->textureFull.id) UnloadTexture(c->textureFull);
                    if (c->textureDamn.id) UnloadTexture(c->textureDamn);
                }

                if (c->water)
                {
                    for (int w = 0; w < c->waterCount; w++)
                    {
                        if (c->water[w].model.meshCount > 0)
                        {
                            UnloadModel(c->water[w].model);
                        }
                    }

                    MemFree(c->water);
                    c->water = NULL;
                    c->waterCount = 0;
                }

                free(c->props);
                c->props = NULL;
            }
        }

        for (int x = 0; x < CHUNK_COUNT; x++)
        {
            free(chunks[x]);
        }

        free(chunks);
        chunks = NULL;
    }
    Corn_Unload();
    // ---- tile manifest / compressed tile entries ----
    if (foundTiles)
    {
        for (int i = 0; i < foundTileCount; i++)
        {
            if (foundTiles[i].state == TS_IN_GPU && foundTiles[i].model.meshCount > 0)
            {
                UnloadModel(foundTiles[i].model);
            }

            /*free(foundTiles[i].compData);
            foundTiles[i].compData = NULL;

            free(foundTiles[i].uncompData);
            foundTiles[i].uncompData = NULL;*/
        }

        /*free(foundTiles); //dont actually need to free data, just get it all out of the gpu, as much as possible, os will restore this
        foundTiles = NULL;
        foundTileCount = 0;*/
    }
    UnloadBloomSystem();
    // ---- raylib shutdown ----
    MUTEX_DESTROY(mutex);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
