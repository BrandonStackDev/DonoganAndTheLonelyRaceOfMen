#ifndef CORE_H
#define CORE_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define RLIGHTS_IMPLEMENTATION    // <— add this line, or dont, it wouldnt change the fact I have to bum rides off of people!
#include "rlights.h"

//fairly standard things
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // for seeding rand once if needed
//for big report numbers
#include <stdint.h>
#include <inttypes.h>
//me
#include "util.h"

#ifdef _WIN32
// Keep only the Win32 pieces we actually need //brandon here, no idea what any of this is...
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define NOGDI      // avoids GDI's Rectangle()
#define NOUSER     // avoids User32's CloseWindow/ShowCursor/LoadImage
#include <windows.h>
#include <process.h>

typedef CRITICAL_SECTION Mutex;
Mutex mutex;
#define MUTEX_INIT(m)    InitializeCriticalSection(&(m))
#define MUTEX_LOCK(m)    EnterCriticalSection(&(m))
#define MUTEX_UNLOCK(m)  LeaveCriticalSection(&(m))
#define MUTEX_DESTROY(m) DeleteCriticalSection(&(m))

typedef unsigned(__stdcall* thread_func)(void*);
static int thread_start_detached(thread_func fn, void* arg) {
    uintptr_t th = _beginthreadex(NULL, 0, fn, arg, 0, NULL);
    if (!th) return -1;
    CloseHandle((HANDLE)th);   // detach
    return 0;
}

static void sleep_ms(unsigned ms) { Sleep(ms); }
static void sleep_s(unsigned s) { Sleep(s * 1000u); }
#else
#error This project is currently Windows-only.
#endif

#ifndef GL_POLYGON_OFFSET_FILL
#define GL_POLYGON_OFFSET_FILL 0x8037
#endif

/////end windows section

typedef enum {
    LOD_64,
    LOD_32,
    LOD_16,
    LOD_8
} TypeLOD;

typedef struct {
    int id;
    int cx;
    int cy;
    bool isLoaded; //in GPU
    bool isReady; //in RAM
    bool isTextureReady;
    bool isTextureLoaded;
    BoundingBox box;
    BoundingBox origBox;
    Model model;
    Model model32;
    Model model16;
    Model model8;
    Mesh mesh;
    Mesh mesh32;
    Mesh mesh16;
    Mesh mesh8;
    TypeLOD lod;
    Image img_tex;
    Image img_tex_big;
    Image img_tex_full;
    Image img_tex_damn;
    Texture2D texture;
    Texture2D textureBig;
    Texture2D textureFull;
    Texture2D textureDamn;
    Vector3 position;
    Vector3 center;
    StaticGameObject* props;
    int treeCount;
    int curTreeIdx;
    Model* water;
    int waterCount;
} Chunk;

//tiles-------------------------------------------------------------------------
typedef struct {
    int cx, cy;
    int tx, ty;
    char path[256];
    Mesh mesh;
    //Model_Type type; eventually it might be nice so we could tell what we are drawing if we desire
    Model model;
    BoundingBox box;
    bool isReady, isLoaded; //is ready but !isloaded means needs gpu upload
    Model_Type type;
} TileEntry;

typedef struct {
    float angle; //radians
    float rate;      // vertical up/down rate
    Vector3 pos;
    BoundingBox origBox, box;
    float timer;
    float alpha;
} LightningBug;
#define BUG_COUNT 256 //oh yeah!
typedef struct {
    Vector3 pos;
    float timer;
    float alpha;
} Star;
#define STAR_COUNT 512 //oh yeah!!
Color starColors[4] = {
    {255, 200, 100, 255}, // warm
    {100, 200, 255, 255}, // cool
    {255, 255, 255, 255}, // white
    {200, 100, 255, 255}  // purple-ish
};
#define MAX_ARROWS 64
typedef struct Arrow {
    Vector3 pos;      // tip position (world)
    Vector3 vel;      // m/s, points from tail -> tip
    float   life;     // seconds left
    unsigned char alive;
    unsigned char stuck;
} Arrow;
//////////////////////IMPORTANT GLOBAL VARIABLES///////////////////////////////
//very very important
float scaleNightTransition = 0.0989f;
float gravityCollected = 0.0f;
int chosenX = 7;
int chosenY = 7;
int closestCX = 7;
int closestCY = 7;
bool onLoad = false;
Vector3 lastLBSpawnPosition = { 0 };
TileEntry* foundTiles = NULL; //will be quite large potentially (in reality not as much)
int foundTileCount = 0;
static int foundTilesCap = 0;
static void EnsureFoundTilesCapacity(int need) //no idea what this does .... but thanks chatGPT
{
    if (foundTilesCap >= need) return;
    int newCap = (foundTilesCap > 0) ? foundTilesCap : 4096; // start with a decent chunk
    while (newCap < need) newCap *= 2;

    void* p = realloc(foundTiles, (size_t)newCap * sizeof(TileEntry));
    if (!p) {
        TraceLog(LOG_FATAL, "Out of memory growing foundTiles to %d", newCap);
        exit(1);
    }
    foundTiles = (TileEntry*)p;
    foundTilesCap = newCap;
}

