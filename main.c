#include "src/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "src/raygui.h"
#include "src/MintedRat.h"
#include "src/mintraticon.h"

#include "src/defines.h"
#include "src/save.c"
#include "src/tools.c"

int tileSize = 11;
float mouseWheelMove = 0.0f;

void UpdateCanvasZoom(float zoomFactor, int *canvasWidth, int *canvasHeight, float *cellWidth, float *cellHeight, float *spacing, Rectangle *editorLayoutRecs, int numColumns, int numRows) {
    *canvasWidth = (int)(tileSize * numColumns * zoomFactor);
    *canvasHeight = (int)(tileSize * numRows * zoomFactor);

    *cellWidth = *canvasWidth / (float)numColumns;
    *cellHeight = *canvasHeight / (float)numRows;

    *spacing = (*cellWidth + *cellHeight) / 2;

    // Update the canvas rectangle
    editorLayoutRecs[3].width = *canvasWidth;
    editorLayoutRecs[3].height = *canvasHeight;
}

//----------------------------------------------------------------------------------
// Buttons Functions
//----------------------------------------------------------------------------------
static void SaveButton(const bool image[MAX_HEIGHT][MAX_WIDTH], int numColumns, int numRows) {
    printf("SAVE!\n");
    SaveImage(image, numColumns, numRows);
}

static void LoadButton() {
    printf("LOAD!\n");
}

