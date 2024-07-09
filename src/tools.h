#pragma once

#include <raylib.h>

#include "defines.h"

void FloodFill(bool image[MAX_HEIGHT][MAX_WIDTH], int x, int y, int width, int height, bool color);
void DrawBresenhamLine(float x0, float y0, float x1, float y1, bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows, bool color);