int manifestTileCount = 2048; //start with a guess, not 0 because used as a denominatorfor the load bar
int waterManifestCount = 0; //this is required so start at 0
bool wasTilesDocumented = false;
Color chunk_16_color = { 255,255,255,220 };
Color chunk_08_color = { 255,255,255,180 };
Chunk** chunks = NULL;
Vector3 cameraVelocity = { 0 };
Mesh skyboxPanelMesh;
Model skyboxPanelFrontModel;
Model skyboxPanelBackModel;
Model skyboxPanelLeftModel;
Model skyboxPanelRightModel;
Model skyboxPanelUpModel;
Color backGroundColor = { 100, 190, 255, 255 }; //SKYBLUE;
bool dayTime = true;
Color skyboxDay = { 255, 255, 255, 180 };
Color skyboxNight = { 8,  10,  80, 253 };
Color backgroundDay = { 255, 255, 255, 255 };
Color backgroundNight = { 7,   8, 120, 255 };
Vector3 LightPosTargetDay = { 50.0f, 50.0f, 0.0f };
Vector3 LightPosTargetNight = { 4.0f, 30.0f, 35.0f };
Vector3 LightPosDraw = { 4.0f, 30.0f, 35.0f };
Vector3 LightTargetTargetDay = { 1.0f, 0.0f, 0.0f };
Vector3 LightTargetTargetNight = { 4.0f, 50.0f, 5.0f };
Vector3 LightTargetDraw = { 4.0f, 0.0f, 5.0f };
Color lightColorTargetNight = { 20,   30, 140, 202 };
Color lightColorTargetDay = { 102, 191, 255, 255 };
Color lightColorDraw = { 102, 191, 255, 255 };
Color lightTileColor = { 254, 254, 254, 254 };

BoundingBox UpdateBoundingBox(BoundingBox box, Vector3 pos)
{
    // Calculate the half-extents from the current box
    Vector3 halfSize = {
        (box.max.x - box.min.x) / 2.0f,
        (box.max.y - box.min.y) / 2.0f,
        (box.max.z - box.min.z) / 2.0f
    };

    // Create new min and max based on the new center
    BoundingBox movedBox = {
        .min = {
            pos.x - halfSize.x,
            pos.y - halfSize.y,
            pos.z - halfSize.z
        },
        .max = {
            pos.x + halfSize.x,
            pos.y + halfSize.y,
            pos.z + halfSize.z
        }
    };

    return movedBox;
}
/////////////////////////////////////REPORT FUNCTIONS///////////////////////////////////////////
void MemoryReport()
{
    printf("Start Memory Report -> \n");
    printf("FPS                                : %d\n", GetFPS());
    printf("Chunk Memory         (estimated)   : %zu\n", (CHUNK_COUNT * CHUNK_COUNT) * sizeof(Chunk));
    printf("Batched Props Memory (estimated)   : %zu\n", foundTileCount * sizeof(StaticGameObject));
    int64_t ct_8_tri = 0, ct_16_tri = 0, ct_32_tri = 0, ct_64_tri = 0;
    int64_t ct_8_vt = 0, ct_16_vt = 0, ct_32_vt = 0, ct_64_vt = 0;
    for (int cx = 0; cx < CHUNK_COUNT; cx++)
    {
        for (int cy = 0; cy < CHUNK_COUNT; cy++)
        {
            if (!chunks[cx][cy].isLoaded) { continue; }
            ct_64_tri += chunks[cx][cy].model.meshes[0].triangleCount;
            ct_64_vt += chunks[cx][cy].model.meshes[0].vertexCount;
            ct_32_tri += chunks[cx][cy].model32.meshes[0].triangleCount;
            ct_32_vt += chunks[cx][cy].model32.meshes[0].vertexCount;
            ct_16_tri += chunks[cx][cy].model16.meshes[0].triangleCount;
            ct_16_vt += chunks[cx][cy].model16.meshes[0].vertexCount;
            ct_8_tri += chunks[cx][cy].model8.meshes[0].triangleCount;
            ct_8_vt += chunks[cx][cy].model8.meshes[0].vertexCount;
        }
    }
    printf("CHUNK 64 Triangles             : %" PRId64 "\n", ct_64_tri);
    printf("CHUNK 64 Vertices              : %" PRId64 "\n", ct_64_vt);
    printf("CHUNK 32 Triangles             : %" PRId64 "\n", ct_32_tri);
    printf("CHUNK 32 Vertices              : %" PRId64 "\n", ct_32_vt);
    printf("CHUNK 16 Triangles             : %" PRId64 "\n", ct_16_tri);
    printf("CHUNK 16 Vertices              : %" PRId64 "\n", ct_16_vt);
    printf("CHUNK 08 Triangles             : %" PRId64 "\n", ct_8_tri);
    printf("CHUNK 08 Vertices              : %" PRId64 "\n", ct_8_vt);
    int64_t chunkTotalTri = ct_8_tri + ct_16_tri + ct_32_tri + ct_64_tri;
    int64_t chunkTotalVert = ct_8_vt + ct_16_vt + ct_32_vt + ct_64_vt;
    printf("CHUNK TOTAL Triangles          : %" PRId64 "\n", chunkTotalTri);
    printf("CHUNK TOTAL Vertices           : %" PRId64 "\n", chunkTotalVert);
    printf("(found tiles %d)\n", foundTileCount);
    int64_t tileGpuTri = 0, tileGpuVert = 0;
    int64_t tileTotalTri = 0, tileTotalVert = 0;
    for (int i = 0; i < foundTileCount; i++)
    {
        if (!foundTiles[i].isReady) { continue; }
        tileTotalTri += foundTiles[i].model.meshes[0].triangleCount;
        tileTotalVert += foundTiles[i].model.meshes[0].vertexCount;
        if (!foundTiles[i].isLoaded) { continue; }
        tileGpuTri += foundTiles[i].model.meshes[0].triangleCount;
        tileGpuVert += foundTiles[i].model.meshes[0].vertexCount;
    }
    printf("GPU   Tile Triangles           : %" PRId64 "\n", tileGpuTri);
    printf("GPU   Tile Vertices            : %" PRId64 "\n", tileGpuVert);
    printf("Total Tile Triangles           : %" PRId64 "\n", tileTotalTri);
    printf("Total Tile Vertices            : %" PRId64 "\n", tileTotalVert);
    printf("Total Triangles                : %" PRId64 "\n", tileTotalTri + chunkTotalTri);
    printf("Total Vertices                 : %" PRId64 "\n", tileTotalVert + chunkTotalVert);
    printf("   ->   ->   End Memory Report. \n");
}

