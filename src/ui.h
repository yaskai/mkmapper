#ifndef UI_H_
#define UI_H_

#include <stdint.h>
#include "raylib.h"
#include "cursor.h"
#include "config.h"

#define FILE_DIAG 0x01 
#define SCROLL_X  0x02

#define DD_COLS 3 
#define DD_ROWS 8

enum TOOL_RECTS {
	PENCIL_REC,
	ERASER_REC,
	SELECT_REC
};

enum MENU_ID {
	MENU_FILE,
	MENU_EDIT,
	MENU_HELP
};

#define ACTIVE	0x01
#define PRESS	0x02
#define HOVER 	0x04
#define DRAG  	0x08

typedef struct {
	uint8_t flags;

	float val, drag_offset;
	float min, max;
	
	Rectangle bounds;	
} Scroller;

typedef struct {
	uint8_t flags, gui_style;

	float ww, wh;

	Rectangle panel_recs[4];
	char *panel_text[4];

	Rectangle tool_recs[5];
	uint8_t tool_icons[5];

	short active_dd;
	uint8_t dd_opt_count[3];
	Rectangle dd_recs[3];
	char *dd_titles[3][8];

	Scroller scrollers[2];
} Ui;

void GuiInit(Ui *gui, Config *conf);
void GuiUpdate(Ui *gui, Cursor *cursor);

void ScrollerUpdate(Scroller *scroller, Ui *gui, Cursor *cursor);
void ScrollerUpdateVertical(Scroller *scroller, Cursor *cursor);

void DrawOutline(Rectangle bounds, float thick, Color color);

void PanelsInit(Ui *gui);
void ToolsInit(Ui *gui);
void DropdownsInit(Ui *gui);

void OnToolClick(uint8_t tool_id, Ui *gui, Cursor *cursor);
void OnDropdownClick(char *title, Ui *gui, Cursor *cursor);

#endif // !UI_H_
