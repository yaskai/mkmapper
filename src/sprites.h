#ifndef SPRITES_H_
#define SPRITES_H_

#include <stdint.h>
#include "raylib.h"

typedef struct {
	uint8_t frame_count;
	uint8_t cols, rows;

	uint8_t frame_w, frame_h;
	Rectangle *frames;

	Texture2D texture;
} Spritesheet;

Spritesheet SpritesheetCreate(Texture2D texture, Vector2 frame_dimensions);
void SpritesheetClose(Spritesheet *spritesheet);
void DrawSprite(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position);
uint8_t FrameIndex(Spritesheet *spritesheet, uint8_t c, uint8_t r);

#endif // !SPRITES_H_
