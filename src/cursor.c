#include <stdint.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "include/raygui.h"
#include "raymath.h"
#include "gridmath.h"
#include "tilemap.h"
#include "cursor.h"

Vector2 pan_down_pos = (Vector2){0, 0};

void CursorCamControls(Cursor *cursor, float delta_time) {
	Camera2D *cam = cursor->camera;

	Vector2 prev = GetScreenToWorld2D(cursor->screen_pos, *cam);

	float scroll = GetMouseWheelMove();
	if(fabs(scroll) > 0) {
		cam->zoom += scroll * 0.25f;
		cam->zoom = Clamp(cam->zoom, 0.4f, 2.0f);

		Vector2 next = GetScreenToWorld2D(cursor->screen_pos, *cam);

		Vector2 diff = Vector2Subtract(prev, next);
		cam->target = Vector2Add(cam->target, diff);
	}

	cursor->world_pos = GetScreenToWorld2D(cursor->screen_pos, *cam);

	if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
		if((cursor->flags & PAN_MODE) == 0) pan_down_pos = cursor->world_pos;
		cursor->flags |= PAN_MODE;

		Vector2 diff = Vector2Subtract(pan_down_pos, cursor->world_pos);
		cam->target = Vector2Add(cam->target, diff);
	} else cursor->flags &= ~PAN_MODE;
}

void CursorUpdate(Cursor *cursor, float delta_time) {
	cursor->screen_pos = GetMousePosition();	

	if((cursor->flags & UI_LOCK)) return;

	CursorCamControls(cursor, delta_time);
		
	if(cursor->world_pos.x >= 0) 
		cursor->grid_pos.c = cursor->world_pos.x / cursor->tilemap->tile_size;

	if(cursor->world_pos.y >= 0) 
		cursor->grid_pos.r = cursor->world_pos.y / cursor->tilemap->tile_size;

	uint32_t grid_index = GridToIndex(cursor->grid_pos, cursor->tilemap->cols);

	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		switch(cursor->tool) {
			case PENCIL:
			case ERASER:
				ApplyPencil(cursor, grid_index, (cursor->tool == ERASER));
				break;
			case SELECT:
				cursor->select_timer += delta_time;
				SelectBox(cursor);
				break;
		}
	}

	if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		if(CmpCoords(cursor->box_size, CoordsOne) || (cursor->select_timer < 0.1f)) {
			cursor->box_size = CoordsZero;
			cursor->flags &= ~SELECT_MODE;
		}

		cursor->select_timer = 0;
	}

	if(IsKeyPressed(KEY_Z)) UndoAction(&cursor->tilemap->actions[cursor->tilemap->curr_action], cursor->tilemap);
	if(IsKeyPressed(KEY_R)) RedoAction(&cursor->tilemap->actions[cursor->tilemap->curr_action+1], cursor->tilemap);

	if((cursor->flags & SELECT_MODE)) {
		if(IsKeyPressed(KEY_F)) FillSelection(cursor, false);
		if(IsKeyPressed(KEY_D)) FillSelection(cursor, true);
		if(IsKeyPressed(KEY_C)) CopySelection(cursor);
	}

	if(IsKeyPressed(KEY_V)) PasteSelection(cursor);
}

void ApplyPencil(Cursor *cursor, uint32_t grid_index, bool erase) {
	char key = (erase) ? '0' : cursor->selected_key;
	if(cursor->tilemap->type[grid_index] == key || !InBounds(cursor->grid_pos, cursor->tilemap)) return;	

	Action action = MakeAction(cursor->grid_pos, (Coords){1, 1}, (Coords){cursor->tilemap->cols, cursor->tilemap->rows}, cursor->tilemap);

	action.prev[0] = FetchTileKey(cursor->grid_pos, cursor->tilemap);
	action.curr[0] = key;

	ApplyAction(&action, cursor->tilemap);
}

void CursorDraw(Cursor *cursor) {
	DrawRectangleRec((Rectangle){cursor->grid_pos.c * 64, cursor->grid_pos.r * 64, 64, 64}, ColorAlpha(BLUE, 0.5f));

	if(cursor->flags & SELECT_MODE) 
		DrawSelectBox(cursor);
}

