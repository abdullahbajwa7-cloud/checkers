#include "raylib.h"
using namespace std;

void inBoard(int board[8][8])
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            board[row][col] = 0;
            if ((row + col) % 2 == 1)
            {
                if (row < 3) board[row][col] = 2;
                else if (row > 4) board[row][col] = 1;
            }
        }
    }
}

bool isValidMove(int board[8][8], int row1, int col1, int row2, int col2, int chance)
{
    if (row1 < 0 || row1 > 7 || col1 < 0 || col1 > 7)
        return false;

    if (row2 < 0 || row2 > 7 || col2 < 0 || col2 > 7)
        return false;

    if (board[row2][col2] != 0)
        return false;

    int piece = board[row1][col1];
    if (piece != chance && piece != chance + 2) // chance + 2 for king 
        return false;

    int drow = row2 - row1; // drow means new row minus old row
    int dcol = col2 - col1; // dcol means new column minus old column
    if (dcol != 1 && dcol != -1)
        return false;

    bool king = (piece > 2);
    if (king)
    {
        if (drow == 1 || drow == -1);
        return true;
        return false;
    }
    if (chance == 1)
        return drow == -1; //for player one to move up
    return drow == 1; //for player two to move down
}

bool isvalidJump(int board[8][8], int row1, int col1, int row2, int col2, int chance)
{
    if (row1 < 0 || row1 > 7 || col1 < 0 || col1 > 7)
        return false;
    if (row2 < 0 || row2 > 7 || col2 < 0 || col2 > 7)
        return false;
    if (board[row2][col2] != 0)
        return false;

    int piece = board[row1][col1];
    if (piece != chance && piece != chance + 2)
        return false;

    int drow = row2 - row1;
    int dcol = col2 - col1;
    if (dcol != 2 && dcol != -2)
        return false;
    if (drow != 2 && drow != -2)
        return false;

    bool isking = (piece > 2);
    if (!isking)
    {
        if (chance == 1 && drow != -2)
            return false;
        if (chance == 2 && drow != 2)
            return false;
    }

    int mid = board[(row1 + row2) / 2][(col1 + col2) / 2];
    if (chance == 1)
        return (mid == 2 || mid == 4);
    return (mid == 1 || mid == 3);
}

void movePiece(int board[8][8], int row1, int col1, int row2, int col2)
{
    board[row2][col2] = board[row1][col1];
    board[row1][col1] = 0;

    if (board[row2][col2] == 1 && row2 == 0)
        board[row2][col2] = 3;
    if (board[row2][col2] == 2 && row2 == 7)
        board[row2][col2] = 4;
}

// GUI PART ---
const int CELL = 80;                 // size of one square in pixels

void drawBoard(int board[8][8], int selRow, int selCol)   // replaces printBoard
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            int x = col * CELL, y = row * CELL;

            DrawRectangle(x, y, CELL, CELL, (row + col) % 2 == 1 ? BROWN : BEIGE);

            int p = board[row][col];
            if (p != 0)
            {
                DrawCircle(x + CELL / 2, y + CELL / 2, 30, (p == 1 || p == 3) ? RED : DARKGRAY);
                if (p > 2)           // king: gold circle in the middle
                    DrawCircle(x + CELL / 2, y + CELL / 2, 12, GOLD);
            }

            if (row == selRow && col == selCol)
                DrawRectangleLinesEx({ (float)x, (float)y, (float)CELL, (float)CELL }, 4, GREEN);
        }
    }
}

int main()
{
    int board[8][8];
    inBoard(board);
    int chance = 1;
    int selRow = -1, selCol = -1;
    int errorTimer = 0;              //  Invalid move  

    InitWindow(8 * CELL, 8 * CELL + 40, "Checkers");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            int col = GetMouseX() / CELL;
            int row = GetMouseY() / CELL;

            if (row >= 0 && row < 8 && col >= 0 && col < 8)
            {
                int piece = board[row][col];
                if (piece == chance || piece == chance + 2)       // select own piece
                {
                    selRow = row;
                    selCol = col;
                }
                else if (selRow != -1)                            // click destination
                {
                    if (isValidMove(board, selRow, selCol, row, col, chance))
                    {
                        movePiece(board, selRow, selCol, row, col);
                        chance = 3 - chance;
                        selRow = selCol = -1;
                    }
                    else if (isvalidJump(board, selRow, selCol, row, col, chance))
                    {
                        board[(selRow + row) / 2][(selCol + col) / 2] = 0;
                        movePiece(board, selRow, selCol, row, col);

                        bool more = isvalidJump(board, row, col, row + 2, col + 2, chance) ||
                            isvalidJump(board, row, col, row + 2, col - 2, chance) ||
                            isvalidJump(board, row, col, row - 2, col + 2, chance) ||
                            isvalidJump(board, row, col, row - 2, col - 2, chance);
                        if (more)
                        {
                            selRow = row;                         // same piece jumps again
                            selCol = col;
                        }
                        else
                        {
                            chance = 3 - chance;
                            selRow = selCol = -1;
                        }
                    }
                    else
                    {
                        errorTimer = 90;                        
                        selRow = selCol = -1;
                    }
                }
            }
        }

        // DRAWING
        BeginDrawing();
        ClearBackground(RAYWHITE);
        drawBoard(board, selRow, selCol);

        if (errorTimer > 0)
        {
            DrawText("Invalid move, try again.", 10, 8 * CELL + 10, 20, MAROON);
            errorTimer--;
        }
        else
            DrawText(chance == 1 ? "Player 1 (Red) turn" : "Player 2 (Black) turn", 10, 8 * CELL + 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}