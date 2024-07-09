#pragma once

#include <raylib.h>

#define MAX_WIDTH 48
#define MAX_HEIGHT 32
#define CANVAS_X 238
#define CANVAS_Y 70
#define TILE_SIZE 11

Color lightMint = {239, 255, 228, 255};

typedef enum Scenes {
    SELECT_RESOLUTION = 0,
    EDITOR = 1
} Scenes;