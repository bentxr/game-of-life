#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define MY_BACKGROUND_COLOR 0x111418ff // dark grey
#define MY_TEXT_COLOR 0Xccccccff // white
#define MY_BUTTON_COLOR_NORMAL 0xA6A6A6FF // light grey
#define MY_BUTTON_COLOR_FOCUSED 0xCCCCCCFF // lighter grey
#define MY_BUTTON_COLOR_PRESSED 0x808080FF // grey
#define MY_DEFAULT_TEXT_SIZE 20

#define INPUT_CAP_TEXT_BUFFER 20
#define NUM_FORMAT "%d"

#define MAX_TILES_BUFFER (80*240) 


//----------------------------------------------------------------------------------
// TYPES AND STRUCTURES DEFINITION
//----------------------------------------------------------------------------------

// Generalized coordinates of the system
typedef struct {

    int hTiles; // Number of shown horizontal tiles
    int vTiles; // Number of shown vertical tiles 
    bool grid[MAX_TILES_BUFFER];

} Grid;

typedef struct {

    int screenWidth;
    int screenHeight;

    int drawingFPS; //  frames per second
    int computingFPS; // updates of the board per second

    double time; // time of the simulation
    double frameTimeAccumulator; 

    Color background_color;
    Color text_color;

    int running; // 0 for start menu, 1 for simulation running, 2 for configuration menu
    bool btnStopContinue; // true when Stop/Continue button is pressed
    bool btnRestart; // true when Restart button is pressed
    bool btnSettings; // true when Settings button is pressed
    bool btnGoBackToMenu; // true when Go Back To Menu button is pressed

    bool editMode; // true when on edit mode

    int focus; // int representing the text field that's focused at the moment or -1 if nothing is
    char buffer[INPUT_CAP_TEXT_BUFFER]; // buffer for text input in whichever text field is focused at the moment
    
    int initHTiles;

    bool mutationGrid[MAX_TILES_BUFFER]; // Buffer to store the intermediate mutation information
    Grid stateVector; // Vector with the state of the grid

} State;

//----------------------------------------------------------------------------------
// FUNCTION DECLARATIONS
//----------------------------------------------------------------------------------

// Frame update function (has to consume a void pointer to the state to be compatible with emscripten_set_main_loop_arg)
void UpdateFrame(void *void_state);

// Draws the grid
void DrawCurrentGrid(State *state);

// Draws the simulation
void DrawSimulation(State *state);

// Draws the screen when the simulation is paused and before it starts
void DrawPauseScreen(State *state);

// Draws the settings screen
void DrawSettings(State *state);

// Listen for text input on the text fields in Settings
void SettingsTextFields(State *state);

// Changes the value on the given adress from a character buffer
int ChangeValue(int *value, char buffer[20]);

// Clears the grid to all 0s
void ClearGrid(State *state);

// Resets values to the initial state
void ResetGrid(State *state);

// Counts the activated neighbours around a tile
int CountNeighbours(Grid *grid, int h, int v);

// Computes a step of the system
void ComputeGrid(State *state);

// Locate mouse in grid
int LocateMouseInGrid(State *state, Vector2 mousePosition);

// Calculate vertical number of tiles from the current screen dimensions and number of horizontal tiles
int calculate_vTiles(State *state);

//------------------------------------------------------------------------------------
// PROGRAM MAIN ENTRY POINT
//------------------------------------------------------------------------------------

