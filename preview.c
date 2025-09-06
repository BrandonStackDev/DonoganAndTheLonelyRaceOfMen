//raylib
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

//#define RLIGHTS_IMPLEMENTATION    // <— add this line, or dont, it wouldnt change the fact I have to bum rides off of people!
#include "rlights.h"
//me
#include "models.h"
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
//fairly standard things
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // for seeding rand once if needed
//for big report numbers
#include <stdint.h>
#include <inttypes.h>
//debugging
//#include <vld.h>

//structs
typedef struct Plane {
    Vector3 normal;
    float d;
} Plane;

typedef struct Frustum {
    Plane planes[6]; // left, right, top, bottom, near, far
} Frustum;

static Plane NormalizePlane(Plane p) {
    float len = Vector3Length(p.normal);
    return (Plane) {
        .normal = Vector3Scale(p.normal, 1.0f / len),
            .d = p.d / len
    };
}

Frustum ExtractFrustum(Matrix mat)
{
    Frustum f;

    // LEFT
    f.planes[0] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 + mat.m0, mat.m7 + mat.m4, mat.m11 + mat.m8 },
            .d = mat.m15 + mat.m12
    });

    // RIGHT
    f.planes[1] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 - mat.m0, mat.m7 - mat.m4, mat.m11 - mat.m8 },
            .d = mat.m15 - mat.m12
    });

    // BOTTOM
    f.planes[2] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 + mat.m1, mat.m7 + mat.m5, mat.m11 + mat.m9 },
            .d = mat.m15 + mat.m13
    });

    // TOP
    f.planes[3] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 - mat.m1, mat.m7 - mat.m5, mat.m11 - mat.m9 },
            .d = mat.m15 - mat.m13
    });

    // NEAR
    f.planes[4] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 + mat.m2, mat.m7 + mat.m6, mat.m11 + mat.m10 },
            .d = mat.m15 + mat.m14
    });

    // FAR
    f.planes[5] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 - mat.m2, mat.m7 - mat.m6, mat.m11 - mat.m10 },
            .d = mat.m15 - mat.m14
    });

    return f;
}

