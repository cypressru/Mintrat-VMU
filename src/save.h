#pragma once

#include <raylib.h>
#include <stdio.h>

#include "defines.h"

char *SaveImageDialog(const char *default_name);
void SaveImageToArray(const char *filePath, const bool image[MAX_HEIGHT][MAX_WIDTH], int width, int height);
void SaveImage(const bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows);