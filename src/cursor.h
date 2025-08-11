#ifndef CURSOR_H_
#define CURSOR_H_

#include <stdint.h>
#include "raylib.h"
#include "tilemap.h"
#include "gridmath.h"

#define UI_LOCK		 0x01 
#define PAN_MODE	 0x02
#define SELECT_MODE  0x04

enum TOOL {
	PENCIL,
	ERASER,
	SELECT,
};

typedef struct {
	uint8_t flags, tool;

	char selected_key;

	float select_timer;

	Coords box_pos, box_size, box_origin;

	Coords cb_size;

	Coords grid_pos;
	Vector2 screen_pos, world_pos;	

	Camera2D *camera;
	Tilemap *tilemap;
	
	char *clipboard;
} Cursor;

void CursorUpdate(Cursor *cursor, float delta_time);
void CursorCamControls(Cursor *cursor, float delta_time);

void ApplyPencil(Cursor *cursor, uint32_t grid_index, bool erase);

void CursorDrawIcon(Cursor *cursor);
void CursorDraw(Cursor *cursor);

void SelectBox(Cursor *cursor);
void DrawSelectBox(Cursor *cursor);
void FillSelection(Cursor *cursor, bool erase);
void CopySelection(Cursor *cursor);
void PasteSelection(Cursor *cursor);

#endif // !CURSOR_H_
