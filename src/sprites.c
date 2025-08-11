#include <stdlib.h>
#include "raylib.h"
#include "sprites.h"

Spritesheet SpritesheetCreate(Texture2D texture, Vector2 frame_dimensions) {
	Spritesheet spritesheet; 

	spritesheet.cols = texture.width / frame_dimensions.x;
	spritesheet.rows = texture.height / frame_dimensions.y;
	spritesheet.frame_count = spritesheet.cols * spritesheet.rows;

	spritesheet.texture = texture;
	spritesheet.frames = (Rectangle*)malloc(sizeof(Rectangle) * spritesheet.frame_count);

	for(uint8_t r = 0; r < spritesheet.rows; r++) {
		for(uint8_t c = 0; c < spritesheet.cols; c++) {
			spritesheet.frames[c + r * spritesheet.cols] = (Rectangle) {
				.x = c * frame_dimensions.x,
				.y = r * frame_dimensions.y,
				.width = frame_dimensions.x, 
				.height = frame_dimensions.y
			};
		}
	}	

	return spritesheet;
}

void SpritesheetClose(Spritesheet *spritesheet) {
	//UnloadTexture(spritesheet->texture);
	free(spritesheet->frames);
}

void DrawSprite(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position) {
	DrawTextureRec(spritesheet->texture, spritesheet->frames[frame_index], position, WHITE);	
}

uint8_t FrameIndex(Spritesheet *spritesheet, uint8_t c, uint8_t r) {
	return (c + r * spritesheet->cols);
}

