#include "raylib.h"

typedef struct { // For positions in the grid
    int x;
    int y;
} Vector2_int;

int InitBoard(int board[20][30]) { // To populate the board matrix with 0s
    for (int w=0; w<30; w++) {
        for (int h=0; h<20; h++) {
                board[h][w] = 0;
        }
    }
    return 0;
}

int DrawBoard(int board[20][30], int squareL){ // To draw the board in its current state
    for (int w=0; w<30; w++) {
        int W = squareL * w;
        for (int h=0; h<20; h++) {
            if (board[h][w] == 1) {
                DrawRectangle(W, squareL * h, squareL, squareL, DARKGRAY);
            }
        }
    }
    return 0;
}

int DrawBlankGrid(int squareL) { // To draw the grid of the board
    for (int w=0; w<30; w++) {
        int W = squareL * w;
        for (int h=0; h<20; h++) {
                DrawRectangleLines(W, squareL * h, squareL, squareL, LIGHTGRAY);
        }
    }
    return 0;
}

int ChangeSquare(int board[20][30], int H, int W) { // To change the value of a given position in the board
    for (int w=0; w<30; w++) {
        for (int h=0; h<20; h++) {
            if (h == H && w == W) {
                if (board[h][w] == 0) {
                    board[h][w] = 1;
                } else {
                    board[h][w] = 0;
                }
            }
        }
    }
    return 0;
}

Vector2_int LocateMouseInBoard(Vector2 mousePosition) {
    Vector2_int result = { (int)(mousePosition.x / 90) , (int)(mousePosition.y / 90)};
    return result;
}

int CountNeighbours(int board[20][30], int h, int w) {
    int result = 0;
    int squaresHorizontal = 30;
    int squaresVertical = 20;
    // Upper neighbours
    if (h != 0) {
        result += board[h-1][w]; // Upper
        if (w != 0) {
            result += board[h-1][w-1]; // Upper left
        }
        if (w != squaresHorizontal - 1) {
            result += board[h-1][w+1]; // Upper right
        }
    }
    
    // Side neighbours
    if (w != 0) {
        result += board[h][w-1]; // Left
    }
    if (w != squaresHorizontal - 1) {
        result += board[h][w+1]; // Right
    }

    // Bottom neighbours
    if (h != squaresVertical - 1) {
        result += board[h+1][w]; // Bottom
        if (w != 0) {
            result += board[h+1][w-1]; // Bottom left
        }
        if (w != squaresHorizontal - 1) {
            result += board[h+1][w+1]; // Bottom right
        }
    }
    return result;
}

int ComputeBoard(int board[20][30], int changeBoard[20][30]) {
    InitBoard(changeBoard);
    int num_neigh;
    for (int w=0; w<30; w++) {
        for (int h=0; h<20; h++) {
            num_neigh = CountNeighbours(board, h, w);
            if (board[h][w] == 1) {
                if (num_neigh < 2 || num_neigh > 3) {
                    changeBoard[h][w] = 1;
                }
            } else {
                if (num_neigh == 3) {
                    changeBoard[h][w] = 1;
                }
            } 
        }
    }

    for (int w=0; w<30; w++) {
        for (int h=0; h<20; h++) {
            if (changeBoard[h][w] == 1) {
                if (board[h][w] == 0) {
                    board[h][w] = 1;
                } else {
                    board[h][w] = 0;
                }
            }
        }
    }
    return 0;
}

int main(void){

    // -------------------------------------------
    // CONSTANTS DEFINITION
    // -------------------------------------------

    const int screenWidth = 2700;
    const int screenHeight = 1800;
    const int squareL = 90;

    // -------------------------------------------
    // BOARD VARIABLES DEFINITION
    // -------------------------------------------

    int board[20][30]; // Creation of the board matrix
    InitBoard(board); // Populate the board matrix with 0s

    int changeBoard[20][30]; // Matrix of change (intermediate tool for computing change in the board)

    // -------------------------------------------
    // MOUSE VARIABLES DEFINITION
    // -------------------------------------------

    Vector2 mousePosition = {0.0f, 0.0f}; // Position of the mouse in pixels
    Vector2_int mousePositionInBoard = {0 , 0}; // Position of the mouse in squares

    // -------------------------------------------
    // SETTINGS
    // -------------------------------------------

    int running = 0; // When 0 the game is stopped and the user can change the board, when 1 the game runs
    float time = 0.0f;
    float freq = 0.5f; // Frequency of computing for the board [s]
    InitWindow(screenWidth, screenHeight, "Game Of Life");
    SetTargetFPS(24);

    while (!WindowShouldClose()){

        // -------------------------------------------
        // VARIABLES ARE UPDATED
        // -------------------------------------------

        if (running) {
            time += GetFrameTime(); // Increasing the time passed by the duration of the last frame
            if (time >= freq) {
                ComputeBoard(board, changeBoard);
                time = 0.0f;
            }
        } else {
            mousePosition = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                mousePositionInBoard = LocateMouseInBoard(mousePosition);
                ChangeSquare(board, mousePositionInBoard.y, mousePositionInBoard.x);
                //printf("Mouse Position: < %f , %f >\n", mousePosition.x, mousePosition.y);
                //printf("Square to change: < %d , %d >\n", W, H);
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            if (running == 0) {
                running = 1;
            } else {
                running = 0;
            }
        }


        // -------------------------------------------
        // DRAWING BEGINS
        // -------------------------------------------

        BeginDrawing();

        ClearBackground(BLACK);

        DrawBlankGrid(squareL); // Drawing of the grid

        DrawBoard(board, squareL); // Drawing the board in its current state

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
