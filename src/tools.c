#include "tools.h"

void FloodFill(bool image[MAX_HEIGHT][MAX_WIDTH], int x, int y, int width, int height, bool color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    if (image[y][x] == color) {
        return;
    }
    image[y][x] = color;

    FloodFill(image, x + 1, y, width, height, color);
    FloodFill(image, x - 1, y, width, height, color);
    FloodFill(image, x, y + 1, width, height, color);
    FloodFill(image, x, y - 1, width, height, color);
}

void DrawBresenhamLine(float x0, float y0, float x1, float y1, bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows, bool color) {
    float dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    float dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    float err = dx + dy, e2;

    while (true) {
        if (x0 >= 0 && x0 < numColumns && y0 >= 0 && y0 < numRows) {
            image[(int)y0][(int)x0] = color;
        }

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}