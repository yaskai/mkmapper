#include <string.h>
#include "raylib.h"
#include "raymath.h"

#include "include/raygui.h"
#include "tilemap.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#undef RAYGUI_IMPLEMENTATION
#include "include/styles/style_jungle.h"

#include "ui.h"
#include "cursor.h"

void GuiInit(Ui *gui, Config *conf) {
	GuiLoadStyleJungle();

	gui->flags = 0;

	gui->gui_style = conf->gui_style;

	gui->ww = conf->window_width;
	gui->wh = conf->window_height;

	// -1 for none active
	gui->active_menu = -1;
	
	PanelsInit(gui);
	ToolsInit(gui);
	MenusInit(gui);

	Scroller scrollers[2] = {
		// Horizontal
		(Scroller) {
			.flags = 0, .min = 0, .max = gui->ww, .val = 0,
			.bounds = (Rectangle) { 100, gui->wh - 100, gui->ww - 100, 32 }
		},

		// Vertical
		(Scroller) {
			.flags = 0, .min = 0, .max = gui->wh, .val = 0,
			.bounds = (Rectangle) { 0, 0, 32, gui->wh}
		}
		
	};
	memcpy(gui->scrollers, scrollers, sizeof(scrollers));
}

void GuiUpdate(Ui *gui, Cursor *cursor) {
	gui->flags = 0;
	cursor->flags &= ~UI_LOCK;
	
	// Tool panel
	Camera2D *cam = cursor->camera;	
	Tilemap *map = cursor->tilemap;	

	// Panels
	for(uint8_t i = 0; i < 4; i++) {
		GuiPanel(gui->panel_recs[i], gui->panel_text[i]);
		if(CheckCollisionPointRec(cursor->screen_pos, gui->panel_recs[i])) cursor->flags |= UI_LOCK;
	}

	// Tool buttons
	for(uint8_t i = 0; i < 5; i++) {
		if(GuiButton(gui->tool_recs[i], TextFormat("#%d#", gui->tool_icons[i]))) {
			OnToolClick(i, gui, cursor);
		}
	}

	// Menu buttons
	for(uint8_t i = 0; i < 3; i++) {
		if(GuiButton(gui->menu_recs[i], TextFormat("%s", gui->menu_text[i]))) {
		}
	}

	//if(CheckCollisionPointRec(cursor->screen_pos, bot_rec)) cursor->flags |= UI_LOCK;
	//DrawRectangleRec(bot_rec, DARKBROWN);

	//ScrollerUpdate(&gui->scrollers[0], gui, cursor);
}

void ScrollerUpdate(Scroller *scroller, Ui *gui, Cursor *cursor) {
	scroller->flags &= ~HOVER;
	
	// Background
	//DrawRectangleRec(scroller->bounds, GRAY);
	//DrawRectangleRounded(scroller->bounds, 0.25f, 4, GRAY);
	//DrawOutline(scroller->bounds, 4, DARKGRAY);
	DrawRectangleRec(scroller->bounds, BLACK); 

	if(scroller->bounds.width < scroller->bounds.height) {
		ScrollerUpdateVertical(scroller, cursor);
		return;
	}

	// Logic for horizontal scroll bars
	Camera2D *cam = cursor->camera;
	Tilemap *map = cursor->tilemap;

	// Handle
	float content_size = map->cols * map->tile_size;
	float view_size = gui->ww / cam->zoom;
	float range = content_size - view_size;
	if(range < 1) range = 1; 

	float percent = (cam->target.x - gui->ww / 2) / range;
	percent = Clamp(percent, 0, 1);

	Rectangle handle_rec;
	handle_rec.width  = (view_size / content_size) * scroller->bounds.width; 
	handle_rec.height = scroller->bounds.height;
	handle_rec.x 	  = scroller->bounds.x + percent * (scroller->bounds.width - handle_rec.width);
	handle_rec.y	  = scroller->bounds.y; 

	if(CheckCollisionPointRec(cursor->screen_pos, scroller->bounds) &&
	   IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
	   !CheckCollisionPointRec(cursor->screen_pos, handle_rec)) {

		float handle_x = cursor->screen_pos.x - handle_rec.width * 0.5f;

		float min = scroller->bounds.x;
		float max = scroller->bounds.x + scroller->bounds.width - handle_rec.width;
		handle_x = Clamp(handle_x, min, max);

		float local_percent = (handle_x - scroller->bounds.x) / (scroller->bounds.width - handle_rec.width);
        local_percent = Clamp(local_percent, 0.0f, 1.0f);

        content_size = map->cols * map->tile_size;
        view_size = gui->ww / cam->zoom;
        range = content_size - view_size;

        cam->target.x = local_percent * range + (view_size * 0.5f);
	}

	if(CheckCollisionPointRec(cursor->screen_pos, handle_rec)) {
		scroller->flags |= HOVER;

		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			if((scroller->flags & DRAG) == 0) {
				scroller->drag_offset = cursor->screen_pos.x - handle_rec.x;
				scroller->flags |= DRAG; 
			}
		}
	}

	if(scroller->flags & DRAG) {
		float handle_x = cursor->screen_pos.x - scroller->drag_offset;

		float min = scroller->bounds.x;
		float max = scroller->bounds.x + scroller->bounds.width - handle_rec.width;
		handle_x = Clamp(handle_x, min, max);

		float local_percent = (handle_x - scroller->bounds.x) / (scroller->bounds.width - handle_rec.width);
		local_percent = Clamp(local_percent, 0, 1);

		cam->target.x = local_percent * range + (view_size * 0.5f);
	}

	if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) scroller->flags &= ~DRAG;

	DrawRectangleRec(handle_rec, BEIGE);
}

