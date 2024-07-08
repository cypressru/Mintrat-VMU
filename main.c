#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_WIDTH 48
#define MAX_HEIGHT 32
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30
#define BUTTON_PADDING 10
#define CANVAS_X 200
#define CANVAS_Y 40
#define TILE_SIZE 10

typedef enum Scenes {
    SELECT_RESOLUTION = 0,
    EDITOR = 1
} Scenes;

void floodFill(bool image[MAX_HEIGHT][MAX_WIDTH], int x, int y, int width, int height) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    if (image[y][x] == true) {
        return;
    }
    image[y][x] = true;

    floodFill(image, x + 1, y, width, height);
    floodFill(image, x - 1, y, width, height);
    floodFill(image, x, y + 1, width, height);
    floodFill(image, x, y - 1, width, height);
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

static void SaveButton();
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

    Vector2 mousePos;
    bool mouseDown = false;
    bool savingFile = false;

    bool image[MAX_HEIGHT][MAX_WIDTH] = {0};

    //----------------------------------------------------------------------------------
    // Const text
    const char *ToggleGroupToolsText = "#22#;#28#;#29#";
    const char *SaveButtonText = "#05# Save";
    const char *LoadButtonText = "#06# Load";

    // Define anchors
    Vector2 anchorTopLeft = {20, 20};
    Vector2 anchorTopCenter = {screenWidth / 2, 20};
    Vector2 anchorCenterLeft = {20, screenHeight / 2};
    Vector2 anchorCenterCenter = {screenWidth / 2, screenHeight / 2};
    Vector2 anchorCanvasTopLeft = {CANVAS_X, CANVAS_Y};

    // Define controls variables
    int activeTool = 0;  // 0 = Draw 1 = Erase 2 = Fill
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

    // Define editor controls rectangles
    Rectangle resolutionLayoutRecs[3] = {
        (Rectangle){anchorCenterCenter.x - 140, anchorCenterCenter.y - 20, 100, 30},  // Spinner: Width
        (Rectangle){anchorCenterCenter.x + 40, anchorCenterCenter.y - 20, 100, 30},   // Spinner: Height
        (Rectangle){anchorCenterCenter.x - 90, anchorCenterCenter.y + 40, 160, 30},   // GuiButton: Confirm
    };

    Rectangle CanvasScrollView = {anchorCanvasTopLeft.x, anchorCanvasTopLeft.y, canvasWidth, canvasHeight};
    Vector2 CanvasScrollOffset = {10, 10};
    Vector2 CanvasBoundsOffset = {20, 20};

    Rectangle editorLayoutRecs[4] = {
        (Rectangle){anchorCenterLeft.x, anchorCenterLeft.y / 2, 40, 24},                                                                    // ToggleGroup: Tools
        (Rectangle){anchorTopLeft.x + 60, anchorTopLeft.y - 20, 64, 60},                                                                    // LabelButton: Save
        (Rectangle){anchorTopLeft.x, anchorTopLeft.y - 20, 64, 60},                                                                         // LabelButton: Load
        (Rectangle){anchorCanvasTopLeft.x - CanvasBoundsOffset.x, anchorCanvasTopLeft.y - CanvasBoundsOffset.y, canvasWidth, canvasHeight}  // GuiGrid, Canvas
    };

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        // Update
            printf("X: %d Y: %d\n", (int)gridMouseCell.x, (int)gridMouseCell.y);  // TODO: Remove

            if (IsMouseButtonPressed(0) && gridMouseCell.x >= 0 && gridMouseCell.x < numColumns && gridMouseCell.y >= 0 && gridMouseCell.y < numRows) {
                image[(int)gridMouseCell.y][(int)gridMouseCell.x] = 1; // IMPLEMENT TOOLS HERE!
            }

        // Draw
        BeginDrawing();
        if (scene == SELECT_RESOLUTION) {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            if (GuiSpinner(resolutionLayoutRecs[0], "Width  ", &numColumns, 0, MAX_WIDTH, editResolutionWidth)) editResolutionWidth = !editResolutionWidth;
            if (GuiSpinner(resolutionLayoutRecs[1], "Height  ", &numRows, 0, MAX_HEIGHT, editResolutionHeight)) editResolutionHeight = !editResolutionHeight;
            if (GuiButton(resolutionLayoutRecs[2], "Confirm")) ConfirmButton(&scene, numColumns, numRows, &canvasWidth, &canvasHeight, &cellWidth, &cellHeight, &spacing, editorLayoutRecs);

        } else if (scene == EDITOR) {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            // Draw canvas
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numColumns; j++) {
                    if (image[i][j]) {
                        DrawRectangle(CANVAS_X - 20 + j * cellWidth, CANVAS_Y - 20 + i * cellHeight, cellWidth, cellHeight, BLACK);
                    } else {
                        DrawRectangle(CANVAS_X - 20 + j * cellWidth, CANVAS_Y - 20 + i * cellHeight , cellWidth, cellHeight, WHITE);
                    }
                }
            }

            // Draw controls
            GuiToggleGroup(editorLayoutRecs[0], ToggleGroupToolsText, &activeTool);
            if (GuiButton(editorLayoutRecs[1], SaveButtonText)) SaveButton();
            if (GuiButton(editorLayoutRecs[2], LoadButtonText)) LoadButton();
            GuiGrid(editorLayoutRecs[3], NULL, spacing, 1, &gridMouseCell);

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

static void SaveButton() {
    printf("SAVE!\n");
}

static void LoadButton() {
    printf("LOAD!\n");
}