void CursorDrawIcon(Cursor *cursor) {
	Vector2 pos = Vector2Subtract(cursor->screen_pos, (Vector2){24, 24});

	if(cursor->flags & PAN_MODE) 
		GuiDrawIcon(ICON_CURSOR_HAND, pos.x, pos.y, 3, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED)));
	else if(cursor->flags & UI_LOCK)
		GuiDrawIcon(ICON_CURSOR_CLASSIC, pos.x, pos.y, 3, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED)));
	else 
		switch(cursor->tool) {
			case PENCIL:
				GuiDrawIcon(ICON_PENCIL_BIG, pos.x, pos.y, 3, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED)));
				break;
			case ERASER:
				GuiDrawIcon(ICON_RUBBER, pos.x, pos.y, 3, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED)));
				break;
			case SELECT:
				GuiDrawIcon(ICON_CURSOR_CLASSIC, pos.x, pos.y, 3, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_PRESSED)));
				break;
		}
}

void SelectBox(Cursor *cursor) {
	if((cursor->flags & SELECT_MODE) == 0) {
		cursor->box_origin = cursor->grid_pos, cursor->box_pos= cursor->grid_pos, cursor->box_size = CoordsZero;
		cursor->flags |= SELECT_MODE;
	} else {
		Tilemap *map = cursor->tilemap;

		Coords start = cursor->box_origin;
		Coords end = cursor->grid_pos;

		start = CoordsClamp(start, CoordsZero, (Coords){map->cols - 1, map->rows - 1});
		end = CoordsClamp(end, CoordsZero, (Coords){map->cols - 1, map->rows - 1});

		Coords min = CoordsMin(start, end), max = CoordsMax(start, end);

		cursor->box_pos = min;
		cursor->box_size = CoordsAdd(CoordsSubtract(max, min), (Coords){1, 1});
	}
}

void DrawSelectBox(Cursor *cursor) {
	for(uint32_t r = cursor->box_pos.r; r < cursor->box_pos.r + cursor->box_size.r; r++)
	for(uint32_t c = cursor->box_pos.c; c < cursor->box_pos.c + cursor->box_size.c; c++)
		TilePaint((Coords){c, r}, ColorAlpha(SKYBLUE, 0.5f), cursor->tilemap);
}

void FillSelection(Cursor *cursor, bool erase) {
	Tilemap *map = cursor->tilemap;
	uint32_t tile_count = cursor->box_size.c * cursor->box_size.r;

	Action fill = MakeAction(cursor->box_pos, cursor->box_size, (Coords){map->cols, map->rows}, map);

	char prev_buf[tile_count], curr_buf[tile_count];
		
	for(uint32_t i = 0; i < tile_count; i++) {
		Coords box_coords = IndexToGrid(i, cursor->box_size.c);

		prev_buf[i] = FetchTileKey(CoordsAdd(cursor->box_pos, box_coords), map);
		curr_buf[i] = (erase) ? '0' : cursor->selected_key;
	}
	
	memcpy(fill.prev, prev_buf, sizeof(prev_buf));
	memcpy(fill.curr, curr_buf, sizeof(curr_buf));

	ApplyAction(&fill, map);
}

void CopySelection(Cursor *cursor) {
	Tilemap *map = cursor->tilemap;	
	uint32_t tile_count = cursor->box_size.c * cursor->box_size.r;

	cursor->cb_size = cursor->box_size;
		
	for(uint32_t i = 0; i < tile_count; i++) {
		Coords box_coords = IndexToGrid(i, cursor->box_size.c);
		cursor->clipboard[i] = FetchTileKey(CoordsAdd(cursor->box_pos, box_coords), map);
	}
}

void PasteSelection(Cursor *cursor) {
	Tilemap *map = cursor->tilemap;	
	uint32_t tile_count = cursor->cb_size.c * cursor->cb_size.r;

	Action paste = MakeAction(cursor->grid_pos, cursor->cb_size, (Coords){map->cols, map->rows}, map);

	char prev_buf[tile_count], curr_buf[tile_count];
		
	for(uint32_t i = 0; i < tile_count; i++) {
		Coords box_coords = IndexToGrid(i, cursor->cb_size.c);

		prev_buf[i] = FetchTileKey(CoordsAdd(cursor->grid_pos, box_coords), map);
		curr_buf[i] = cursor->clipboard[i];
	}

	memcpy(paste.prev, prev_buf, sizeof(prev_buf));
	memcpy(paste.curr, curr_buf, sizeof(curr_buf));
	
	ApplyAction(&paste, map);
}