static void ConfirmButton(Scenes *scene, int numColumns, int numRows, int *canvasWidth, int *canvasHeight, float *cellWidth, float *cellHeight, float *spacing, Rectangle *editorLayoutRecs) {
    *canvasWidth = tileSize * numColumns;
    *canvasHeight = tileSize * numRows;

    *cellWidth = *canvasWidth / numColumns;
    *cellHeight = *canvasHeight / numRows;

    *spacing = (*cellWidth + *cellHeight) / 2;

    editorLayoutRecs[3].width = *canvasWidth;
    editorLayoutRecs[3].height = *canvasHeight;

    *scene = EDITOR;
}
//----------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------------
bool IsOutOfBounds(Vector2 gridMouseCell, int numColumns, int numRows) {
    return gridMouseCell.x < 0 || gridMouseCell.x > numColumns || gridMouseCell.y < 0 || gridMouseCell.y > numRows;
}
//----------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
    //----------------------------------------------------------------------------------
    // Initialization
    //----------------------------------------------------------------------------------

    float zoom = 1.0f;  // For zooming in

    int screenWidth = 800;
    int screenHeight = 480;

    Scenes scene = SELECT_RESOLUTION;

    Vector2 mousePos;
    bool mouseDown = false;

    bool image[MAX_HEIGHT][MAX_WIDTH] = {0};
    bool previewCanvas[MAX_HEIGHT][MAX_WIDTH] = {0};
    bool isDrawingLine = false;

    bool savingFile = false;

    const char *ToggleGroupToolsText = "#22#;#28#;#29#;#192#";
    const char *SaveButtonText = "#05# Save";
    const char *LoadButtonText = "#06# Load";

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

    int activeTool = 0;  // 0 = Draw | 1 = Erase | 2 = Fill | 3 = Line |

    bool editResolutionWidth = 0;
    bool editResolutionHeight = 0;

    int numColumns = 48;  // Width
    int numRows = 32;     // Height

    int canvasWidth = tileSize * numColumns;
    int canvasHeight = tileSize * numRows;

    float cellWidth = canvasWidth / numColumns;
    float cellHeight = canvasHeight / numRows;
    float spacing = (cellWidth + cellHeight) / 2;

    Vector2 gridMouseCell = {0, 0};
    Vector2 dragStartPoint = {0, 0};
    Vector2 dragEndPoint = {0, 0};

    Rectangle resolutionLayoutRecs[3] = {
        (Rectangle){anchorCenterCenter.x - 140, anchorCenterCenter.y - 20, 100, 30},  // Spinner: Width
        (Rectangle){anchorCenterCenter.x + 40, anchorCenterCenter.y - 20, 100, 30},   // Spinner: Height
        (Rectangle){anchorCenterCenter.x - 90, anchorCenterCenter.y + 40, 160, 30},   // GuiButton: Confirm
    };

    Rectangle editorLayoutRecs[6] = {
        (Rectangle){anchorCenterLeft.x - 5, anchorCenterLeft.y * 0.7, 40, 24},                 // ToggleGroup: Tools
        (Rectangle){anchorTopLeft.x + 84, anchorTopLeft.y + 20, 60, 40},                       // LabelButton: Save
        (Rectangle){anchorTopLeft.x + 20, anchorTopLeft.y + 20, 60, 40},                       // LabelButton: Load
        (Rectangle){anchorCanvasTopLeft.x, anchorCanvasTopLeft.y, canvasWidth, canvasHeight},  // GuiGrid, Canvas

        (Rectangle){anchorTopLeft.x + 260, anchorTopLeft.y, 30, 30},  // Increase canvas zoom
        (Rectangle){anchorTopLeft.x + 300, anchorTopLeft.y, 30, 30},  // Decrease canvas zoom

    };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "Mintrat");
    GuiLoadStyleMintedRat();
    SetTargetFPS(240);
    Image mintRatIcon = {0};
    mintRatIcon.format = MINTRATICON_FORMAT;
    mintRatIcon.width = MINTRATICON_WIDTH;
    mintRatIcon.height = MINTRATICON_HEIGHT;
    mintRatIcon.data = MINTRATICON_DATA;
    mintRatIcon.mipmaps = 1;
    Texture2D texMintRat = LoadTextureFromImage(mintRatIcon);
    //--------------------------------------------------------------------------------------//

    //------------------------------------------------------------------------------------
    // Main game loop
    //------------------------------------------------------------------------------------
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        //------------------------------------------------------------------------------------
        // Update
        //------------------------------------------------------------------------------------
        bool mouseButtonPresses[2] = {IsMouseButtonPressed(0), IsMouseButtonPressed(1)};
        bool mouseButtonReleases[2] = {IsMouseButtonReleased(0), IsMouseButtonReleased(1)};
        bool mouseButtonDowns[2] = {IsMouseButtonDown(0), IsMouseButtonDown(1)};

        switch (scene) {
            case SELECT_RESOLUTION:
                break;
            case EDITOR:
                if (IsOutOfBounds(gridMouseCell, numColumns, numRows)) {
                    if (mouseButtonReleases[0] || mouseButtonReleases[1]) {
                        if (!isDrawingLine)
                            break;
                        isDrawingLine = false;
                        DrawBresenhamLine(dragStartPoint.x, dragStartPoint.y, dragEndPoint.x, dragEndPoint.y, image, numColumns, numRows, mouseButtonReleases[0] ? 1 : 0);
                    }
                } else if (mouseButtonPresses[0] || mouseButtonPresses[1]) {
                    if (activeTool != 3)
                        break;
                    dragStartPoint = gridMouseCell;
                    isDrawingLine = true;
                    // Initialize previewCanvas with the current image
                    memcpy(previewCanvas, image, sizeof(image));
                } else if (mouseButtonDowns[0] || mouseButtonDowns[1]) {
                    switch (activeTool) {
                        case 0:
                            image[(int)gridMouseCell.y][(int)gridMouseCell.x] = mouseButtonDowns[0] ? 1 : 0;
                            break;
                        case 1:
                            image[(int)gridMouseCell.y][(int)gridMouseCell.x] = mouseButtonDowns[0] ? 0 : 1;
                            break;
                        case 2:
                            FloodFill(image, (int)gridMouseCell.x, (int)gridMouseCell.y, numColumns, numRows, mouseButtonDowns[0] ? 1 : 0);
                            break;
                        case 3:
                            memcpy(previewCanvas, image, sizeof(image));  // Draw preview line on previewCanvas
                            dragEndPoint = gridMouseCell;
                            DrawBresenhamLine(dragStartPoint.x, dragStartPoint.y, dragEndPoint.x, dragEndPoint.y, previewCanvas, numColumns, numRows, mouseButtonDowns[0] ? 1 : 0);
                            break;
                        default:
                            break;
                    }
                } else if (mouseButtonReleases[0] || mouseButtonReleases[1]) {
                    if (!isDrawingLine)
                        break;
                    isDrawingLine = false;
                    DrawBresenhamLine(dragStartPoint.x, dragStartPoint.y, dragEndPoint.x, dragEndPoint.y, image, numColumns, numRows, mouseButtonReleases[0] ? 1 : 0);
                }

                mouseWheelMove = GetMouseWheelMove();
                if (mouseWheelMove != 0) {
                    zoom *= (1.0f + mouseWheelMove * 0.05f);  // Adjust (this to change zoom level)
                    if (zoom < 0.1f)
                        zoom = 0.1f;
                    if (zoom > 10.0f)
                        zoom = 10.0f;
                    UpdateCanvasZoom(zoom, &canvasWidth, &canvasHeight, &cellWidth, &cellHeight, &spacing, editorLayoutRecs, numColumns, numRows);
                }

                break;
            default:
                break;
        }
        //------------------------------------------------------------------------------------//

        //------------------------------------------------------------------------------------
        // Draw
        //------------------------------------------------------------------------------------
        BeginDrawing();
        switch (scene) {
            case SELECT_RESOLUTION:
                ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

                if (GuiSpinner(resolutionLayoutRecs[0], "Width  ", &numColumns, 1, MAX_WIDTH, editResolutionWidth))
                    editResolutionWidth = !editResolutionWidth;
                if (GuiSpinner(resolutionLayoutRecs[1], "Height  ", &numRows, 1, MAX_HEIGHT, editResolutionHeight))
                    editResolutionHeight = !editResolutionHeight;
                if (GuiButton(resolutionLayoutRecs[2], "Confirm"))
                    ConfirmButton(&scene, numColumns, numRows, &canvasWidth, &canvasHeight, &cellWidth, &cellHeight, &spacing, editorLayoutRecs);
                break;
            case EDITOR:
                ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

                // Draw separator
                DrawLine(anchorTopCenter.x / 2, 0, anchorBottomCenter.x / 2, screenHeight, BLACK);

                // Draw canvas
                for (int i = 0; i < numRows; i++) {
                    for (int j = 0; j < numColumns; j++) {
                        if ((isDrawingLine && previewCanvas[i][j]) || (!isDrawingLine && image[i][j])) {
                            DrawRectangle(CANVAS_X + j * cellWidth, CANVAS_Y + i * cellHeight, cellWidth, cellHeight, BLACK);
                        } else {
                            DrawRectangle(CANVAS_X + j * cellWidth, CANVAS_Y + i * cellHeight, cellWidth, cellHeight, lightMint);
                        }
                    }
                }

                if (GuiButton(editorLayoutRecs[4], "+")) {
                    zoom *= 1.1f;  // Increase zoom
                    UpdateCanvasZoom(zoom, &canvasWidth, &canvasHeight, &cellWidth, &cellHeight, &spacing, editorLayoutRecs, numColumns, numRows);
                }
                if (GuiButton(editorLayoutRecs[5], "-")) {
                    zoom *= 0.9f;  // Decrease zoom
                    UpdateCanvasZoom(zoom, &canvasWidth, &canvasHeight, &cellWidth, &cellHeight, &spacing, editorLayoutRecs, numColumns, numRows);
                }

                // Draw controls
                GuiToggleGroup(editorLayoutRecs[0], ToggleGroupToolsText, &activeTool);  // Tools
                if (GuiButton(editorLayoutRecs[1], SaveButtonText))
                    SaveButton(image, numColumns, numRows);  // Save
                if (GuiButton(editorLayoutRecs[2], LoadButtonText))
                    LoadButton();                                                // Load
                GuiGrid(editorLayoutRecs[3], NULL, spacing, 1, &gridMouseCell);  // Canvas Grid
                DrawEllipse(95, 390, 70, 20, (Color){0, 0, 0, 150});             // Mintrat's shadow
                DrawTextureEx(texMintRat, (Vector2){40, 300}, 0.0f, 2, WHITE);   // Mintrat :)
        }

        EndDrawing();
        //----------------------------------------------------------------------------------//
    }
    //--------------------------------------------------------------------------------------//

    //--------------------------------------------------------------------------------------
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();  // Close window and OpenGL context
    //UnloadTexture(texMintRat);
    //--------------------------------------------------------------------------------------//
    return 0;
}