#include <stdbool.h>
#include <stdint.h>
#include "raylib.h"
#include "gridmath.h"

uint32_t GridToIndex(Coords coords, uint32_t width) {
	return (coords.c + coords.r * width);
}

Coords IndexToGrid(uint32_t id, uint32_t width) {
	return (Coords) {
		.c = id % width,
		.r = id / width
	};
}

Coords CoordsAdd(Coords a, Coords b) {
	return (Coords) {
		a.c + b.c, 
		a.r + b.r
	};
}

Coords CoordsSubtract(Coords a, Coords b) {
	return (Coords) {
		a.c - b.c, 
		a.r - b.r
	};
}

Coords CoordsClamp(Coords coords, Coords min, Coords max) {
	Coords clamped = coords;

	if(coords.c < min.c) clamped.c = min.c;
	if(coords.c > max.c) clamped.c = max.c;
	if(coords.r < min.r) clamped.r = min.r;
	if(coords.r > max.r) clamped.r = max.r;

	return clamped;
}

Coords CoordsMin(Coords a, Coords b) {
	return (Coords) {
		.c = (a.c < b.c) ? a.c : b.c,
		.r = (a.r < b.r) ? a.r : b.r
	};
}

Coords CoordsMax(Coords a, Coords b) {
	return (Coords) {
		.c = (a.c > b.c) ? a.c : b.c,
		.r = (a.r > b.r) ? a.r : b.r
	};
}

bool CmpCoords(Coords a, Coords b) {
	return (a.c == b.c && a.r == b.r);
}

Vector2 GridToVec(Coords coords, float tile_size) {
	return (Vector2){coords.c * tile_size, coords.r * tile_size};
}

