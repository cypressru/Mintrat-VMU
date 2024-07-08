#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "MintedRat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_WIDTH 48
#define MAX_HEIGHT 32
#define CANVAS_X 238
#define CANVAS_Y 70
#define TILE_SIZE 11

Color lightMint = {239, 255, 228, 255};

// SAVE

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
    SaveImageToArray(filePath, image, numColumns, numRows);
    free(filePath);
}

// ENUMS

typedef enum Scenes {
    SELECT_RESOLUTION = 0,
    EDITOR = 1
} Scenes;

// TOOLS

void floodFill(bool image[MAX_HEIGHT][MAX_WIDTH], int x, int y, int width, int height, bool color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    if (image[y][x] == color) {
        return;
    }
    image[y][x] = color;

    floodFill(image, x + 1, y, width, height, color);
    floodFill(image, x - 1, y, width, height, color);
    floodFill(image, x, y + 1, width, height, color);
    floodFill(image, x, y - 1, width, height, color);
}

void drawLine(float x0, float y0, float x1, float y1, bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows, bool color) {
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

static void ConfirmButton(Scenes *scene, int numColumns, int numRows, int *canvasWidth, int *canvasHeight, float *cellWidth, float *cellHeight, float *spacing, Rectangle *editorLayoutRecs) {
    *canvasWidth = TILE_SIZE * numColumns;
    *canvasHeight = TILE_SIZE * numRows;

    *cellWidth = *canvasWidth / numColumns;
    *cellHeight = *canvasHeight / numRows;

    *spacing = (*cellWidth + *cellHeight) / 2;

    editorLayoutRecs[3].width = *canvasWidth;
    editorLayoutRecs[3].height = *canvasHeight;

    *scene = EDITOR;
}

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void SaveButton(const bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows);
void SaveImage(const bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows);
static void LoadButton();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
    // Initialization
    //---------------------------------------------------------------------------------------

    int screenWidth = 800;
    int screenHeight = 480;

    int tile_size;

    Scenes scene = SELECT_RESOLUTION;

    InitWindow(screenWidth, screenHeight, "Mintrat");
    // Here ya go
    GuiLoadStyleMintedRat();

    Vector2 mousePos;
    bool mouseDown = false;
    bool savingFile = false;

    bool image[MAX_HEIGHT][MAX_WIDTH] = {0};

    //----------------------------------------------------------------------------------
    // Const text
    const char *ToggleGroupToolsText = "#22#;#28#;#29#;#30#";
    const char *SaveButtonText = "#05# Save";
    const char *LoadButtonText = "#06# Load";

    // Define anchors
    Vector2 anchorTopLeft = {20, 20};
    Vector2 anchorTopCenter = {screenWidth / 2, 20};
    Vector2 anchorTopRight = {screenWidth - 20, 20};

    Vector2 anchorCenterLeft = {20, screenHeight / 2};
    Vector2 anchorCenterCenter = {screenWidth / 2, screenHeight / 2};
    Vector2 anchorCenterRight = {screenWidth - 20, screenHeight / 2};

    Vector2 anchorBottomLeft = {20, screenHeight - 20};
    Vector2 anchorBottomCenter = {screenWidth / 2, screenHeight - 20};
    Vector2 anchorBottomRight = {screenWidth - 20, screenHeight - 20};

    Vector2 anchorCanvasTopLeft = {CANVAS_X, CANVAS_Y};

    // Define controls variables
    int activeTool = 0;  // 0 = Draw | 1 = Erase | 2 = Fill | 3 = Line |

    bool editResolutionWidth = 0;
    bool editResolutionHeight = 0;

    // Define stuff TODO: name this shit lol

    int numColumns = 48;  // Width
    int numRows = 32;     // Height

    int canvasWidth = TILE_SIZE * numColumns;
    int canvasHeight = TILE_SIZE * numRows;

    float cellWidth = canvasWidth / numColumns;
    float cellHeight = canvasHeight / numRows;

    float spacing = (cellWidth + cellHeight) / 2;

    Vector2 gridMouseCell = {0, 0};

    Vector2 dragStartPoint = {0, 0};

    // Define editor controls rectangles
    Rectangle resolutionLayoutRecs[3] = {
        (Rectangle){anchorCenterCenter.x - 140, anchorCenterCenter.y - 20, 100, 30},  // Spinner: Width
        (Rectangle){anchorCenterCenter.x + 40, anchorCenterCenter.y - 20, 100, 30},   // Spinner: Height
        (Rectangle){anchorCenterCenter.x - 90, anchorCenterCenter.y + 40, 160, 30},   // GuiButton: Confirm
    };

    Rectangle editorLayoutRecs[4] = {
        (Rectangle){anchorCenterLeft.x + 20, anchorCenterLeft.y * 0.7, 40, 24},               // ToggleGroup: Tools
        (Rectangle){anchorTopLeft.x + 84, anchorTopLeft.y + 20, 60, 40},                      // LabelButton: Save
        (Rectangle){anchorTopLeft.x + 20, anchorTopLeft.y + 20, 60, 40},                      // LabelButton: Load
        (Rectangle){anchorCanvasTopLeft.x, anchorCanvasTopLeft.y, canvasWidth, canvasHeight}  // GuiGrid, Canvas
    };

    Image mintRat = LoadImage("../mintrat.png");
    Texture2D texMintRat = LoadTextureFromImage(mintRat);

    SetTargetFPS(240);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        // Update

        switch (scene) {
            case SELECT_RESOLUTION:
                break;
            case EDITOR:
                if (IsMouseButtonPressed(0)) {
                    switch(activeTool) {
                        case 3:
                            dragStartPoint = gridMouseCell;
                            break;
                        default:
                            break;
                    }
                }
                if (IsMouseButtonDown(0) && gridMouseCell.x >= 0 && gridMouseCell.x < numColumns && gridMouseCell.y >= 0 && gridMouseCell.y < numRows) {
                    switch (activeTool) {
                        case 0:
                            printf("(%f, %f)\n", gridMouseCell.x, gridMouseCell.y);
                            image[(int)gridMouseCell.y][(int)gridMouseCell.x] = 1;
                            break;
                        case 1:
                            image[(int)gridMouseCell.y][(int)gridMouseCell.x] = 0;
                            break;
                        case 2:
                            if (image[(int)gridMouseCell.y][(int)gridMouseCell.x] == 0) {
                                floodFill(image, (int)gridMouseCell.x, (int)gridMouseCell.y, numColumns, numRows, 1);
                            }
                            break;
                        case 3:
                            drawLine(dragStartPoint.x, dragStartPoint.y, gridMouseCell.x, gridMouseCell.y, image, numColumns, numRows, 1);
                            break;
                        default:
                            break;
                    };
                } else if (IsMouseButtonDown(1) && gridMouseCell.x >= 0 && gridMouseCell.x < numColumns && gridMouseCell.y >= 0 && gridMouseCell.y < numRows) {
                    switch (activeTool) {
                        case 0:
                            image[(int)gridMouseCell.y][(int)gridMouseCell.x] = 0;
                            break;
                        case 1:
                            image[(int)gridMouseCell.y][(int)gridMouseCell.x] = 1;
                            break;
                        case 2:
                            floodFill(image, (int)gridMouseCell.x, (int)gridMouseCell.y, numColumns, numRows, 0);
                            break;
                        default:
                            break;
                    };
                }
                break;
        }

        // Draw
        BeginDrawing();
        if (scene == SELECT_RESOLUTION) {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            if (GuiSpinner(resolutionLayoutRecs[0], "Width  ", &numColumns, 1, MAX_WIDTH, editResolutionWidth)) editResolutionWidth = !editResolutionWidth;
            if (GuiSpinner(resolutionLayoutRecs[1], "Height  ", &numRows, 1, MAX_HEIGHT, editResolutionHeight)) editResolutionHeight = !editResolutionHeight;
            if (GuiButton(resolutionLayoutRecs[2], "Confirm")) ConfirmButton(&scene, numColumns, numRows, &canvasWidth, &canvasHeight, &cellWidth, &cellHeight, &spacing, editorLayoutRecs);

        } else if (scene == EDITOR) {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            // Draw separator
            DrawLine(anchorTopCenter.x / 2, 0, anchorBottomCenter.x / 2, screenHeight, BLACK);
            // Draw canvas
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numColumns; j++) {
                    if (image[i][j]) {
                        DrawRectangle(CANVAS_X + j * cellWidth, CANVAS_Y + i * cellHeight, cellWidth, cellHeight, BLACK);
                    } else {
                        DrawRectangle(CANVAS_X + j * cellWidth, CANVAS_Y + i * cellHeight, cellWidth, cellHeight, lightMint);
                    }
                }
            }

            // Draw controls
            GuiToggleGroup(editorLayoutRecs[0], ToggleGroupToolsText, &activeTool);
            if (GuiButton(editorLayoutRecs[1], SaveButtonText)) SaveButton(image, numColumns, numRows);
            if (GuiButton(editorLayoutRecs[2], LoadButtonText)) LoadButton();
            GuiGrid(editorLayoutRecs[3], NULL, spacing, 1, &gridMouseCell);
            DrawEllipse(95, 390, 70, 20, (Color){0, 0, 0, 150});
            DrawTextureEx(texMintRat, (Vector2){ 40, 300 }, 0.0f, 2, WHITE);
            //DrawTexture(texMintRat, 70, 370, WHITE);
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------

static void SaveButton(const bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows) {
    printf("SAVE!\n");
    SaveImage(image, numColumns, numRows);
}

static void LoadButton() {
    printf("LOAD!\n");
}