bool IsBoxInFrustum(BoundingBox box, Frustum frustum)
{
    for (int i = 0; i < 6; i++)
    {
        Plane plane = frustum.planes[i];

        // Find the corner of the AABB that is most *opposite* to the normal
        Vector3 positive = {
            (plane.normal.x >= 0) ? box.max.x : box.min.x,
            (plane.normal.y >= 0) ? box.max.y : box.min.y,
            (plane.normal.z >= 0) ? box.max.z : box.min.z
        };

        // If that corner is outside, the box is not visible
        float distance = Vector3DotProduct(plane.normal, positive) + plane.d;
        if (distance < 0) { return false; }
    }

    return true;
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

/// @brief main!
/// @param  
/// @return 
int main(void) {
    SetTraceLogLevel(LOG_ALL);
    MUTEX_INIT(mutex);
    bool displayBoxes = false;
    bool displayLod = false;
    LightningBug *bugs;
    Star *stars;
    bool vehicleMode = false;
    // --- Donny mode state ---
    bool donnyMode = true;
    Vector3 donMove = (Vector3){ 0 };
    ControllerData gpad = { 0 };
    bool havePad = false;
    float moveMag = 0.0f;
    // --- Third-person orbit camera state (around don.pos) ---
    float yaw = 0.0f, pitch = 0.25f, radius = 14.0f;
    //day-night timer
    Timer nightTimer = CreateTimer(128); //128 seconds, just above 2 minutes

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
    
    //int for model type to search for when pressing R
    int modelSearchType = 0;
    //---------------RAYLIB INIT STUFF---------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Map Preview with Trees & Grass");
    InitAudioDevice();
    EnableCursor();//now that we default to donny boy, lets not capture the mouse
    SetTargetFPS(60);
    //load the homes models/scenes and stuff like that
    InitHomes();
    ////whales---------------------------------------------------
    int numWhales = 6; // six whales right now
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
    ////end whales setup-----------------------------------------
        // fish
    // --- FISH SETUP --------------------------------------------------------------
    Model fishModel = LoadModel("models/fish.obj");
    fishModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    fishModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/fish.png");

    // School allocation + seeding
    int numSchools = 13;
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
    // Load lighting shader---------------------------------------------------------------------------------------
    Shader instancingLightShader = LoadShader("shaders/100/lighting_instancing.vs","shaders/100/lighting.fs");
    // Get shader locations
    instancingLightShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(instancingLightShader, "mvp");
    instancingLightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(instancingLightShader, "viewPos");
    // Create one light
    Light instanceLight = CreateLight(LIGHT_DIRECTIONAL, LightPosDraw, LightTargetDraw, lightColorDraw, instancingLightShader);
    //init the static game props stuff
    InitStaticGameProps(instancingLightShader);//get the high fi models ready
    fishModel.materials[0].shader = instancingLightShader; //use this guy for fish as well
    //END -- lighting shader---------------------------------------------------------------------------------------
    //START -- lightning bug shader :)---------------------------------------------------------------------------------------
    // Load PBR shader and setup all required locations
    Shader lightningBugShader = LoadShader("shaders/120/lightning_bug.vs","shaders/120/lightning_bug.fs");
    lightningBugShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(lightningBugShader, "mvp");
    //--stars
    Shader starShader = LoadShader("shaders/120/lighting_star.vs","shaders/120/lighting_star.fs");
    starShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(starShader, "mvp");
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
    mapTexture = LoadTexture("map/treasure_map.png"); //mapTexture = LoadTexture("map/elevation_color_map.png");
    
    //controller and truck and donny
    // //donny boy
    Donogan don = InitDonogan();
    don.pos = Scenes[SCENE_HOME_CABIN_02].pos;
    don.pos.y = 533.333f;
    //tree of Life
    Model tol = LoadModel("models/tree_of_life.obj");
    tol.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/tree_of_life.png");
    Vector3 tolPos = {-334.0f, 564.0f, -497.35f};
    // Load  //todo: move this and most of the truck stuff into truck.h
    Model truck = LoadModel("models/truck.obj");
    truck.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/truck.png");
    Material truckMaterial = LoadMaterialDefault();
        truckMaterial.shader = LoadShader(0, 0);
        truckMaterial.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
        truckMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/truck.png");
    TraceLog(LOG_INFO, "CWD: %s", GetWorkingDirectory());
    TraceLog(LOG_INFO, "Has truck.obj? %d  Has texture? %d",
        FileExists("models/truck.obj"), FileExists("textures/truck.png"));

    if (truck.meshCount == 0) {
        TraceLog(LOG_ERROR, "Truck failed to load: meshCount==0");
        // bail out or skip drawing the truck
    }
    // Load tire
    Model tire = LoadModel("models/tire.obj");
    tire.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/tire.png");
    Material tireMaterial = LoadMaterialDefault();
        tireMaterial.shader = LoadShader(0, 0);
        tireMaterial.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
        tireMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/tire.png");
    Model tires[4] = {tire,tire,tire,tire};
    // Set tire offsets relative to truck
    Vector3 tireOffsets[4] = {
        {  1.6f, 0.0f,  3.36f }, // Front-right
        { -1.58f, 0.0f,  3.36f }, // Front-left - stubby
        {  1.6f, 0.0f, -2.64f }, // Rear-right
        { -1.6f, 0.0f, -2.64f }  // Rear-left
    };
    truckInteractTimer = CreateTimer(1.0f);
    StartTimer(&truckInteractTimer);
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
    skyTexFront = LoadTexture("skybox/sky_front_smooth.png");
    skyTexBack  = LoadTexture("skybox/sky_back_smooth.png");
    skyTexLeft  = LoadTexture("skybox/sky_left_smooth.png");
    skyTexRight = LoadTexture("skybox/sky_right_smooth.png");
    skyTexUp    = LoadTexture("skybox/sky_up_smooth.png");
    skyboxPanelFrontModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexFront;
    skyboxPanelBackModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexBack;
    skyboxPanelLeftModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexLeft;
    skyboxPanelRightModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexRight;
    skyboxPanelUpModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = skyTexUp;
    // for (int cy = 0; cy < CHUNK_COUNT; cy++) {
    //     for (int cx = 0; cx < CHUNK_COUNT; cx++) {
    //         if (!chunks[cx][cy].isLoaded && !chunks[cx][cy].isReady) {
    //             //PreLoadTexture(cx, cy);
    //             //LoadChunk(cx, cy);
    //         }
    //     }
    // }
    // INIT INTERACTIVE POINTS OF INTEREST
    InteractivePoints[POI_TYPE_TRUCK] = (POI){ POI_TYPE_TRUCK , &truckPosition};
    //init the stuff before launching thread launcher
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
        OpenWaterObjects(waterShader);//water manifest is required right now
    }
    //TODO: loop through each chunk, then each water feature for that chunk, set the sahder of the model
    //launch the initial loading background threads
    StartChunkLoader();
    StartFileManger();

    Camera3D camera = {
        .position = (Vector3){ 0.0f, 2000.0f, 0.0f },  // Higher if needed,
        .target = (Vector3){ 0.0f, 0.0f, 0.0f },  // Centered
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

    while (!WindowShouldClose()) {
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
        }
        //controller and truck stuff
        havePad = ReadControllerWindows(0, &gpad);
        if (!vehicleMode && donnyMode)
        {
            bool inBowCam = don.bowMode || (don.bowReleaseCamHold > 0.0f);
            float dt = GetFrameTime();
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
            float baseRadius = 14.0f;
            float baseFov = 80.0f;

            // -- much smaller zoom while aiming --
            float zoomRadius = 13.2f;   // was 10.0f
            float zoomFov = 74.0f;   // was 62.0f

            float followRate = don.bowMode ? 10.0f : 6.0f;
            float zoomRate = 6.0f;
            float fovRate = 6.0f;

            // Base target = torso
            Vector3 desiredTarget = (Vector3){ don.pos.x, don.pos.y + 1.0f, don.pos.z };

            if (inBowCam) {
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
            if (inBowCam) { don.yawY = yaw; } // remove the '-' if you see it

            float desiredRadius = inBowCam ? zoomRadius : baseRadius;
            float desiredFov = inBowCam ? zoomFov : baseFov;

            radius = Lerp(radius, desiredRadius, 1.0f - expf(-zoomRate * dt));
            camera.fovy = Lerp(camera.fovy, desiredFov, 1.0f - expf(-fovRate * dt));

            // orbit from target (unchanged)
            camera.position.x = camera.target.x + radius * cosf(pitch) * sinf(yaw);
            camera.position.y = camera.target.y + radius * sinf(pitch);
            camera.position.z = camera.target.z + radius * cosf(pitch) * cosf(yaw);

            if (gpad.btnTriangle > 0 
                && Vector3Distance(*InteractivePoints[POI_TYPE_TRUCK].pos, don.pos) < 12.4f
                && HasTimerElapsed(&truckInteractTimer))
            {
                StartTimer(&truckInteractTimer);
                vehicleMode = true; donnyMode = false;
            }
        }
        else if (vehicleMode)
        {
            if (truckAirState == AIRBORNE) //gravity
            {
                truckPitch += 0.0001 * GetFrameTime();//dip it slightly down
                if (truckPitch > PI / 7.0f) { truckPitch = PI / 7.0f; }
                truckPosition.y -= GetFrameTime() * GRAVITY * gravityCollected;
                gravityCollected += GetFrameTime() * GRAVITY;
                truckForward.y = Lerp(truckForward.y, 0, GetFrameTime() * GRAVITY * gravityCollected);
                //update tricks - dont shut them off here, just upadte them
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
            if (gpad.btnTriangle > 0 && HasTimerElapsed(&truckInteractTimer) && truckAirState != AIRBORNE)
            {
                StartTimer(&truckInteractTimer);
                vehicleMode = false; donnyMode = true;
                don.pos = Vector3Add(truckPosition, (Vector3) {3,0,-2});
            }
        }
        //old important stuff
        float time = GetTime();
        SetShaderValue(waterShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
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
        if(wasTilesDocumented)
        {
            int gx, gy;
            float time = GetTime(); // or your own time tracker
            SetShaderValue(starShader, GetShaderLocation(starShader, "u_time"), &time, SHADER_UNIFORM_FLOAT);
            GetGlobalTileCoords(camera.position, &gx, &gy);
            int playerTileX  = gx % TILE_GRID_SIZE;
            int playerTileY  = gy % TILE_GRID_SIZE;
            for (int te = 0; te < foundTileCount; te++)
            {
                bool maybeNeeded = (chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_64); //todo: testing this to see if it is my issue
                MUTEX_LOCK(mutex);
                if(foundTiles[te].isReady && !foundTiles[te].isLoaded && maybeNeeded)
                {
                    TraceLog(LOG_INFO, "loading tiles: %d", te);
                    // Upload meshes to GPU
                    UploadMesh(&foundTiles[te].model.meshes[0], false);
                    // Load GPU models
                    //foundTiles[te].model = LoadModelFromMesh(foundTiles[te].model.meshes[0]);
                    foundTiles[te].box = GetModelBoundingBox(foundTiles[te].model);
                    // Apply textures
                    foundTiles[te].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = HighFiStaticObjectModelTextures[foundTiles[te].type];
                    //mark work done
                    foundTiles[te].isLoaded = true;
                    //and now its safe to unlock
                }
                else if(foundTiles[te].isLoaded && !maybeNeeded)
                {
                    foundTiles[te].isLoaded = false;
                    foundTiles[te].isReady = false;
                    UnloadModel(foundTiles[te].model);
                }
                MUTEX_UNLOCK(mutex);
            }
        }
        for (int cy = 0; cy < CHUNK_COUNT; cy++) {
            for (int cx = 0; cx < CHUNK_COUNT; cx++) {
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
        Vector2 mouse = GetMouseDelta();
        yaw -= mouse.x * 0.003f;
        pitch -= mouse.y * 0.003f;
        pitch = Clamp(pitch, -PI/2.0f, PI/2.0f);

        Vector3 forward = {
            cosf(pitch) * cosf(yaw) * MAP_SCALE,
            sinf(pitch) * MAP_SCALE,
            cosf(pitch) * sinf(yaw) * MAP_SCALE
        };
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));

        Vector3 move = { 0 };
        if (IsKeyPressed(KEY_T)) {modelSearchType++; modelSearchType=modelSearchType%MODEL_TOTAL_COUNT;}
        if (IsKeyDown(KEY_R)) 
        {
            if (onLoad && !FindNextTreeInChunk(&camera, closestCX, closestCY, 15.0f, modelSearchType)) {
                TraceLog(LOG_INFO, "No suitable prop found in current chunk.");
                if(!FindAnyTreeInWorld(&camera, 15.0f, modelSearchType))
                {
                    TraceLog(LOG_INFO, "Prop error, we didnt find any on the map this try...");
                }
            }
        }
        //map input
        float goku = false;
        float spd = MOVE_SPEED;
        if (IsKeyPressed(KEY_C)) {DisableCursor();}
        if (IsKeyPressed(KEY_X)) {EnableCursor();}
        if (IsKeyPressed(KEY_Y)) {contInvertY=!contInvertY;}
        if (IsKeyPressed(KEY_M)) showMap = !showMap; // Toggle map
        if (IsKeyDown(KEY_EQUAL)) mapZoom += 0.01f;  // Zoom in (+ key)
        if (IsKeyDown(KEY_MINUS)) mapZoom -= 0.01f;  // Zoom out (- key)
        mapZoom = Clamp(mapZoom, 0.5f, 4.0f);
        //end map input
        if (onLoad && IsKeyPressed(KEY_V)) { vehicleMode = !vehicleMode; donnyMode = false; EnableCursor(); }
        if (IsKeyPressed(KEY_B)) {displayBoxes = !displayBoxes;}
        if (IsKeyPressed(KEY_L)) {displayLod = !displayLod;}
        if (IsKeyPressed(KEY_F12)) {TakeScreenshotWithTimestamp();}
        if (IsKeyPressed(KEY_F11)) {reportOn = true;}
        if (IsKeyPressed(KEY_F10)) {MemoryReport();}
        if (IsKeyPressed(KEY_F9)) {GridChunkReport();}
        if (IsKeyPressed(KEY_F8)) {GridTileReport();}
        if (IsKeyPressed(KEY_PAGE_UP)) {chosenX = (chosenX+1)%CHUNK_COUNT;}
        if (IsKeyPressed(KEY_PAGE_DOWN)) {chosenY = (chosenY+1)%CHUNK_COUNT;}
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {goku=true;move = Vector3Add(move, forward);spd = GOKU_DASH_DIST;TraceLog(LOG_INFO, " --> Instant Transmission -->");}
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {goku=true;move = Vector3Add(move, forward);spd = GOKU_DASH_DIST_SHORT;TraceLog(LOG_INFO, " --> Steady does it -->");}
        if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
        if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
        if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);
        if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
        if (IsKeyDown(KEY_Z)) { dayTime=!dayTime;}
        if (IsKeyDown(KEY_LEFT_SHIFT)) move.y -= (1.0f * MAP_SCALE);
        if (IsKeyPressed(KEY_LEFT_CONTROL)) { donnyMode = !donnyMode; vehicleMode = false; }
        if (IsKeyDown(KEY_SPACE)) move.y += (1.0f * MAP_SCALE);
        if (IsKeyDown(KEY_ENTER)) {chunks[chosenX][chosenY].curTreeIdx=0;closestCX=chosenX;closestCY=chosenY;camera.position.x=chunks[closestCX][closestCY].center.x;camera.position.z=chunks[closestCX][closestCY].center.z;}
        //handle controller input
        if (vehicleMode)
        {
            if(gpad.btnR1 > 0 && truckAirState==AIRBORNE)
            {
                doing360 = true;
            }
            if(gpad.btnR2 > 0 && truckAirState==AIRBORNE) //one trick at a time? NO! Lots of tricks at once!!!! Except sometimes I guess, maybe for more advanced things
            {
                doingFlip = true;
            }
            if(gpad.btnL1 > 0 && truckAirState==AIRBORNE) //one trick at a time? NO! Lots of tricks at once!!!! Except sometimes I guess, maybe for more advanced things
            {
                doingRoll = true;
            }
            if(gpad.btnL2 > 0 && truckAirState==AIRBORNE) //one trick at a time? NO! Lots of tricks at once!!!! Except sometimes I guess, maybe for more advanced things
            {
                doingBonkers = true;
            }
            // Deadzone
            if (fabsf(gpad.normLY) > 0.1f) 
            {
                if(truckAirState!=AIRBORNE)
                {
                    truckSpeed += -gpad.normLY * acceleration * GetFrameTime() * 64.0f;
                }
                //printf("speed=%f",truckSpeed);
            }
            else
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
            if (gpad.btnSquare>0)//square
            {
                if(truckAirState!=AIRBORNE && fabs(truckSpeed)>0)
                {
                    truckSpeed -= deceleration * (truckSpeed>0?1:-1);
                    truckPitch += GetFrameTime();//dip it forward when hitting the breaks
                    printf("Square was pressed and is having an effect!\n");
                    if(isTruckSliding){truckSlidePeek=true;}
                }
            }
            if(gpad.btnCircle > 0){displayTruckPoints = !displayTruckPoints;}
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
        //adjust LOD for applied movement----------------------------------------------------------------------
        //truck position
        // Clamp speed
        if(truckAirState==GROUND) //handle rolling down hills
        {
            if(truckPitch>=PI/15.0f && (fabsf(gpad.normLY) < 0.1f) ) //roll down hills
            {
                truckSpeed += GetFrameTime() * GRAVITY * (1.0f/16.0f) * (1+truckSpeed) * (1+truckSpeed);
            }
            else if(truckPitch<=-PI/15.0f && (fabsf(gpad.normLY) < 0.1f) )//reverse
            {
                truckSpeed -= GetFrameTime() * GRAVITY * (1.0f/16.0f) * (1+truckSpeed) * (1+truckSpeed);
            }
            else
            {
                truckForward.y=0;
            }
        }
        if (truckSpeed > maxSpeed) {truckSpeed = maxSpeed;}
        if (truckSpeed < maxSpeedReverse) {truckSpeed = maxSpeedReverse;printf("max truck speed reverse\n");}
        
        //sliding
        if(truckAirState==GROUND && truckSlideSpeed>=0) // sliding, shut off if not on the ground or the slide is complete
        {
            //sliding 
            if(truckSpeed > 1.23 && fabsf(gpad.normLX) > 0.56f && !isTruckSliding)//trigger slide
            {
                //printf("sliding ... \n");
                isTruckSliding = true;
                truckSlidePeek = false; // we just started
                truckSlideSpeed+=GetFrameTime();
                rotSlide = gpad.normLX * -PI/1.8f;//gpad.normLX>0?-PI/2.0f:PI/2.0f; //use this to control which way we slide
                truckSlideForward = RotateY(truckForward,rotSlide);
            }
            else if (isTruckSliding && fabsf(gpad.normLX) > 0.12)
            {
                rotSlide = gpad.normLX * -PI/1.8f;//gpad.normLX>0?-PI/2.0f:PI/2.0f; //use this to control which way we slide
                truckSlideForward = RotateY(truckForward,rotSlide);
                if(truckSlidePeek){truckSlideSpeed-=GetFrameTime();}
                else{truckSlideSpeed+=GetFrameTime() * truckSpeed;}
                if(truckSlideSpeed > 0.71f && fabsf(gpad.normLX) < 0.88f){truckSlidePeek=true;printf("sliding peeked (%f).... \n", fabsf(gpad.normLX));}
                if(truckSlideSpeed > 1.20f){truckSlideSpeed = 1.200001f;}
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
            if(isTruckSliding)
            {
                printf("!\n");//this should never happen
                //turn off slide if not on ground
                isTruckSliding = false;
                truckSlidePeek = false;
                truckSlideSpeed = 0;
                rotSlide = 0;
            }
        }

        if (!vehicleMode) { truckSpeed = 0; }
        truckForward = (Vector3){ sinf(truckAngle), sinf(-truckPitch), cosf(truckAngle) };
        Vector3 tempTruck = Vector3Scale(truckForward, truckSpeed);
        if(isTruckSliding)
        {
            tempTruck = Vector3Add(Vector3Scale(truckForward, truckSpeed), Vector3Scale(truckSlideForward, truckSlideSpeed));
        }
        truckPosition = Vector3Add(truckPosition, tempTruck);
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
            skyboxTint = LerpColor(skyboxTint, skyboxNight, 0.02f); //dont scale this one
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
            else if (bugGenHappened && Vector3Distance(camera.position,lastLBSpawnPosition)>360.12f)
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
        if(!vehicleMode && !donnyMode)
        {
            camera.target = Vector3Add(camera.position, forward);
            if(onLoad && camera.position.y > PLAYER_FLOAT_Y_POSITION)//he floats underwater
            {
                if (closestCX < 0 || closestCY < 0 || closestCX >= CHUNK_COUNT || closestCY >= CHUNK_COUNT) {
                    // Outside world bounds
                    TraceLog(LOG_INFO, "Outside of world bounds: %d,%d", closestCX, closestCY);
                }
                else
                {
                    float groundY = GetTerrainHeightFromMeshXZ(camera.position.x, camera.position.z);
                    //TraceLog(LOG_INFO, "setting camera y: (%d,%d){%f,%f,%f}[%f]", closestCX, closestCY, camera.position.x, camera.position.y, camera.position.z, groundY);
                    if(groundY < -9000.0f){groundY=camera.position.y - PLAYER_HEIGHT;} // if we error, dont change y
                    camera.position.y = groundY + PLAYER_HEIGHT;  // e.g. +1.8f for standing
                }
            }
        }
        else if (donnyMode && !vehicleMode)
        {
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
                    else
                    {
                        don.groundY = groundY;
                        // right after setting don.groundY
                        Vector3 nrm = GetTerrainNormalFromMeshXZ(don.pos.x, don.pos.z);
                        if (nrm.x == 0 && nrm.y == 0 && nrm.z == 0) nrm = (Vector3){ 0,1,0 }; // fallback
                        don.groundNormal = nrm;
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
        if(onLoad) //time to rock and roll!
        {
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
                    //truckPitch = Lerp(truckPitch,-pitch,GetFrameTime());//
                    truckPitch = pitch;//set it directly here
                    //if(pitch<-0.5f){truckSpeed-=fabs(pitch)*0.006*GetFrameTime();}//slow down if we are climbing to steep, pitch is oppisite of what you expect
                }
                if (truckAirState==AIRBORNE) {
                    verticalVelocity -= GRAVITY * GetFrameTime();  // e.g. gravity = 9.8f
                    truckPosition.y += verticalVelocity * truckSpeed * GetFrameTime();

                    // Check for landing
                    float groundY = GetTerrainHeightFromMeshXZ(truckPosition.x, truckPosition.z);
                    if(groundY < -9000.0f){groundY=truckPosition.y;} 
                    if (truckPosition.y <= groundY) {
                        truckPosition.y = groundY;
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
                            //tireYOffsets[i] += (groundYy - groundY) * GetFrameTime();//move the tire up proportional to the difference between the truck y and tire y
                            truckAirState=LANDING;
                            tireYPos[i] = groundYy;
                            tireYOffset[i] -= (groundY - groundYy) * GetFrameTime();
                            if(tireYOffset[i]>0.12f){tireYOffset[i]=0.12f;}
                            if(tireYOffset[i]<-0.23f){tireYOffset[i]=-0.23f;}
                            rebuildFromTires = true;
                        }
                    }
                } else { //not airborne, either landing or ground
                    if(gpad.btnCross>0)
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
                        if(truckAirState==GROUND && truckPosition.y>groundY)//todo: this might be too aggresive
                        {
                            if(truckPitch<-PI/4.0f && truckSpeed > 1.01f && !isTruckSliding) //not while sliding, this is basically shut off for now
                            {
                                //here, take off!
                                truckAirState=AIRBORNE;
                                verticalVelocity=3.2f * truckSpeed * GetFrameTime(); //natural
                            }
                        }
                        else//LANDING
                        {
                            truckPosition.y = groundY;
                        }
                        //tireYOffsets
                        for(int i=0; i<4; i++)
                        {
                            Vector3 localOffset = RotateY(tireOffsets[i], -truckAngle);
                            Vector3 pos = Vector3Add(truckOrigin, localOffset);
                            float groundYy = GetTerrainHeightFromMeshXZ(pos.x, pos.z);
                            if(groundYy < -9000.0f){groundYy=pos.y;} // if we error, dont change y
                            pos.y = groundYy;
                            tireYPos[i] = pos.y;
                            tireYOffset[i] -= (groundY - groundYy) * GetFrameTime();
                            if(tireYOffset[i]>0.2f){tireYOffset[i]=0.2f;}
                            if(tireYOffset[i]<-0.12f){tireYOffset[i]=-0.12f;}
                            // float swtch = (groundYy - groundY) > 0 ? 1.0f : -1.0f;
                            // if(i<2){truckPitch-=GetFrameTime()*swtch;}//front
                            // else{truckPitch+=GetFrameTime()*swtch;}//back
                            // if(i%2==0){truckRoll+=GetFrameTime()*swtch;}//left
                            // else{truckRoll-=GetFrameTime()*swtch;}//right
                            // if(truckRoll>PI/16.0f){truckRoll=PI/16.0f;}
                            // if(truckRoll<-PI/16.0f){truckRoll=-PI/16.0f;}
                        }
                        rebuildFromTires = true;
                    }
                }
            }
            if(rebuildFromTires)
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
                truckPosition.y = Lerp(truckPosition.y , maxTireY, GetFrameTime()*16.0f);

                // 2. Pitch (X-axis rotation, nose up/down)
                // front height vs back height
                float frontAvg = (fl + fr) / 2.0f;
                float backAvg  = (bl + br) / 2.0f;
                truckPitch = Lerp(truckPitch, -atan2f(frontAvg - backAvg, truckLength),GetFrameTime()*16.0f);  // pitch is positive when nose is up

                // 3. Roll (Z-axis rotation, lean left/right)
                // left height vs right height
                float leftAvg  = (fl + bl) / 2.0f;
                float rightAvg = (fr + br) / 2.0f;
                truckRoll = Lerp(truckRoll, -atan2f(rightAvg - leftAvg, truckWidth),GetFrameTime()*16.0f);//todo: do we need to lerp this, does it make it better or worse
                //--rebuild end ...
            }
            if(truckRoll>PI){truckRoll=PI;}
            if(truckRoll<-PI){truckRoll=-PI;}
            if(truckPitch>PI){truckPitch=PI;}
            if(truckPitch<-PI){truckPitch=-PI;}
            if (vehicleMode)
            {
                camYaw = truckAngle * RAD2DEG + relativeYaw;
                float radYaw = camYaw * DEG2RAD;
                float radPitch = relativePitch * DEG2RAD;
                float followSpeed = 5.0f * GetFrameTime();
                Vector3 offset = {
                    camDistance * cosf(radPitch) * sinf(radYaw),
                    camDistance * sinf(radPitch),
                    camDistance * cosf(radPitch) * cosf(radYaw)
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
        //home collision
        for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
        {
            if (CheckCollisionBoxes(don.box, Scenes[i].box))
            {
                // classify slope: anything flatter than ~50° treated as ground
                const float groundSlopeCos = DEFAULT_GROUND_SLOPE_COS; // or cosf(DEG2RAD*50.0f);
                for (int it = 0; it < 3; ++it)
                {
                    MeshBoxHit hit = CollideAABBWithMeshTriangles(don.outerBox, &HomeModels[Scenes[i].modelType].meshes[0], Scenes[i].pos, Scenes[i].scale, Scenes[i].yaw, groundSlopeCos, false);
                    if (hit.hitGround) {
                        // snap to ground and re-make AABB
                        don.pos.y = hit.groundY;
                    }
                    else if (hit.hit) {
                        DebugLogMeshBoxHit("HOME", i, don.box, don.pos, hit, Scenes[i].pos, Scenes[i].scale);
                        // wall: gently nudge away
                        don.pos = Vector3Add(don.oldPos, hit.push);
                    }
                }
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
        // BeginMode3D(skyCam) ... draw panels around (0,0,0) as you already do
        UpdateCamera(&camera, vehicleMode||donnyMode?CAMERA_THIRD_PERSON:CAMERA_FIRST_PERSON);
        UpdateCamera(&skyCam, CAMERA_FIRST_PERSON);
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
        DonUpdate(&don, havePad ? &gpad : NULL, dt, vehicleMode);
        // Update the light shader with the camera view position
        SetShaderValue(lightningBugShader, lightningBugShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
        SetShaderValue(instancingLightShader, instancingLightShader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position, SHADER_UNIFORM_VEC3);
        //-------------------------------------------------------------------------------

        BeginDrawing();
        ClearBackground(backGroundColor);
        //skybox separate scene
        BeginMode3D(skyCam);
            //skybox stuff
            rlDisableDepthMask();
            /*rlDisableBackfaceCulling();*/
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
            /*rlEnableBackfaceCulling();*/
            rlEnableDepthMask();
        EndMode3D();
        //regular scene of the map
        BeginMode3D(camera);
            if(onLoad){SetCustomCameraProjection(camera, 45.0f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.3f, 5000.0f);} // Near = 1, Far = 4000
            //rlDisableBackfaceCulling();
            bool loadedEem = true;
            int loadCnt = 0;
            //int loadTileCnt = 0; -- this one needs to be global so we can update it while loading tiles
            //get frustum
            Matrix view = MatrixLookAt(camera.position, camera.target, camera.up);
            Matrix proj = MatrixPerspective(DEG2RAD * camera.fovy, SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 5200.1f);
            Matrix projChunk8 = MatrixPerspective(DEG2RAD * camera.fovy, SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 16384.0f);//for far away chunks
            Matrix vp = MatrixMultiply(view, proj);
            Matrix vpChunk8 = MatrixMultiply(view, projChunk8);
            Frustum frustum = ExtractFrustum(vp);
            Frustum frustumChunk8 = ExtractFrustum(vpChunk8);
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
            if (onLoad && donnyMode)
            {
                // Draw Donogan
                DrawModel(don.model, don.pos, don.scale, WHITE); // uses model.transform for rotation
                if (displayBoxes) 
                { 
                    DrawBoundingBox(don.box, RED); 
                    DrawBoundingBox(don.outerBox, GREEN);
                    DrawBoundingBox(don.innerBox, YELLOW);
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
                DonDrawArrows(&don);
                //bubbles
                if (don.inWater) { DonDrawBubbles(&don); }
            }
            //tree of life
            if (true)
            {
                DrawModel(tol, tolPos, 8.0f, WHITE); //(Color) {160,100,220,255}//purple lol!
            }
            //homes
            if (onLoad)
            {
                rlDisableBackfaceCulling();
                for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
                {
                    DrawModelEx(HomeModels[Scenes[i].modelType], Scenes[i].pos, 
                        (Vector3) { 0, 1, 0 }, Scenes[i].yaw * RAD2DEG,
                        (Vector3) { Scenes[i].scale , Scenes[i].scale, Scenes[i].scale}, 
                        WHITE);
                    if (displayBoxes) { DrawBoundingBox(Scenes[i].box, PURPLE); }
                }
                rlEnableBackfaceCulling();
            }
            //whales and fish
            if (onLoad)
            {
                //whales
                for (int i = 0; i < numWhales; i++)
                {
                    FSM_Tick(&whales[i], (float)GetTime(), GetFrameTime());
                    Quaternion qFinal = BuildWorldQuat(&whales[i]);
                    Matrix R = QuaternionToMatrix(qFinal);
                    Matrix T = MatrixTranslate(whales[i].pos.x, whales[i].pos.y, whales[i].pos.z);
                    Matrix S = MatrixScale(10, 10, 10);
                    Matrix whaleXform = MatrixMultiply(S, MatrixMultiply(R,T));
                    DrawMesh(whales[i].model.meshes[0], whales[i].model.materials[0], whaleXform);
                    //DrawSphere(whales[i].pos, 4.0f, RED);
                }
                //fish
                // //fish movmemnt
                // === FISH UPDATE + DRAW ======================================================
                for (int s = 0; s < numSchools; s++)
                {
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
                    for (int i = 0; i < schoolCount; i++) {
                        Fish* f = &fish[s].fish[i];
                        Matrix rot = MatrixRotateY(DEG2RAD * f->yawDeg);
                        Matrix sca = MatrixScale(f->scale, f->scale, f->scale);
                        Matrix tra = MatrixTranslate(f->pos.x, f->pos.y, f->pos.z);
                        schoolMatrices[i] = MatrixMultiply(MatrixMultiply(sca, rot), tra);
                    }

                    // 4) draw entire school in one GPU instancing call
                    DrawMeshInstanced(
                        fishModel.meshes[0],
                        fishModel.materials[0],
                        schoolMatrices,
                        schoolCount
                    );
                    //if (schoolMatrices) { MemFree(schoolMatrices); } //this fails for me, not sure what to do with it...
                }
                // ============================================================================
            }
            if (onLoad && truck.meshCount > 0)
            {
                if (displayTruckPoints)
                {
                    DrawSphere(front, 0.41f, BLUE);
                    DrawSphere(back, 0.41f, RED);
                }
                if (displayTruckForward)
                {
                    DrawLine3D(truckOrigin, Vector3Add(truckOrigin, Vector3Scale(truckForward, (1 + truckSpeed) * truckLength + 1)), MAGENTA);
                }
                DrawMesh(truck.meshes[0], truckMaterial, rotationTruck);//tireOffsets[i]
                for (int i = 0; i < 4; i++)
                {
                    float tireAngleQ = -(tireTurnPos[i]);//fabsf//
                    float tireAngleDelta = 0.0f;//float tireAngleDelta = tireAngleQ;  // Default for rear tires
                    // Compute tire-specific spin and steering
                    float steerAngle = 0.0f;
                    if (i < 2) {
                        // Front tires only — steer left/right
                        steerAngle = PI / 8.0f * gpad.normLX; // tweak max angle
                    }
                    // First apply spin around X (wheel axis), then steering around Y
                    // Step 1: Create rotation matrices for yaw (Y), pitch (X), and roll (Z)
                    //printf("steerAngle : %f\n",steerAngle);
                    Matrix yawMatrix = MatrixRotateY((truckAngle - steerAngle));     // Turn left/right
                    //Matrix yawMatrix   = MatrixRotateY(tireAngleDelta);
                    Matrix pitchMatrix = MatrixRotateX(-tireSpinPos[i]);   // Tilt forward/back //sinf(truckAngle)
                    Matrix rollMatrix = MatrixRotateZ(0);    // Lean left/right
                    //truckTireOffsetMatrix
                    Vector3 tireSpace = RotateY(RotateX(RotateZ(tireOffsets[i], truckRoll + truckTrickRoll), truckPitch - truckTrickPitch), -truckAngle - truckTrickYaw);
                    // Step 2: Combine them in the proper order:
                    // Yaw → Pitch → Roll (you can change order depending on your feel/needs)
                    Matrix rotation = MatrixMultiply(pitchMatrix, MatrixMultiply(yawMatrix, rollMatrix));//neo where are you!
                    // Step 3: Apply position translation
                    rotation.m12 = truckOrigin.x + tireSpace.x;
                    rotation.m13 = truckOrigin.y + tireSpace.y - tireYOffset[i]; //!!!!SPACE TIRES!!!!
                    rotation.m14 = truckOrigin.z + tireSpace.z;
                    DrawMesh(tire.meshes[0], tireMaterial, rotation);//tireOffsets[i]
                }
            }
            //lightning bugs &&&&&&&&&
            if(!dayTime)
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
            //TraceLog(LOG_INFO, "-------TILES DRAWING-----------");
            for(int te = 0; te < foundTileCount; te++)
            {
                // if(chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_64)
                // {
                //     TraceLog(LOG_INFO, "Tile (%d,%d) [%d,%d] {%d}", 
                //         foundTiles[te].tx,foundTiles[te].ty,
                //         foundTiles[te].cx,foundTiles[te].cy,
                //         !IsTileActive(foundTiles[te].cx,foundTiles[te].cy,foundTiles[te].tx,foundTiles[te].ty, gx, gy)
                //         //IsBoxInFrustum(foundTiles[te].box , frustumChunk8)
                //     );
                // }
                if (!wasTilesDocumented) { break; }
                if(!foundTiles[te].isReady){continue;}
                if(!foundTiles[te].isLoaded){continue;}
                //TraceLog(LOG_INFO, "TEST - Maybe - Drawing tile model: chunk %02d_%02d, tile %02d_%02d", foundTiles[te].cx, foundTiles[te].cy, foundTiles[te].tx, foundTiles[te].ty);
                if(chunks[foundTiles[te].cx][foundTiles[te].cy].lod == LOD_64 //this one first because its quick, although it might get removed later
                    && (!IsTileActive(foundTiles[te].cx,foundTiles[te].cy,foundTiles[te].tx,foundTiles[te].ty, gx, gy) || USE_TILES_ONLY) 
                    && IsBoxInFrustum(foundTiles[te].box , frustumChunk8))
                {
                    BeginShaderMode(foundTiles[te].model.materials[0].shader);
                    SetMaterialTexture(&foundTiles[te].model.materials[0], MATERIAL_MAP_DIFFUSE, foundTiles[te].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture);//added this because I was having tiles draw with the wrong texture
                    if(reportOn){tileBcCount++;tileTriCount+=foundTiles[te].model.meshes[0].triangleCount;};
                    DrawModel(foundTiles[te].model, (Vector3){0,0,0}, 1.0f, lightTileColor);
                    // TraceLog(LOG_INFO, "TEST Drawing tile model: chunk %02d_%02d, tile %02d_%02d", foundTiles[te].cx, foundTiles[te].cy, foundTiles[te].tx, foundTiles[te].ty);
                    // TraceLog(LOG_INFO, "Drawing tile %d,%d model with Texture ID: %d Shader ID: %d",
                    //     foundTiles[te].cx, foundTiles[te].cy,
                    //     foundTiles[te].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture.id,
                    //     foundTiles[te].model.materials[0].shader.id
                    // );
                    if(displayBoxes){DrawBoundingBox(foundTiles[te].box,RED);}
                    EndShaderMode();
                }
            }
            //TraceLog(LOG_INFO, "-------END TILES DRAWING END-----------");
            for (int cy = 0; cy < CHUNK_COUNT; cy++) {
                for (int cx = 0; cx < CHUNK_COUNT; cx++) {
                    if(chunks[cx][cy].isLoaded)
                    {
                        loadCnt++;
                        //if(onLoad && !IsBoxInFrustum(chunks[cx][cy].box, frustum)){continue;}
                        //if(onLoad && (cx!=closestCX||cy!=closestCY) && !ShouldRenderChunk(chunks[cx][cy].center,camera)){continue;}
                        //TraceLog(LOG_INFO, "drawing chunk: %d,%d", cx, cy);
                        if(chunks[cx][cy].lod == LOD_64) 
                        {
                            chunkBcCount++;
                            chunkTriCount+=chunks[cx][cy].model.meshes[0].triangleCount;
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
                            if(onLoad)//only once we have fully loaded everything
                            {
                                //handle water first
                                for (int w=0; w<chunks[cx][cy].waterCount; w++)
                                {
                                    glEnable(GL_POLYGON_OFFSET_FILL);
                                    glPolygonOffset(-1.0f, -1.0f); // Push water slightly forward in Z-buffer
                                    rlDisableBackfaceCulling();
                                    //rlDisableDepthMask(); 
                                    //BeginBlendMode(BLEND_ALPHA);
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
                                    DrawModel(chunks[cx][cy].water[w], drawPos, 1.0f, (Color){ 0, 100, 253, 232 });
                                    EndShaderMode();
                                    //EndBlendMode();
                                    //rlEnableDepthMask(); 
                                    rlEnableBackfaceCulling();
                                    glDisable(GL_POLYGON_OFFSET_FILL);
                                }
                                if(USE_GPU_INSTANCING) //GPU INSTANCING FOR CLOSE STATIC PROPS
                                {
                                    int counter[MODEL_TOTAL_COUNT] = {0,0};
                                    //- loop through all of the static props that are in the active active tile zone
                                    for(int pInd = 0; pInd<chunks[cx][cy].treeCount; pInd++)
                                    {
                                        //culling
                                        BoundingBox tob = UpdateBoundingBox(treeOrigBox,chunks[cx][cy].props[pInd].pos);
                                        if((!IsTreeInActiveTile(chunks[cx][cy].props[pInd].pos, gx, gy) || USE_TILES_ONLY)
                                            || !IsBoxInFrustum(tob, frustum)){continue;}
                                        //get ready to draw
                                        StaticGameObject *obj = &chunks[cx][cy].props[pInd];
                                        Matrix scaleMatrix = MatrixScale(obj->scale, obj->scale, obj->scale);
                                        Matrix pitchMatrix = MatrixRotateX(obj->pitch);
                                        Matrix yawMatrix   = MatrixRotateY(obj->yaw);
                                        Matrix rollMatrix  = MatrixRotateZ(obj->roll);
                                        Matrix rotationMatrix = MatrixMultiply(MatrixMultiply(pitchMatrix, yawMatrix), rollMatrix);
                                        Matrix transform = MatrixMultiply(scaleMatrix, rotationMatrix);
                                        transform = MatrixMultiply(transform, MatrixTranslate(obj->pos.x, obj->pos.y, obj->pos.z));
                                        HighFiTransforms[chunks[cx][cy].props[pInd].type][counter[chunks[cx][cy].props[pInd].type]] = transform;//well this is kindof insane
                                        counter[chunks[cx][cy].props[pInd].type]++;
                                        if(displayBoxes){DrawBoundingBox(tob,BLUE);}
                                        if(reportOn){treeTriCount+=HighFiStaticObjectModels[chunks[cx][cy].props[pInd].type].meshes[0].triangleCount;}
                                    }
                                    //draw
                                    for(int mt=0; mt<MODEL_TOTAL_COUNT; mt++)
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
                        else if(chunks[cx][cy].lod == LOD_32 && IsBoxInFrustum(chunks[cx][cy].box, frustumChunk8)) {
                            chunkBcCount++;
                            chunkTriCount+=chunks[cx][cy].model32.meshes[0].triangleCount;
                            Matrix mvp = MatrixMultiply(proj, MatrixMultiply(view, chunks[cx][cy].model.transform));
                            SetShaderValueMatrix(heightShaderLight, mvpLocLight, mvp);
                            BeginShaderMode(heightShaderLight);
                            DrawModel(chunks[cx][cy].model32, chunks[cx][cy].position, MAP_SCALE, displayLod?BLUE:WHITE);
                            EndShaderMode();
                            for (int w=0; w<chunks[cx][cy].waterCount; w++)
                            {
                                glEnable(GL_POLYGON_OFFSET_FILL);
                                glPolygonOffset(-1.0f, -1.0f); // Push water slightly forward in Z-buffer
                                rlDisableBackfaceCulling();
                                //rlDisableDepthMask();
                                //BeginBlendMode(BLEND_ALPHA);
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
                                DrawModel(chunks[cx][cy].water[w], drawPos, 1.0f, (Color){ 0, 100, 254, 180 });
                                EndShaderMode();
                                //EndBlendMode();
                                //rlEnableDepthMask();
                                rlEnableBackfaceCulling();
                                glDisable(GL_POLYGON_OFFSET_FILL);
                            }
                        }
                        else if(chunks[cx][cy].lod == LOD_16 && IsBoxInFrustum(chunks[cx][cy].box, frustumChunk8)) {
                            chunkBcCount++;
                            chunkTriCount+=chunks[cx][cy].model16.meshes[0].triangleCount;
                            DrawModel(chunks[cx][cy].model16, chunks[cx][cy].position, MAP_SCALE, displayLod?PURPLE:chunk_16_color);
                        }
                        else if(IsBoxInFrustum(chunks[cx][cy].box, frustumChunk8)||!onLoad) {
                            chunkBcCount++;
                            chunkTriCount+=chunks[cx][cy].model8.meshes[0].triangleCount;
                            DrawModel(chunks[cx][cy].model8, chunks[cx][cy].position, MAP_SCALE, displayLod?RED:chunk_08_color);
                        }
                        if(displayBoxes){DrawBoundingBox(chunks[cx][cy].box,YELLOW);}
                    }
                    else {loadedEem = false;}
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
            //DrawGrid(256, 1.0f);
        EndMode3D();
        DrawText("WASD to move, mouse to look", 10, 10, 20, BLACK);
        DrawText(TextFormat("Pitch: %.2f  Yaw: %.2f", pitch, yaw), 10, 30, 20, BLACK);
        DrawText(TextFormat("Next Chunk: (%d,%d)", chosenX, chosenY), 10, 50, 20, BLACK);
        DrawText(TextFormat("Current Chunk: (%d,%d), Tile: (%d,%d), Global Tile: (%d,%d)", closestCX, closestCY, playerTileX, playerTileY, gx, gy), 10, 70, 20, BLACK);
        DrawText(TextFormat("X: %.2f  Y: %.2f Z: %.2f", camera.position.x, camera.position.y, camera.position.z), 10, 90, 20, BLACK);
        DrawText(TextFormat("Search Type: %s (%d) [t=toggle,r=search]", GetModelName(modelSearchType), modelSearchType), 10, 110, 20, BLACK);
        if(vehicleMode)
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
            if (don.bowMode && (don.state == DONOGAN_STATE_BOW_PULL || don.state == DONOGAN_STATE_BOW_AIM || don.state == DONOGAN_STATE_BOW_REL))
            {
                Vector2 center = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
                DrawCircleLines((int)center.x, (int)center.y, 10, WHITE);
                DrawLine((int)center.x - 12, (int)center.y, (int)center.x + 12, (int)center.y, WHITE);
                DrawLine((int)center.x, (int)center.y - 12, (int)center.x, (int)center.y + 12, WHITE);

                Vector3 spawn = Vector3Add(don.pos, RotYawOffset(don.arrowOffset, don.yawY, 1, false));
                Vector3 dir = DonAimForward(&don, 0.0f);   // match actual shot direction
                float   speed = 46.2;//72.0f;//same speed always
                Vector3 hit = PredictArrowImpact(&don, spawn, dir, speed, 3.0f);

                Vector2 hitSS = GetWorldToScreen(hit, camera);
                DrawCircle((int)hitSS.x, (int)hitSS.y, 4, RED);
            }
        }
        if (showMap) {
            // Map drawing area (scaled by zoom)
            //
            Rectangle dest = {
                SCREEN_WIDTH - (GAME_MAP_SIZE * mapZoom) - 10, //just calculate this x value every time
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
        DrawFPS(10,130);
        EndDrawing();
    }
    quitFileManager = true;
    //VLDReportLeaks();
    UnloadModel(truck);
    UnloadModel(tire);
    //unload skybox
    UnloadTexture(skyTexFront);
    UnloadTexture(skyTexBack);
    UnloadTexture(skyTexLeft);
    UnloadTexture(skyTexRight);
    UnloadTexture(skyTexUp);
    //unload in game map
    UnloadTexture(mapTexture);
    //unload tiles
    free(foundTiles);
    //unload chunks
    for (int cy = 0; cy < CHUNK_COUNT; cy++)
    {
        for (int cx = 0; cx < CHUNK_COUNT; cx++) {
            if(chunks[cx][cy].isLoaded)
            {
                UnloadModel(chunks[cx][cy].model);
                UnloadModel(chunks[cx][cy].model32);
                UnloadModel(chunks[cx][cy].model16);
                UnloadModel(chunks[cx][cy].model8);
                UnloadTexture(chunks[cx][cy].texture);
                UnloadTexture(chunks[cx][cy].textureBig);
                UnloadTexture(chunks[cx][cy].textureFull);
                UnloadTexture(chunks[cx][cy].textureDamn);
                free(chunks[cx][cy].props);
                chunks[cx][cy].props = NULL;
            }
        }
    }

    for (int x = 0; x < CHUNK_COUNT; x++) {
        free(chunks[x]);
    }
    free(chunks);
    chunks = NULL;
    MUTEX_DESTROY(mutex);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
