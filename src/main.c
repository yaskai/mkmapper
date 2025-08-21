#include <stdlib.h>
#include "raylib.h"
#include "config.h"
#include "sprites.h"
#include "tilemap.h"
#include "cursor.h"
#include "ui.h"

int main() {
	Config conf = (Config){
		.refresh_rate = 60,
		.window_width = 1920,
		.window_height = 1080,
		.window_flags = 0
	};

	ConfRead(&conf, "config.txt");
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	InitWindow(conf.window_width, conf.window_height, "mkmapper");
	SetTargetFPS(conf.refresh_rate);
	HideCursor();

	Camera2D camera = (Camera2D) {
		.target = (Vector2){0, 0},
		.offset = (Vector2){0, 0},
		.rotation = 0,
		.zoom = 1.0f
	};

	Tilemap tilemap;
	TilemapInit(&tilemap, (Coords){127, 64}, &camera);

	Cursor cursor = (Cursor) {
		.flags = 0,
		.tool = 0,
		.selected_key = '1',
		.camera = &camera,
		.tilemap = &tilemap 
	};

	cursor.clipboard = (char*)malloc(tilemap.tile_count);

	Ui gui;	
	GuiInit(&gui, &conf);

	Spritesheet tile_sheet = SpritesheetCreate(LoadTexture("resources/sorted_tileset01.png"), (Vector2){64, 64});

	while(!WindowShouldClose()) {
		float delta_time = GetFrameTime();

		CursorUpdate(&cursor, delta_time);

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode2D(camera);	
		TilemapDraw(&tilemap, &tile_sheet);
		CursorDraw(&cursor);
		EndMode2D();

		TilemapDrawDebugInfo(&tilemap);
		GuiUpdate(&gui, &cursor);

		CursorDrawIcon(&cursor);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