void GridChunkReport()
{
    printf("Start Chunk Grid Report -> \n");
    for (int cx = 0; cx < CHUNK_COUNT; cx++)
    {
        for (int cy = 0; cy < CHUNK_COUNT; cy++)
        {
            if (chunks[cx][cy].lod == LOD_8) { continue; }//dont show these as they are numerous and beligerant
            printf("Chunk %d %d - %d\n", cx, cy, chunks[cx][cy].lod);
        }
    }
    printf("   ->   ->   End Chunk Grid Report. \n");
}

void GridTileReport()
{
    printf("Start Tile Grid Report -> \n");
    for (int cx = 0; cx < CHUNK_COUNT; cx++)
    {
        for (int cy = 0; cy < CHUNK_COUNT; cy++)
        {
            if (chunks[cx][cy].lod != LOD_64) { continue; }//we only care about the active tile grid
            for (int i = 0; i < foundTileCount; i++)
            {
                if (foundTiles[i].cx == cx && foundTiles[i].cy == cy)
                {
                    printf("(%d,%d) - [%d,%d] - {%d,%d} - %s\n",
                        cx, cy,
                        foundTiles[i].tx, foundTiles[i].ty,
                        foundTiles[i].isReady, foundTiles[i].isLoaded,
                        GetModelName(foundTiles[i].type)
                    );
                }
            }
        }
    }
    printf("   ->   ->   End Tile Grid Report. \n");
}

////////////////////////////////////////////////////////////////////////////////
// Strip GPU buffers but keep CPU data
void UnloadMeshGPU(Mesh* mesh) {
    //MUTEX_LOCK(mutex);
    rlUnloadVertexArray(mesh->vaoId);
    /*for (int i = 0; i < MAX_MESH_VERTEX_BUFFERS; i++) { //this always fails so just removing it
        if ((mesh->vboId[i]) != 0)
        {
            rlUnloadVertexBuffer(mesh->vboId[i]);
        }
        mesh->vboId[i] = 0;
    }*/
    mesh->vaoId = 0;
    //mesh->vboId[0] = 0;
    //MUTEX_UNLOCK(mutex);
}

int loadTileCnt = 0; //-- need this counter to be global, counted in these functions
void OpenTiles()
{
    FILE* f = fopen("map/manifest.txt", "r"); // Open for read
    if (f != NULL) {
        char line[512];  // Adjust size based on expected path lengths
        while (fgets(line, sizeof(line), f)) {
            // Remove newline if present
            //char *newline = strchr(line, '\n');
            //if (newline) *newline = '\0';
            int cx, cy, tx, ty, type;
            char path[256];
            if (sscanf(line, "%d %d %d %d %d %255[^\n]", &cx, &cy, &tx, &ty, &type, path) == 6)
            {
                // Save entry
                TileEntry entry = { cx, cy, tx, ty };
                strcpy(entry.path, path);
                if (chunks[cx][cy].lod == LOD_64 || chunks[cx][cy].lod == LOD_64)//only get ready close enough tiles
                {
                    entry.model = LoadModel(entry.path);
                    entry.mesh = entry.model.meshes[0];
                    entry.isReady = true;
                }
                entry.type = (Model_Type)type;
                MUTEX_LOCK(mutex);
                EnsureFoundTilesCapacity(foundTileCount + 1);
                foundTiles[foundTileCount++] = entry;
                MUTEX_UNLOCK(mutex);
                TraceLog(LOG_INFO, "manifest entry: %s", path);
                loadTileCnt++;
            }
            else {
                printf("Malformed line: %s\n", line);
            }
        }
        fclose(f);
    }
}
void DocumentTiles(int cx, int cy)
{
    for (int tx = 0; tx < TILE_GRID_SIZE; tx++) {
        for (int ty = 0; ty < TILE_GRID_SIZE; ty++) {
            for (int i = 0; i < MODEL_TOTAL_COUNT; i++)
            {
                MUTEX_LOCK(mutex);
                char path[256];
                snprintf(path, sizeof(path),
                    "map/chunk_%02d_%02d/tile_64/%02d_%02d/tile_%s_64.obj",
                    cx, cy, tx, ty, GetModelName(i));

                FILE* f = fopen(path, "r");
                if (f) {
                    fclose(f);
                    // Save entry
                    TileEntry entry = { cx, cy, tx, ty };
                    strcpy(entry.path, path);
                    entry.model = LoadModel(entry.path);
                    entry.mesh = entry.model.meshes[0];
                    entry.isReady = true;
                    entry.type = (Model_Type)i;
                    MUTEX_LOCK(mutex);
                    EnsureFoundTilesCapacity(foundTileCount + 1);
                    foundTiles[foundTileCount++] = entry;
                    MUTEX_UNLOCK(mutex);
                    TraceLog(LOG_INFO, "Found tile: %s", path);
                    loadTileCnt++;
                }
                MUTEX_UNLOCK(mutex);
            }
        }
    }
}
//
Color LerpColor(Color from, Color to, float t)
{
    Color result = {
        .r = (unsigned char)(from.r + (to.r - from.r) * t),
        .g = (unsigned char)(from.g + (to.g - from.g) * t),
        .b = (unsigned char)(from.b + (to.b - from.b) * t),
        .a = (unsigned char)(from.a + (to.a - from.a) * t)
    };
    return result;
}

Vector3 LerpVector3(Vector3 from, Vector3 to, float t)
{
    Vector3 result = {
        .x = (float)(from.x + (to.x - from.x) * t),
        .y = (float)(from.y + (to.y - from.y) * t),
        .z = (float)(from.z + (to.z - from.z) * t)
    };
    return result;
}

int lightCount = 0;
// Send light properties to shader
// NOTE: Light shader locations should be available
static void UpdateLightPbr(Shader shader, Light light)
{
    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

    // Send to shader light position values
    float position[3] = { light.position.x, light.position.y, light.position.z };
    SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

    // Send to shader light target position values
    float target[3] = { light.target.x, light.target.y, light.target.z };
    SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, light.colorLoc, &light.color, SHADER_UNIFORM_VEC4);
    //SetShaderValue(shader, light.intensityLoc, &light.intensity, SHADER_UNIFORM_FLOAT);
}
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
        float dist = ((float)GetRandomValue(0, 1000) / 1000.0f) * maxDistance; // random 0 to maxDistance

        float x = cameraPos.x + cosf(angle) * dist;
        float z = cameraPos.z + sinf(angle) * dist;
        bugs[i].angle = 0.0f;
        bugs[i].pos = (Vector3){ x, 0.0f, z }; // you'll set .y later
        if (onLoad) { bugs[i].pos.y = GetTerrainHeightFromMeshXZ(bugs[i].pos.x, bugs[i].pos.z); }
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
        float dist = ((float)GetRandomValue(0, 1000) / 1000.0f) * maxDistance; // random 0 to maxDistance

        float x = cameraPos.x + cosf(angle) * dist;
        float z = cameraPos.z + sinf(angle) * dist;
        bugs[i].angle = 0.0f;
        bugs[i].pos = (Vector3){ x, 0.0f, z }; // you'll set .y later
        if (onLoad) { bugs[i].pos.y = GetTerrainHeightFromMeshXZ(bugs[i].pos.x, bugs[i].pos.z); }
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
        float randDelt = (float)((float)GetRandomValue(0, 10)) / 18788.0f;
        float randDeltZ = (float)((float)GetRandomValue(0, 10)) / 18888.0f;
        bugs[i].pos.x += cosf(bugs[i].angle) * speed * deltaTime + randDelt;
        bugs[i].pos.z += sinf(bugs[i].angle) * speed * deltaTime + randDeltZ;

        // Drift the angle slightly (wander)
        float angleWander = ((float)GetRandomValue(-50, 50) / 10000.0f) * PI; // small random
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
            if (GetRandomValue(0, 2970000) < 23)
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

void UpdateStars(Star* stars, int count)
{
    for (int i = 0; i < count; i++)
    {
        // === XZ movement ===
        //new stuff
        stars[i].timer -= 0.0001;
        if (stars[i].timer <= 0.0f)
        {
            // 25% chance this bug blinks
            if (GetRandomValue(1, 597) < 13)
            {
                stars[i].alpha = 1.0f;
            }
            stars[i].timer = 1.0f + (GetRandomValue(0, 100000) / 100.0f); // reset 1–2 sec
        }
        // fade out
        if (stars[i].alpha > 0.0f)
        {
            stars[i].alpha -= 0.07f; // fade fast
            if (stars[i].alpha < 0.0f) stars[i].alpha = 0.0f;
        }
    }
}

//stars
bool starGenHappened = false;
Star* GenerateStars(int count)
{
    Star* stars = (Star*)malloc(sizeof(Star) * count);
    if (!stars) return NULL;

    for (int i = 0; i < count; i++)
    {
        float angle = (float)GetRandomValue(0, 359) * 0.88;
        float dist = ((float)GetRandomValue(0, 1000) / 1000.0f) * 9008; // random 0 to maxDistance

        float x = cosf(angle) * dist;
        float z = sinf(angle) * dist;
        stars[i].pos = (Vector3){ x, GetRandomValue(1800, 2400), z }; // you'll set .y later
    }
    return stars;
}


//water is similar to tiles with a manifest
void OpenWaterObjects(Shader shader) {
    FILE* f = fopen("map/water_manifest.txt", "r"); // Open the manifest
    if (!f) {
        TraceLog(LOG_WARNING, "Failed to open water manifest");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        int cx, cy, patch;
        if (sscanf(line, "%d %d %d", &cx, &cy, &patch) == 3) {
            if (cx >= 0 && cx < CHUNK_COUNT && cy >= 0 && cy < CHUNK_COUNT) {
                // Build file path
                char path[256];
                snprintf(path, sizeof(path), "map/chunk_%02d_%02d/water/patch_%d.obj", cx, cy, patch);

                Model model = LoadModel(path);
                if (model.meshCount > 0) {
                    // Allocate if needed
                    if (chunks[cx][cy].water == NULL) {
                        chunks[cx][cy].water = MemAlloc(sizeof(Model) * MAX_WATER_PATCHES_PER_CHUNK);
                        chunks[cx][cy].waterCount = 0;
                    }

                    if (chunks[cx][cy].waterCount < MAX_WATER_PATCHES_PER_CHUNK) {
                        chunks[cx][cy].water[chunks[cx][cy].waterCount] = model;
                        chunks[cx][cy].water[chunks[cx][cy].waterCount].materials[0].shader = shader;
                        chunks[cx][cy].water[chunks[cx][cy].waterCount].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 0, 100, 255, 255 }; // semi-transparent blue;
                        chunks[cx][cy].waterCount++;
                        TraceLog(LOG_INFO, "Loaded water model: %s", path);
                    }
                    else {
                        TraceLog(LOG_WARNING, "Too many water patches in chunk %d,%d", cx, cy);
                        UnloadModel(model);
                    }
                }
                else {
                    TraceLog(LOG_WARNING, "Failed to load water mesh: %s", path);
                }
            }
        }
        else {
            TraceLog(LOG_WARNING, "Malformed line in water manifest: %s", line);
        }
    }

    fclose(f);
}

// Convert world-space position to global tile coordinates
void GetGlobalTileCoords(Vector3 pos, int* out_gx, int* out_gy) {
    float worldX = pos.x + WORLD_ORIGIN_OFFSET;
    float worldZ = pos.z + WORLD_ORIGIN_OFFSET;

    *out_gx = (int)(worldX / TILE_WORLD_SIZE);
    *out_gy = (int)(worldZ / TILE_WORLD_SIZE);
}
bool IsTreeInActiveTile(Vector3 pos, int pgx, int pgy) {
    int gx, gy;
    GetGlobalTileCoords(pos, &gx, &gy);

    return (gx >= pgx - ACTIVE_TILE_GRID_OFFSET && gx <= pgx + ACTIVE_TILE_GRID_OFFSET &&
        gy >= pgy - ACTIVE_TILE_GRID_OFFSET && gy <= pgy + ACTIVE_TILE_GRID_OFFSET);
}
bool IsTileActive(int cx, int cy, int tx, int ty, int pgx, int pgy) {
    int tile_gx = cx * TILE_GRID_SIZE + tx;
    int tile_gy = cy * TILE_GRID_SIZE + ty;

    return (tile_gx >= pgx - ACTIVE_TILE_GRID_OFFSET && tile_gx <= pgx + ACTIVE_TILE_GRID_OFFSET &&
        tile_gy >= pgy - ACTIVE_TILE_GRID_OFFSET && tile_gy <= pgy + ACTIVE_TILE_GRID_OFFSET);
}
//-------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////CUSTOM CAMERA PROJECTION//////////////////////////////////////////////////
void SetCustomCameraProjection(Camera camera, float fovY, float aspect, float nearPlane, float farPlane) {
    // Build custom projection matrix
    Matrix proj = MatrixPerspective(fovY * DEG2RAD, aspect, nearPlane, farPlane);
    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();
    rlMultMatrixf(MatrixToFloat(proj));  // Apply custom projection

    // Re-apply view matrix (so things don’t disappear)
    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();
    Matrix view = MatrixLookAt(camera.position, camera.target, camera.up);
    rlMultMatrixf(MatrixToFloat(view));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TakeScreenshotWithTimestamp(void) {
    // Get timestamp
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char filename[128];
    strftime(filename, sizeof(filename), "screenshot/screenshot_%Y%m%d_%H%M%S.png", t);

    // Save the screenshot
    TakeScreenshot(filename);
    TraceLog(LOG_INFO, "Saved screenshot: %s", filename);
}

BoundingBox ScaleBoundingBox(BoundingBox box, Vector3 scale)
{
    // Compute the center of the box
    Vector3 center = {
        (box.min.x + box.max.x) / 2.0f,
        (box.min.y + box.max.y) / 2.0f,
        (box.min.z + box.max.z) / 2.0f
    };

    // Compute half-size (extent) of the box
    Vector3 halfSize = {
        (box.max.x - box.min.x) / 2.0f,
        (box.max.y - box.min.y) / 2.0f,
        (box.max.z - box.min.z) / 2.0f
    };

    // Apply scaling to the half-size
    halfSize.x *= scale.x;
    halfSize.y *= scale.y;
    halfSize.z *= scale.z;

    // Create new box
    BoundingBox scaledBox = {
        .min = {
            center.x - halfSize.x,
            center.y - halfSize.y,
            center.z - halfSize.z
        },
        .max = {
            center.x + halfSize.x,
            center.y + halfSize.y,
            center.z + halfSize.z
        }
    };

    return scaledBox;
}

Color skyboxTint = { 255,255,255,100 };
void DrawSkyboxPanelFixed(Model model, Vector3 position, float angleDeg, Vector3 axis, float size)
{
    Vector3 scale = (Vector3){ size, size, 1.0f };
    DrawModelEx(model, position, axis, angleDeg, scale, skyboxTint);
}

void FindClosestChunkAndAssignLod(Vector3 pos)
{
    bool foundChunkWithBox = false;
    if (!foundChunkWithBox)//compute it directly
    {
        int half = CHUNK_COUNT / 2;
        int chunkX = (int)floor(pos.x / CHUNK_WORLD_SIZE) + half;
        int chunkY = (int)floor(pos.z / CHUNK_WORLD_SIZE) + half;
        //TraceLog(LOG_INFO, "!foundChunkWithBox => (%f,%f)=>[%d,%d]",camera->position.x,camera->position.z, chunkX, chunkY);
        if (chunkX >= 0 && chunkX < CHUNK_COUNT &&
            chunkY >= 0 && chunkY < CHUNK_COUNT &&
            chunks[chunkX][chunkY].isLoaded)
        {
            closestCX = chunkX;
            closestCY = chunkY;
        }
        else if (onLoad)
        {
            TraceLog(LOG_WARNING, "Camera is outside of valid chunk bounds");
        }
    }
    //TraceLog(LOG_INFO, "FindClosestChunkAndAssignLod (2): (%d x %d)", closestCX, closestCY);
    // --- Second pass: assign LODs ---
    for (int cy = 0; cy < CHUNK_COUNT; cy++) {
        for (int cx = 0; cx < CHUNK_COUNT; cx++) {
            int dx = abs(cx - closestCX);
            int dy = abs(cy - closestCY);

            if (cx == closestCX && cy == closestCY) {
                chunks[cx][cy].lod = LOD_64;  // Highest LOD
            }
            else if (dx <= 1 && dy <= 1) {
                chunks[cx][cy].lod = LOD_64;
            }
            else if (dx <= 2 && dy <= 2) {
                chunks[cx][cy].lod = LOD_32;
            }
            else if (dx <= 3 && dy <= 3) {
                chunks[cx][cy].lod = LOD_16;
            }
            else {
                chunks[cx][cy].lod = LOD_8;
            }
        }
    }
}

bool FindNextTreeInChunk(Camera3D* camera, int cx, int cy, float minDistance, Model_Type type) {
    if (!chunks[cx][cy].props || chunks[cx][cy].props <= 0) {
        TraceLog(LOG_INFO, "No props data loaded for chunk_%02d_%02d -> (%d)", cx, cy, chunks[cx][cy].treeCount);
        return false;
    }

    int count = chunks[cx][cy].treeCount;
    if (count <= 0) return false;
    if (count <= chunks[cx][cy].curTreeIdx) { chunks[cx][cy].curTreeIdx = 0; }//if for whatever reason our index is too large, reset
    float minDistSqr = minDistance * minDistance;
    Vector3 camPos = camera->position; //readonly

    for (int i = chunks[cx][cy].curTreeIdx; i < count; i++) {
        Vector3 t = chunks[cx][cy].props[i].pos;
        float dx = t.x - camPos.x;
        float dy = t.y - camPos.y;
        float dz = t.z - camPos.z;
        float distSqr = dx * dx + dy * dy + dz * dz;

        if (distSqr >= minDistSqr && chunks[cx][cy].props[i].type == type) {
            camera->position = (Vector3){ t.x + 0.987, t.y + PLAYER_HEIGHT, t.z + 1.1 };  // Set cam above tree
            FindClosestChunkAndAssignLod(camera->position);
            camera->target = (Vector3){ 0, 0, t.z };           // Look at the tree but not really
            chunks[cx][cy].curTreeIdx = i;
            return true;
        }
    }
    TraceLog(LOG_INFO, "No suitable trees found in this chunk.");
    return false;  // No suitable tree found
}

bool FindAnyTreeInWorld(Camera* camera, float radius, Model_Type type) {
    int attempts = 0;
    const int maxAttempts = MAX_CHUNK_DIM * MAX_CHUNK_DIM;

    while (attempts < maxAttempts) {
        int cx = rand() % MAX_CHUNK_DIM;
        int cy = rand() % MAX_CHUNK_DIM;

        if (FindNextTreeInChunk(camera, cx, cy, radius, type)) {
            TraceLog(LOG_INFO, "Found tree in chunk_%02d_%02d", cx, cy);
            return true;
        }

        attempts++;
    }

    TraceLog(LOG_WARNING, "No suitable trees found in any chunk."); //this one is a warning because this would mean no trees anywhere
    return false;
}

void ImageDataFlipVertical(Image* image) {
    int width = image->width;
    int height = image->height;
    int bytesPerPixel = 4;  // Assuming RGBA8
    int stride = width * bytesPerPixel;

    unsigned char* pixels = (unsigned char*)image->data;
    unsigned char* tempRow = (unsigned char*)malloc(stride);

    for (int y = 0; y < height / 2; y++) {
        unsigned char* row1 = pixels + y * stride;
        unsigned char* row2 = pixels + (height - 1 - y) * stride;

        memcpy(tempRow, row1, stride);
        memcpy(row1, row2, stride);
        memcpy(row2, tempRow, stride);
    }

    free(tempRow);
}

void ImageDataFlipHorizontal(Image* image) {
    int width = image->width;
    int height = image->height;
    int bytesPerPixel = 4;  // Assuming RGBA8
    unsigned char* pixels = (unsigned char*)image->data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width / 2; x++) {
            int i1 = (y * width + x) * bytesPerPixel;
            int i2 = (y * width + (width - 1 - x)) * bytesPerPixel;

            for (int b = 0; b < bytesPerPixel; b++) {
                unsigned char tmp = pixels[i1 + b];
                pixels[i1 + b] = pixels[i2 + b];
                pixels[i2 + b] = tmp;
            }
        }
    }
}

