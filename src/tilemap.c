#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "raylib.h"
#include "tilemap.h"
#include "gridmath.h"
#include "sprites.h"
#include "cursor.h"

const Dir dirs[] = {
	{  0, -1 }, 	// North	
	{ -1,  0 },		// East 
	{  1,  0 },		// West 
	{  0,  1 }		// South 
};

bool InBounds(Coords coords, Tilemap *tilemap) {
	return (coords.c >= 0 && coords.r >= 0 && coords.c < tilemap->cols && coords.r < tilemap->rows);
}

Rectangle TileRec(Coords coords, Tilemap *tilemap) {
	return (Rectangle) {
		.x = coords.c * tilemap->tile_size,
		.y = coords.r * tilemap->tile_size,
		.width = tilemap->tile_size,
		.height = tilemap->tile_size
	};
}

char FetchTileKey(Coords coords, Tilemap *tilemap) {
	if(InBounds(coords, tilemap)) {
		return tilemap->type[GridToIndex(coords, tilemap->cols)];	
	} else return '0';
}

void TilemapInit(Tilemap *tilemap, Coords size, Camera2D *camera) {
	tilemap->camera = camera;
	tilemap->tile_size = 64;

	tilemap->cols = size.c;
	tilemap->rows = size.r;
	tilemap->tile_count = size.c * size.r;

	tilemap->type = (unsigned char*)malloc(tilemap->tile_count);
	memset(tilemap->type, '0', tilemap->tile_count);

	tilemap->data = (unsigned char*)malloc(tilemap->tile_count);
	memset(tilemap->data, 0, tilemap->tile_count);

	tilemap->debug = (unsigned char*)malloc(tilemap->tile_count);
	memset(tilemap->debug, 0, tilemap->tile_count);

	tilemap->action_count = 0, tilemap->curr_action = 0, tilemap->action_cap = 2;
	tilemap->actions = (Action*)malloc(sizeof(Action) * tilemap->action_cap); 
}

Action MakeAction(Coords origin, Coords size, Coords map_size, Tilemap *tilemap) {
	return (Action) {
		.position = origin, 
		.size = size,
		.map_size = map_size,
		.prev = (char*)malloc(size.c * size.r),
		.curr = (char*)malloc(size.c * size.r)
	};
}

void ApplyAction(Action *action, Tilemap *tilemap) {
	action->id = tilemap->curr_action++;
	tilemap->action_count++;
	
	if(tilemap->action_count > tilemap->action_cap - 1) {
		tilemap->action_cap *= 2;

		Action *new_ptr = (Action*)realloc(tilemap->actions, sizeof(Action) * tilemap->action_cap);	
		tilemap->actions = new_ptr;
	}

	if(tilemap->curr_action < tilemap->action_count) {
		for(int i = tilemap->curr_action; i < tilemap->action_count; i++) {
			free(tilemap->actions[i].prev);
			free(tilemap->actions[i].curr);
		}

		tilemap->action_count = tilemap->curr_action;
	}
	
	for(uint32_t i = 0; i < (action->size.c * action->size.r); i++) {
		Coords coords = IndexToGrid(i, action->size.c);
		uint32_t tile_index = GridToIndex(CoordsAdd(action->position, coords), tilemap->cols);
		
		if(InBounds(coords, tilemap)) tilemap->type[tile_index] = action->curr[i]; 
	}

	tilemap->actions[tilemap->curr_action] = *action;
	//UpdateTileSprites(tilemap);
	UpdateTileSpritesRec(action->position, action->size, tilemap);
}

void UndoAction(Action *action, Tilemap *tilemap) {
	if(tilemap->curr_action < 1) return;

	for(uint32_t i = 0; i < (action->size.c * action->size.r); i++) {
		Coords coords = IndexToGrid(i, action->size.c);

		action->curr[i] = tilemap->type[GridToIndex(CoordsAdd(action->position, coords), tilemap->cols)];
		tilemap->type[GridToIndex(CoordsAdd(action->position, coords), tilemap->cols)] = action->prev[i];
	}

	tilemap->curr_action--;
	//UpdateTileSprites(tilemap);
	UpdateTileSpritesRec(action->position, action->size, tilemap);
}

