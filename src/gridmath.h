#ifndef GRIDMATH_H_
#define GRIDMATH_H_

#include <stdbool.h>
#include <stdint.h>
#include "raylib.h"

#define CoordsZero (Coords){0, 0}
#define CoordsOne (Coords){1, 1}

typedef struct { uint32_t c, r; } Coords;

uint32_t GridToIndex(Coords coords, uint32_t width); 
Coords IndexToGrid(uint32_t id, uint32_t width);

Coords CoordsAdd(Coords a, Coords b);
Coords CoordsSubtract(Coords a, Coords b);
Coords CoordsClamp(Coords coords, Coords min, Coords max);

Coords CoordsMin(Coords a, Coords b);
Coords CoordsMax(Coords a, Coords b);

bool CmpCoords(Coords a, Coords b);

Vector2 GridToVec(Coords coords, float tile_size);

#endif // !GRIDMATH_H_
