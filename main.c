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
#define CANVAS_WIDTH 480
#define CANVAS_HEIGHT 320

enum {
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

    Scenes = SELECT_RESOLUTION;

    InitWindow(screenWidth, screenHeight, "Mintrat");

    Vector2 mousePos;
    bool mouseDown = false;
    bool savingFile = false;

    bool image[MAX_HEIGHT][MAX_WIDTH] = {0};
    for (int y = 0; y < MAX_HEIGHT; y++) {
        for (int x = 0; x < MAX_WIDTH; x++) {
            image[y][x] = false;
        }
    }

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

    // Define controls variables
    int activeTool = 0;  // 0 = Draw 1 = Erase 2 = Fill
    bool editResolutionWidth = 0;
    bool editResolutionHeight = 0;

    // Define value variables
    int resolutionWidth = MAX_WIDTH;
    int resolutionHeight = MAX_HEIGHT;

    int numRows = 48;
    int numColumns = 32;

    // Define editor controls rectangles
    Rectangle resolutionLayoutRecs[3] = {
        (Rectangle){anchorCenterCenter.x - 140, anchorCenterCenter.y - 20, 100, 30},  // Spinner: Width
        (Rectangle){anchorCenterCenter.x + 40, anchorCenterCenter.y - 20, 100, 30},   // Spinner: Height
        (Rectangle){anchorCenterCenter.x - 90, anchorCenterCenter.y + 40, 160, 30},   // GuiButton: Confirm
    };

    Rectangle editorLayoutRecs[3] = {
        (Rectangle){anchorCenterLeft.x, anchorCenterLeft.y / 2, 40, 24},  // ToggleGroup: Tools
        (Rectangle){anchorTopLeft.x + 60, anchorTopLeft.y - 20, 64, 60},  // LabelButton: Save
        (Rectangle){anchorTopLeft.x, anchorTopLeft.y - 20, 64, 60},       // LabelButton: Load
    };

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (Scenes == SELECT_RESOLUTION) {
            // Resolution selection logic
        } else if (Scenes == EDITOR) {
            // GRAB MOUSE INPUT HERE
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        if (Scenes == SELECT_RESOLUTION) {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            if (GuiSpinner(resolutionLayoutRecs[0], "Width  ", &resolutionWidth, 0, MAX_WIDTH, editResolutionWidth)) editResolutionWidth = !editResolutionWidth;
            if (GuiSpinner(resolutionLayoutRecs[1], "Height  ", &resolutionHeight, 0, MAX_HEIGHT, editResolutionHeight)) editResolutionHeight = !editResolutionHeight;
            if (GuiButton(resolutionLayoutRecs[2], "Confirm")) Scenes = EDITOR;

        } else if (Scenes == EDITOR) {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            mousePos = GetMousePosition();
            mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

            // Draw controls
            GuiToggleGroup(editorLayoutRecs[0], ToggleGroupToolsText, &activeTool);
            if (GuiButton(editorLayoutRecs[1], SaveButtonText)) SaveButton();
            if (GuiButton(editorLayoutRecs[2], LoadButtonText)) LoadButton();
            GuiPanel((Rectangle){CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT}, NULL);

            // DRAW CANVAS HERE
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