Image LoadSafeImage(const char* filename) {
    Image img = LoadImage(filename);
    // if (img.width != 64 || img.height != 64) {
    //     TraceLog(LOG_WARNING, "Image %s is not 64x64: (%d x %d)", filename, img.width, img.height);
    // }

    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageDataFlipVertical(&img);
    // //ImageDataFlipHorizontal(&img);
    // Texture2D tex = LoadTextureFromImage(img);
    // UnloadImage(img);

    // if (tex.id == 0) {
    //     TraceLog(LOG_WARNING, "Failed to create texture from image: %s", filename);
    // }
    // return tex;
    return img;
}

void PreLoadTexture(int cx, int cy)
{
    //char colorPath[256];
    //char colorBigPath[256];
    //char slopePath[256];
    //char slopeBigPath[256];
    char avgPath[64];
    char avgBigPath[64];
    char avgFullPath[64];
    char avgDamnPath[64];
    //snprintf(colorPath, sizeof(colorPath), "map/chunk_%02d_%02d/color.png", cx, cy);
    //snprintf(colorBigPath, sizeof(colorBigPath), "map/chunk_%02d_%02d/color_big.png", cx, cy);
    //snprintf(slopePath, sizeof(slopePath), "map/chunk_%02d_%02d/slope.png", cx, cy);
    //snprintf(slopeBigPath, sizeof(slopeBigPath), "map/chunk_%02d_%02d/slope_big.png", cx, cy);
    snprintf(avgPath, sizeof(avgPath), "map/chunk_%02d_%02d/avg.png", cx, cy);
    snprintf(avgBigPath, sizeof(avgBigPath), "map/chunk_%02d_%02d/avg_big.png", cx, cy);
    snprintf(avgFullPath, sizeof(avgFullPath), "map/chunk_%02d_%02d/avg_full.png", cx, cy);
    snprintf(avgDamnPath, sizeof(avgDamnPath), "map/chunk_%02d_%02d/avg_damn.png", cx, cy);
    // --- Load images and assign to model material ---
    TraceLog(LOG_INFO, "Loading image in worker thread: %s", avgPath);
    Image img = LoadSafeImage(avgPath); //using slope and color avg right now
    Image imgBig = LoadSafeImage(avgBigPath);
    Image imgFull = LoadSafeImage(avgFullPath);
    Image imgDamn = LoadSafeImage(avgDamnPath);
    chunks[cx][cy].img_tex = img;
    chunks[cx][cy].img_tex_big = imgBig;
    chunks[cx][cy].img_tex_full = imgFull;
    chunks[cx][cy].img_tex_damn = imgDamn;
    chunks[cx][cy].isTextureReady = true;
}

void LoadTreePositions(int cx, int cy)
{
    char treePath[64];
    snprintf(treePath, sizeof(treePath), "map/chunk_%02d_%02d/trees.txt", cx, cy);

    FILE* fp = fopen(treePath, "r");
    if (!fp) {
        TraceLog(LOG_WARNING, "No tree file for chunk (%d,%d)", cx, cy);
        return;
    }

    int treeCount = 0;
    fscanf(fp, "%d\n", &treeCount);
    if (treeCount <= 0) {
        fclose(fp);
        return;
    }

    //Vector3 *treePositions = (Vector3 *)malloc(sizeof(Vector3) * (treeCount + 1));
    StaticGameObject* treePositions = malloc(sizeof(StaticGameObject) * (MAX_PROPS_UPPER_BOUND));//some buffer for these, should never be above 512
    for (int i = 0; i < treeCount; i++) {
        float x, y, z;
        int type;
        float yaw, pitch, roll, scale;
        fscanf(fp, "%f %f %f %d %f %f %f %f\n", &x, &y, &z, &type, &yaw, &pitch, &roll, &scale);
        treePositions[i] = (StaticGameObject){ type, (Vector3) { x, y, z }, yaw, pitch, roll, scale };
    }

    fclose(fp);

    chunks[cx][cy].props = treePositions;
    chunks[cx][cy].treeCount = treeCount;
    TraceLog(LOG_INFO, "Loaded %d trees for chunk (%d,%d)", treeCount, cx, cy);
}

void LoadChunk(int cx, int cy)
{
    // --- Assemble filenames based on chunk coordinates ---
    char objPath[256];
    char objPath32[256];
    char objPath16[256];
    char objPath8[256];
    snprintf(objPath, sizeof(objPath), "map/chunk_%02d_%02d/64.obj", cx, cy);
    snprintf(objPath32, sizeof(objPath32), "map/chunk_%02d_%02d/32.obj", cx, cy);
    snprintf(objPath16, sizeof(objPath16), "map/chunk_%02d_%02d/16.obj", cx, cy);
    snprintf(objPath8, sizeof(objPath8), "map/chunk_%02d_%02d/8.obj", cx, cy);

    // --- Load 3D model from .obj file ---
    TraceLog(LOG_INFO, "Loading OBJ: %s", objPath);
    Model model = LoadModel(objPath);
    Model model32 = LoadModel(objPath32);
    Model model16 = LoadModel(objPath16);
    Model model8 = LoadModel(objPath8);

    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].textureDamn;
    model32.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].textureFull;
    model16.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].textureBig;
    model8.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunks[cx][cy].texture;

    // --- Position the model in world space ---
    float worldHalfSize = (CHUNK_COUNT * CHUNK_SIZE) / 2.0f;
    Vector3 position = (Vector3){
        (cx * CHUNK_SIZE - worldHalfSize) * MAP_SCALE,
        MAP_VERTICAL_OFFSET,
        (cy * CHUNK_SIZE - worldHalfSize) * MAP_SCALE
    };
    Vector3 center = {
        position.x + (CHUNK_SIZE * 0.5f * MAP_SCALE),
        position.y,
        position.z + (CHUNK_SIZE * 0.5f * MAP_SCALE)
    };

    // --- Store the chunk data ---
    // Save CPU-side mesh before uploading
    //chunks[cx][cy].mesh = model.meshes[0];  // <- capture BEFORE LoadModelFromMesh
    MUTEX_LOCK(mutex);
    chunks[cx][cy].model = model;
    chunks[cx][cy].model32 = model32;
    chunks[cx][cy].model16 = model16;
    chunks[cx][cy].model8 = model8;
    chunks[cx][cy].position = position;
    chunks[cx][cy].center = center;
    chunks[cx][cy].isReady = true;
    chunks[cx][cy].lod = LOD_8;
    //mark the chunk with identifiers
    chunks[cx][cy].cx = cx;
    chunks[cx][cy].cy = cy;
    chunks[cx][cy].id = (cx * CHUNK_COUNT) + cy;
    //load trees
    LoadTreePositions(cx, cy);
    MUTEX_UNLOCK(mutex);
    //report
    TraceLog(LOG_INFO, "Chunk [%02d, %02d] loaded at position (%.1f, %.1f, %.1f)",
        cx, cy, position.x, position.y, position.z);
}

bool quitFileManager = false;
static unsigned __stdcall FileManagerThread(void* arg)
{
    while (!quitFileManager)
    {
        sleep_s(1);
        int localCount;
        for (int te = 0; te < foundTileCount; te++)
        {
            if (!wasTilesDocumented) { continue; }
            if (!foundTiles[te].isReady && chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_64)
            {
                //MUTEX_LOCK(mutex); //this will make the program freeze!
                foundTiles[te].model = LoadModel(foundTiles[te].path);
                foundTiles[te].mesh = foundTiles[te].model.meshes[0];
                foundTiles[te].isReady = true;
                //MUTEX_UNLOCK(mutex);
            }
            else if (foundTiles[te].isReady &&
                (chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_32 ||
                    chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_16 ||
                    chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_8))
            {
                //MUTEX_LOCK(mutex); //this will make the program freeze!
                UnloadModel(foundTiles[te].model);
                foundTiles[te].isReady = false;
                //MUTEX_UNLOCK(mutex);
            }
        }
    }
}

static unsigned __stdcall ChunkLoaderThread(void* arg)
{
    bool haveManifest = false;
    FILE* f = fopen("map/manifest.txt", "r"); // Open for append
    if (f != NULL) {
        haveManifest = true;
        //need to count the lines in the file and then set manifestTileCount
        int lines = 0;
        int c;
        while ((c = fgetc(f)) != EOF) {
            if (c == '\n') lines++;
        }
        manifestTileCount = lines;
        fclose(f);

        // reserve at least that many, plus some headroom
        MUTEX_LOCK(mutex);
        EnsureFoundTilesCapacity(manifestTileCount + 1024);
        MUTEX_UNLOCK(mutex);
    }
    for (int cy = 0; cy < CHUNK_COUNT; cy++) {
        for (int cx = 0; cx < CHUNK_COUNT; cx++) {
            if (!haveManifest)
            {
                manifestTileCount = 2048; //fall back for the load bar, we dont know so guess and hope its close
                DocumentTiles(cx, cy);
            }
            if (!chunks[cx][cy].isLoaded) {
                PreLoadTexture(cx, cy);
                LoadChunk(cx, cy);
            }
        }
    }
    if (haveManifest)//document here so we can check lod_64 setting
    {
        OpenTiles();
    }
    wasTilesDocumented = true;
    //return NULL;
    return 0u;   // not NULL
}

void StartChunkLoader() { thread_start_detached(ChunkLoaderThread, NULL); }
void StartFileManger() { thread_start_detached(FileManagerThread, NULL); }


#endif // CORE_H