int main(void){

    //--------------------------------------------------------------------------------------
    // SETUP
    //--------------------------------------------------------------------------------------

    // It's static to avoid stack overflow on WASM (only useful with bigger grids)
    static State state = {

        .screenWidth = 800, //1280,
        .screenHeight = 450, //720,

        .drawingFPS = 60, //  frames per second
        .computingFPS = 2, // updates of the board per second

        .time = 0.0,
        .frameTimeAccumulator = 0.0,
        
        .background_color = {0x11, 0x14, 0x18, 0xff},
        .text_color = {0x91, 0x91, 0x91, 0xff},

        .running = 0,
        .btnStopContinue = false,
        .btnRestart = false,
        .btnSettings = false,
        .btnGoBackToMenu = false,
        .editMode = false,

        .focus = -1,

        .initHTiles = 32, // Number of initial horizontal tiles

        .mutationGrid = {0}, 

        .stateVector = {
            .hTiles = 32,
            .vTiles = 18,
            .grid = {0}
        }

    };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(state.screenWidth, state.screenHeight, "Game of life");
    SetTargetFPS(state.drawingFPS);
    SetExitKey(KEY_NULL);
    
    GuiSetStyle(DEFAULT, TEXT_SIZE, MY_DEFAULT_TEXT_SIZE);
    GuiSetStyle( DEFAULT, TEXT_COLOR_NORMAL, MY_TEXT_COLOR); 
    GuiSetStyle( DEFAULT, BACKGROUND_COLOR, MY_BACKGROUND_COLOR); 

    GuiSetStyle( BUTTON, TEXT_COLOR_NORMAL, MY_BACKGROUND_COLOR); 
    GuiSetStyle( BUTTON, TEXT_COLOR_FOCUSED, MY_BACKGROUND_COLOR); 
    GuiSetStyle( BUTTON, TEXT_COLOR_PRESSED, MY_BACKGROUND_COLOR); 

    GuiSetStyle( BUTTON, BASE_COLOR_NORMAL, MY_BUTTON_COLOR_NORMAL); 
    GuiSetStyle( BUTTON, BASE_COLOR_FOCUSED, MY_BUTTON_COLOR_FOCUSED); 
    GuiSetStyle( BUTTON, BASE_COLOR_PRESSED, MY_BUTTON_COLOR_PRESSED); 

    GuiSetStyle( BUTTON, BORDER_COLOR_NORMAL, MY_BUTTON_COLOR_NORMAL); 
    GuiSetStyle( BUTTON, BORDER_COLOR_FOCUSED, MY_BUTTON_COLOR_FOCUSED); 
    GuiSetStyle( BUTTON, BORDER_COLOR_PRESSED, MY_BUTTON_COLOR_PRESSED); 

    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);

    GuiSetStyle( TEXTBOX, TEXT_COLOR_NORMAL, MY_TEXT_COLOR); 
    GuiSetStyle( TEXTBOX, TEXT_COLOR_FOCUSED, MY_TEXT_COLOR); 
    GuiSetStyle( TEXTBOX, TEXT_COLOR_PRESSED, MY_TEXT_COLOR); 

    GuiSetStyle( TEXTBOX, BASE_COLOR_NORMAL, MY_BUTTON_COLOR_NORMAL); 
    GuiSetStyle( TEXTBOX, BASE_COLOR_FOCUSED, MY_BUTTON_COLOR_FOCUSED); 
    GuiSetStyle( TEXTBOX, BASE_COLOR_PRESSED, MY_BUTTON_COLOR_PRESSED); 

    GuiSetStyle( TEXTBOX, BORDER_COLOR_NORMAL, MY_BUTTON_COLOR_NORMAL); 
    GuiSetStyle( TEXTBOX, BORDER_COLOR_FOCUSED, MY_BUTTON_COLOR_FOCUSED); 
    GuiSetStyle( TEXTBOX, BORDER_COLOR_PRESSED, MY_BUTTON_COLOR_PRESSED); 

    GuiSetIconScale(2);



    //--------------------------------------------------------------------------------------
    // MAIN LOOP
    //--------------------------------------------------------------------------------------

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop_arg(UpdateFrame, &state, state.drawingFPS, 1);
    #else
        SetWindowMinSize(800, 450);
        while (!WindowShouldClose()) UpdateFrame(&state);
    #endif

    //--------------------------------------------------------------------------------------
    // DE-INITIALIZATION
    //--------------------------------------------------------------------------------------

    CloseWindow();

    return 0;
}

//----------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
//----------------------------------------------------------------------------------

