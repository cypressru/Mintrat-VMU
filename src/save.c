#include "save.h"

char *SaveImageDialog(const char *default_name) {
    char command[1024];
    char path[1024];
    FILE *fp;
    snprintf(command, sizeof(command),
             "zenity --file-selection --save --confirm-overwrite "
             "--file-filter='C source files (*.c) | *.c' --filename='%s'",
             default_name);
    fp = popen(command, "r");
    if (fgets(path, sizeof(path) - 1, fp) != NULL) {
        path[strcspn(path, "\n")] = 0;
        pclose(fp);
        return strdup(path);
    }
    pclose(fp);
    return NULL;
}

void SaveImageToArray(const char *filePath, const bool image[MAX_HEIGHT][MAX_WIDTH], int width, int height) {
    FILE *file = fopen(filePath, "w");
    fprintf(file, "static const char VMU_Image = {\n");
    for (int y = 0; y < height; y++) {
        fprintf(file, "    0b");
        for (int x = 0; x < width; x++) {
            fprintf(file, "%d", image[y][x] ? 1 : 0);
        }
        fprintf(file, "%s\n", (y < height - 1) ? "," : "");
    }
    fprintf(file, "};\n\n");
    fclose(file);
}

void SaveImage(const bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows) {
    char *filePath = SaveImageDialog("untitled.c");
    if (filePath == NULL) {
        return;
    }
    SaveImageToArray(filePath, image, numColumns, numRows);
    free(filePath);
}