void ScrollerUpdateVertical(Scroller *scroller, Cursor *cursor) {
	Camera2D *cam = cursor->camera;
	Tilemap *map = cursor->tilemap;
}

void DrawOutline(Rectangle bounds, float thick, Color color) {
	//DrawRectangleLinesEx(bounds, thick, color);
	//DrawRectangleRec((Rectangle){bounds.x, bounds.y + bounds.height - thick * 2, bounds.width, thick * 2}, color);
	DrawRectangleRoundedLinesEx(bounds, 2, 4, thick, color);
}

void PanelsInit(Ui *gui) {
	Rectangle recs[4] = {
		{0, 0, gui->ww, 100}, 
		{0, gui->wh - 100, gui->ww, 100}, 
		{0, 100, 100, gui->wh - 200}, 
		{gui->ww - 100, 100, 100, gui->wh - 200}, 
	};

	char *text[4] = {
		"top",
		"bottom",
		"left",
		"TOOL"
	};

	memcpy(gui->panel_recs, recs, sizeof(recs));
	memcpy(gui->panel_text, text, sizeof(text));
}

void ToolsInit(Ui *gui) {
	Rectangle recs[5];	
	for(short i = 0; i < 5; i++) 
		recs[i] = (Rectangle){gui->panel_recs[3].x + 10, gui->panel_recs[3].y + (60 * (i+1)), 50, 50};
	
	uint8_t icons[5] = {
		ICON_PENCIL_BIG,
		ICON_RUBBER,
		ICON_BOX_DOTS_BIG,
		ICON_UNDO,
		ICON_REDO
	};	

	memcpy(gui->tool_recs, recs, sizeof(recs));
	memcpy(gui->tool_icons, icons, sizeof(icons));
}

void MenusInit(Ui *gui) {
	Rectangle recs[3] = {
		{0, 0, 100, 24},
		{100, 0, 100, 24},
		{200, 0, 100, 24},
	};

	char *text[3] = {
		"FILE",
		"EDIT",
		"HELP"
	};

	memcpy(gui->menu_recs, recs, sizeof(recs));
	memcpy(gui->menu_text, text, sizeof(text));
}

void OnToolClick(uint8_t tool_id, Ui *gui, Cursor *cursor) {
	switch(tool_id) {
		case 0: 
			cursor->tool = PENCIL;
			break;
		case 1: 
			cursor->tool = ERASER;
			break;
		case 2: 
			cursor->tool = SELECT;
			break;
		case 3: 
			UndoAction(&cursor->tilemap->actions[cursor->tilemap->curr_action], cursor->tilemap);break;
		case 4:
			RedoAction(&cursor->tilemap->actions[cursor->tilemap->curr_action+1], cursor->tilemap);
			break;
	}
}

void OnMenuClick(uint8_t menu_id, Ui *gui) {
	gui->active_menu = menu_id;
}