void UpdateFrame(void *void_state) {

    // -------------------------------------------
    // SETUP
    // -------------------------------------------

    State *state = (State *)void_state;
    const double dt = 1.0/state->computingFPS;
    int mousePosition; // Index on the grid
    state->screenHeight = GetScreenHeight();
    state->screenWidth = GetScreenWidth();
    state->stateVector.vTiles = calculate_vTiles(state);

    // -------------------------------------------
    // LISTEN FOR PRESSES AND UPDATE VARIABLES
    // -------------------------------------------

    if (state->running < 2) { // Handle common buttons in screen 0 and 1

        if (state->btnStopContinue || IsKeyPressed(KEY_SPACE)) {
            state->running = !state->running;
            state->btnStopContinue = false;
        }

        if (state->btnRestart || IsKeyPressed(KEY_R)) {

            // Restart simulation parameters
            ResetGrid(state);
            state->btnRestart = false;
        }

        if (state->btnSettings || IsKeyPressed(KEY_S)) {
            state->running = 2;
            state->btnSettings = false;
        }

        if (IsKeyPressed(KEY_E)) {
            state->editMode = !state->editMode;
        }

        if (state->editMode && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            mousePosition = LocateMouseInGrid(state, GetMousePosition());
            if (mousePosition >= 0) state->stateVector.grid[mousePosition] = !state->stateVector.grid[mousePosition];
        }
    }

    else if (state->running == 2) {

        if ((state->btnGoBackToMenu || IsKeyPressed(KEY_B)) && state->focus == -1) {
            state->running = 0;
            state->btnGoBackToMenu = false;
        }

    }

    // -------------------------------------------
    // DRAWING BEGINS
    // -------------------------------------------

    switch (state->running) {

        case 0:

            DrawPauseScreen(state);
            break;

        case 1:

            state->frameTimeAccumulator += GetFrameTime();

            // Computes a timestep once the time accumulator reaches dt
            while (state->frameTimeAccumulator >= dt) { 

                ComputeGrid(state);
                state->time += dt;
                state->frameTimeAccumulator -= dt;

            }

            DrawSimulation(state);
            break;

        case 2:

            DrawSettings(state);
            break;
        
        default: break;

    }

}

void DrawCurrentGrid(State *state) {
    Grid *grid = &state->stateVector;
    const int hTiles = state->stateVector.hTiles;
    const int vTiles = state->stateVector.vTiles;
    const int tileSide = state->screenWidth/state->stateVector.hTiles;
    

    for (int h = 0; h < hTiles; h++) {
        for (int v = 0; v < vTiles; v++) {
            if (grid->grid[v*hTiles + h]) {
                DrawRectangle(tileSide * h, tileSide * v, tileSide, tileSide, WHITE);
            }
            DrawRectangleLines(tileSide * h, tileSide * v, tileSide, tileSide, DARKGRAY);
        }
    }
}

void DrawSimulation(State *state) {

    BeginDrawing();
    ClearBackground(state->background_color);

    DrawCurrentGrid(state);

    EndDrawing();

}

void DrawPauseScreen(State *state) {

    const int sw = state->screenWidth;
    const int sh = state->screenHeight;
    const int line = 20;
    
    BeginDrawing();

    ClearBackground(state->background_color);

    DrawCurrentGrid(state);

    if (!state->editMode) {

        // Draw buttons
        const char* textSettings = TextFormat("SETTINGS [S]");
        state->btnSettings = GuiButton((Rectangle){ sw/2 - GetTextWidth(textSettings), sh/10 + 8*MY_DEFAULT_TEXT_SIZE, 2*GetTextWidth(textSettings), 2*MY_DEFAULT_TEXT_SIZE}, textSettings);

        const char* textRestart = TextFormat("RESTART [R]"); 
        state->btnRestart = GuiButton((Rectangle){ sw/2 - GetTextWidth(textSettings), sh/10 + 4*MY_DEFAULT_TEXT_SIZE, 2*GetTextWidth(textSettings), 2*MY_DEFAULT_TEXT_SIZE}, textRestart);
        
        const char* textStopContinue = TextFormat((state->time != 0.0) ? "RESUME [SPACE]" : "START [SPACE]");
        state->btnStopContinue = GuiButton((Rectangle){ sw/2 - GetTextWidth(textSettings), sh/10, 2*GetTextWidth(textSettings), 2*MY_DEFAULT_TEXT_SIZE}, textStopContinue);

        const char* textStepsPerSecond = TextFormat("Steps per second: %d", state->computingFPS);
        const char* textUseButtonsOrKeyShortcuts = TextFormat("Use the BUTTONS or KEY SHORTCUTS");
        DrawRectangle(0, state->screenHeight-7*line, 2*sw/48 + MeasureText(textUseButtonsOrKeyShortcuts, MY_DEFAULT_TEXT_SIZE), 7*line, state->background_color);
        DrawText(textUseButtonsOrKeyShortcuts, sw/48, state->screenHeight - 6*line, 20, RAYWHITE);
        DrawText("Press [E] to enter edit mode", sw/48, state->screenHeight - 4*line, 20, RAYWHITE);
        DrawText(textStepsPerSecond, sw/48, state->screenHeight - 2*line, 20, RAYWHITE);

    } else {

        const char* textClickAnyCell = TextFormat("Click any cell to change its value");
        DrawRectangle(0, state->screenHeight-5*line, 2*sw/48 + MeasureText(textClickAnyCell, MY_DEFAULT_TEXT_SIZE), 7*line, Fade(state->background_color, 0.9f));
        
        DrawText("Press [E] to exit edit mode", sw/48, state->screenHeight - 4*line, 20, RAYWHITE);
        DrawText(textClickAnyCell, sw/48, state->screenHeight - 2*line, 20, RAYWHITE);

    }

    EndDrawing();

}

void DrawSettings(State *state) {
    
    const int sw = state->screenWidth;
    const int sh = state->screenHeight;

    BeginDrawing();

    ClearBackground(state->background_color);

    // Draw text input fields
    SettingsTextFields(state);
    
    // Draw buttons
    const char* textGoBackMenu = TextFormat("BACK [B]");
    state->btnGoBackToMenu = GuiButton((Rectangle){ sw - 2.2*GetTextWidth(textGoBackMenu), sh-2.5*MY_DEFAULT_TEXT_SIZE, 2*GetTextWidth(textGoBackMenu), 2*MY_DEFAULT_TEXT_SIZE}, textGoBackMenu);

    EndDrawing();
}

void SettingsTextFields(State *state) {

    if (state->focus >= 0 && IsKeyPressed(KEY_ESCAPE)) state->focus = -1;

    char tmp[INPUT_CAP_TEXT_BUFFER];
    bool focused;
    int changeResult;
    int tempValue;
    const int sw = state->screenWidth;
    const int sh = state->screenHeight;

    // Bounds for the text input fields in settings
    Rectangle bounds[2] = {

        { sw/2, sh*0.5/10, sw/8, 2*MY_DEFAULT_TEXT_SIZE}, 
        { sw/2, sh*1.5/10, sw/8, 2*MY_DEFAULT_TEXT_SIZE},

    };

    int *editable_values[2] = {

        &state->computingFPS,
        &state->stateVector.hTiles,

    };

    char names[2][40] = {

        "Steps computed per second: ",
        "Number of horizontal tiles: ",

    };

    for (int i = 0; i < 2; i++) {

        focused = state->focus == i;

        if (!focused) { // If not focused the field draws the current value
            snprintf(tmp, sizeof(tmp), NUM_FORMAT, *editable_values[i]);
        }

        if (GuiTextBox(bounds[i], (focused) ? state->buffer : tmp, INPUT_CAP_TEXT_BUFFER, focused)) {
            
            if (focused && IsKeyPressed(KEY_ENTER)) {
                if (i == 1) tempValue = *editable_values[i];
                changeResult = ChangeValue(editable_values[i], state->buffer);
                if (i == 1 && changeResult == 0 && *editable_values[i] != tempValue) ClearGrid(state);
            }
            
            if (!focused) {
                state->focus = i;
                sprintf(state->buffer, "%s", "");
            } else state->focus = -1;

        };

        // First and second column need different padding between label and text box
        DrawText(names[i], bounds[i].x-320, bounds[i].y+bounds[i].height/5, 20, state->text_color);

    }
}

int ChangeValue(int *value, char buffer[20]) {

    // accepting , and .
    for (char *p = buffer; *p; p++) if (*p == ',') *p = '.';

    // pointer magic to convert str to double
    char *end;
    double num = round(strtod(buffer, &end));
    while (*end == ' ') end++;

    // value is updated num is worthy
    if (end != buffer && *end == '\0' && num > 0 && num <= 80) {
        *value = (int)num;
    } else return 1;

    return 0;
}

void ClearGrid(State *state) {

    for (int i = 0; i < MAX_TILES_BUFFER; i++) {
        state->stateVector.grid[i] = false;
    }

    state->time = 0.0f;
    state->frameTimeAccumulator = 0.0f;

}

void ResetGrid(State *state) {

    ClearGrid(state);
    state->stateVector.hTiles = state->initHTiles;

}

int CountNeighbours(Grid *grid, int h, int v) {
    int result = 0;
    int sqH = grid->hTiles;
    int sqV = grid->vTiles;
    // Upper neighbours
    if (v != 0) {
        result += grid->grid[(v-1)*sqH + h]; // Upper
        if (h != 0) {
            result += grid->grid[(v-1)*sqH + (h-1)]; // Upper left
        }
        if (h != sqH - 1) {
            result += grid->grid[(v-1)*sqH + (h+1)]; // Upper right
        }
    }
    
    // Side neighbours
    if (h != 0) {
        result += grid->grid[(v)*sqH + (h-1)]; // Left
    }
    if (h != sqH - 1) {
        result += grid->grid[(v)*sqH + (h+1)]; // Right
    }

    // Bottom neighbours
    if (v != sqV - 1) {
        result += grid->grid[(v+1)*sqH + (h)]; // Bottom
        if (h != 0) {
            result += grid->grid[(v+1)*sqH + (h-1)]; // Bottom left
        }
        if (h != sqH - 1) {
            result += grid->grid[(v+1)*sqH + (h+1)]; // Bottom right
        }
    }
    return result;
}

void ComputeGrid(State *state) {

    Grid *grid = &state->stateVector;
    bool *mutationGridVector = state->mutationGrid;
    const int hTiles = state->stateVector.hTiles;
    const int vTiles = state->stateVector.vTiles;

    // Resets mutation grid
    for (int i = 0; i < hTiles * vTiles; i++) {
        mutationGridVector[i] = false;
    }

    int num_neigh;

    // Pass to define the mutation
    for (int h = 0; h < hTiles; h++) {
        for (int v = 0; v < vTiles; v++) {
            num_neigh = CountNeighbours(grid, h, v);

            if (grid->grid[v*hTiles + h]) {
                if (num_neigh < 2 || num_neigh > 3) mutationGridVector[v*hTiles + h] = 1;
            } else {
                if (num_neigh == 3) mutationGridVector[v*hTiles + h] = 1;
            }
        }
    }

    // Pass to apply the mutation to the grid
    for (int h = 0; h < hTiles; h++) {
        for (int v = 0; v < vTiles; v++) {
            if (mutationGridVector[v*hTiles + h] == 1) grid->grid[v*hTiles + h] = (grid->grid[v*hTiles + h]) ? 0 : 1;
        }
    }
}

int LocateMouseInGrid(State *state, Vector2 mousePosition) {
    const int side = state->screenWidth/state->stateVector.hTiles;
    int coord_h = (int)(mousePosition.x / side);
    int coord_v = (int)(mousePosition.y / side);
    if (coord_h >= state->stateVector.hTiles || coord_v >= state->stateVector.vTiles) return -1; // There's a gap on the right the mouse could click into
    return coord_v * state->stateVector.hTiles + coord_h;
}

int calculate_vTiles(State *state) {
    const int side = state->screenWidth/state->stateVector.hTiles;
    int vTiles = state->screenHeight / side + (state->screenHeight % side != 0);
    int vTilesMax = MAX_TILES_BUFFER / state->stateVector.hTiles;
    return (vTiles > vTilesMax) ? vTilesMax : vTiles;
}