#ifndef UTIL_H
#define UTIL_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

#define GRAVITY 9.86f //would be nice if this actually works well

#define MAX_CHUNK_DIM 16
#define CHUNK_COUNT 16
#define CHUNK_SIZE 64
#define CHUNK_WORLD_SIZE 1024.0f
#define MAX_WORLD_SIZE (CHUNK_COUNT * CHUNK_WORLD_SIZE)
#define WORLD_WIDTH  MAX_WORLD_SIZE
#define WORLD_HEIGHT MAX_WORLD_SIZE
#define GAME_MAP_SIZE 128
#define MAP_SIZE (CHUNK_COUNT * CHUNK_SIZE)
#define MAX_CHUNKS_TO_QUEUE (CHUNK_COUNT * CHUNK_COUNT)
#define MAP_SCALE 16
#define MAP_VERTICAL_OFFSET 0 //(MAP_SCALE * -64)
#define PLAYER_HEIGHT 1.7f
#define FULL_TREE_DIST 85.42f //112.2f

//chunk tile system
#define TILE_GRID_SIZE 8 //sync with main.c
#define TILE_WORLD_SIZE (CHUNK_WORLD_SIZE / TILE_GRID_SIZE)
#define WORLD_ORIGIN_OFFSET (CHUNK_COUNT / 2 * CHUNK_WORLD_SIZE)
#define MAX_TILES ((CHUNK_WORLD_SIZE * CHUNK_WORLD_SIZE / TILE_GRID_SIZE / TILE_GRID_SIZE))
#define ACTIVE_TILE_GRID_OFFSET 2 //controls the size of the active tile grid, set to 0=1x1, 1=3x3, 2=5x5 etc... (0 may not work?)
#define TILE_GPU_UPLOAD_GRID_DIST 4

//water
#define MAX_WATER_PATCHES_PER_CHUNK 64
#define WATER_Y_OFFSET 20.02f //lets get wet!
#define PLAYER_FLOAT_Y_POSITION 298.75f 
#define WHALE_SURFACE 300.0f 


//movement
#define GOKU_DASH_DIST 512.333f
#define GOKU_DASH_DIST_SHORT 128.2711f
#define MOVE_SPEED 16.16f

//screen
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

//raylib gpu system stuff
#define MAX_MESH_VERTEX_BUFFERS 7

//display/render settings
#define USE_TREE_CUBES false
#define USE_TILES_ONLY false
#define USE_GPU_INSTANCING true //required, never set to false


static float Clampf(float v, float lo, float hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }
static int RandomRange(int lo, int hi)
{
	int r = lo + (rand() % (hi - lo + 1));
	return r;
}

#endif // UTIL_H
