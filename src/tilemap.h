#ifndef TILEMAP_H_
#define TILEMAP_H_

#include <stdint.h>
#include "raylib.h"
#include "gridmath.h"
#include "sprites.h"

// Adjacency bitmask
#define ADJ_TOP 0x01 
#define ADJ_LFT 0x02
#define ADJ_RGT 0x04
#define ADJ_BOT 0x08

#define CHECK_TOP (Coords){pos.c, pos.r - 1}
#define CHECK_BOT (Coords){pos.c, pos.r + 1}
#define CHECK_LFT (Coords){pos.c - 1, pos.r}
#define CHECK_RGT (Coords){pos.c + 1, pos.r}

typedef struct {
	short dx, dy;
} Dir;

typedef struct {
	uint16_t id;
	Coords position, size, map_size;
	char *prev, *curr;
} Action;

typedef struct {
	uint8_t flags;

	uint8_t tile_size;

	uint32_t action_count, action_cap, curr_action;

	uint32_t cols, rows;
	uint32_t tile_count;

	float tile_size_scaled;

	Camera2D *camera;

	unsigned char *type, *data;

	Action *actions;
} Tilemap;

bool InBounds(Coords coords, Tilemap *tilemap);
Rectangle TileRec(Coords coords, Tilemap *tilemap);
char FetchTileKey(Coords coords, Tilemap *tilemap);

void TilemapInit(Tilemap *tilemap, Coords size, Camera2D *camera);

Action MakeAction(Coords origin, Coords size, Coords map_size, Tilemap *tilemap);
void ApplyAction(Action *action, Tilemap *tilemap);
void UndoAction(Action *action, Tilemap *tilemap);
void RedoAction(Action *action, Tilemap *tilemap);

void TilemapDrawGrid(Tilemap *tilemap);
void TilemapDraw(Tilemap *tilemap, Spritesheet *ss);

void UpdateTileSprites(Tilemap *tilemap);
void DrawBitmaskTile(Coords coords, Spritesheet *ss, Tilemap *tilemap);

void TilemapDrawDebugInfo(Tilemap *tilemap);

void TilePaint(Coords coords, Color color, Tilemap *tilemap);

#endif // !TILEMAP_H_