void RedoAction(Action *action, Tilemap *tilemap) {
	if(tilemap->curr_action > tilemap->action_count - 1) return;

	for(uint32_t i = 0; i < (action->size.c * action->size.r); i++) {
		Coords coords = IndexToGrid(i, action->size.c);

		action->prev[i] = tilemap->type[GridToIndex(CoordsAdd(action->position, coords), tilemap->cols)];
		tilemap->type[GridToIndex(CoordsAdd(action->position, coords), tilemap->cols)] = action->curr[i];
	}

	tilemap->curr_action++;
	//UpdateTileSprites(tilemap);
	UpdateTileSpritesRec(action->position, action->size, tilemap);
}

void TilemapDrawGrid(Tilemap *tilemap) {
	for(uint32_t i = 0; i < tilemap->tile_count; i++) {
		uint16_t c = i % tilemap->cols;
		uint16_t r = i / tilemap->cols;

		float tile_size = tilemap->tile_size;
		DrawRectangleLines(c * tile_size, r * tile_size, tile_size, tile_size, RAYWHITE);
	}
}

void TilemapDraw(Tilemap *tilemap, Spritesheet *ss) {
	TilemapDrawGrid(tilemap);

	for(uint32_t i = 0; i < tilemap->tile_count; i++) {
		switch(tilemap->type[i]) {
			case '1': DrawBitmaskTile(IndexToGrid(i, tilemap->cols), ss, tilemap);
		}

		/*
		if(tilemap->debug[i] != 0) TilePaint(IndexToGrid(i, tilemap->cols), ColorAlpha(ORANGE, 0.5f), tilemap);
		Coords pos = IndexToGrid(i, tilemap->cols);
		DrawText(TextFormat("%d", tilemap->data[i]), pos.c * tilemap->tile_size, pos.r * tilemap->tile_size, 10, RAYWHITE);
		*/
	}
}

uint8_t TileGetAdj(Coords pos, Tilemap *tilemap) {
	uint8_t adj = 0;

	for(uint8_t d = 0; d < 4; d++) {
		Coords check = {pos.c + dirs[d].dx, pos.r + dirs[d].dy};
		if(!InBounds(check, tilemap)) continue;

		uint32_t ni = GridToIndex(check, tilemap->cols);
		if(tilemap->type[ni] == '1') adj |= (1 << d);
	}

	return adj;
}

void UpdateTileSprites(Tilemap *tilemap) {
	for(uint32_t i = 0; i < tilemap->tile_count; i++) {
		uint8_t adj = 0;	
		Coords pos = IndexToGrid(i, tilemap->cols);
		
		for(uint8_t d = 0; d < 4; d++) {
			Coords check = { pos.c + dirs[d].dx, pos.r + dirs[d].dy };
			if(!InBounds(check, tilemap)) continue;	

			uint32_t ni = GridToIndex(check, tilemap->cols);
			if(tilemap->type[ni] == '1') adj |= (1 << d);
		}

		tilemap->data[i] = adj;
	}
}

void UpdateTileSpritesRec(Coords pos, Coords size, Tilemap *tilemap) {
	uint32_t start_col = pos.c - 1, start_row = pos.r - 1;
	if(start_col > pos.c) start_col = 0; 
	if(start_row > pos.r) start_row = 0;
	
	uint32_t end_col = pos.c + size.c + 1, end_row = pos.r + size.r + 1;
	if(end_col > tilemap->cols) end_col = tilemap->cols;
	if(end_row > tilemap->rows) end_row = tilemap->rows;

	for(uint32_t r = start_row; r < end_row; r++) {
		for(uint32_t c = start_col; c < end_col; c++) {
			Coords coords = {c, r};
			uint32_t id = GridToIndex(coords, tilemap->cols);

			if(tilemap->type[id] != '1') continue;
			tilemap->data[id] = TileGetAdj(coords, tilemap);
		}
	}
}

void DrawBitmaskTile(Coords coords, Spritesheet *ss, Tilemap *tilemap) {
	DrawSprite(ss, tilemap->data[GridToIndex(coords, tilemap->cols)], (Vector2){coords.c * tilemap->tile_size, coords.r * tilemap->tile_size});
}

void TilemapDrawDebugInfo(Tilemap *tilemap) {
	DrawText(TextFormat("action count: %d", tilemap->action_count), 16, 32, 16, GREEN);	
	DrawText(TextFormat("curr action: %d", tilemap->curr_action), 16, 64, 16, GREEN);	
}

void TilePaint(Coords coords, Color color, Tilemap *tilemap) {
	DrawRectangleRec(TileRec(coords, tilemap), color);
}

