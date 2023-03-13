/*
* ASCII Pong
*    Inspired by "Code-It-Yourself! Tetris - Programming from Scratch (Quick and Simple C++)": https://youtu.be/8OK8_tHeCIA
*/

#include <Windows.h>
#include <chrono>
#include <thread>
#include "main.h"

const int SCREEN_WIDTH = 120;    // Console Screen Size X (columns)
const int SCREEN_HEIGHT = 30;    // Console Screen Size Y (rows)

//Since we only use one character to display the score of each player, the winning score should be smaller than 10
const unsigned int WINNING_SCORE = 9;
const unsigned int SCORE_X_POSITION = 15;
const unsigned int SCORE_Y_POSITION = 2; 
unsigned int player1Score = 0;
unsigned int player2Score = 0;
//WINNER_X_POSITION is measured from the left border for player 1 and from the right border for player 2
const unsigned int WINNER_X_POSITION = SCREEN_WIDTH / 4;
const unsigned int WINNER_Y_POSITION = SCREEN_HEIGHT / 2;
const unsigned int FINAL_GAMEOVER_PAUSE = 5000; //milliseconds

const unsigned int BALL_SPEED = 5;

const unsigned int PADDLE_DISTANCE_FROM_SIDE_BORDERS = 2;
const unsigned int PADDLE_HEIGHT = 8;

//Paddle one XY position
unsigned int paddle1XPos = PADDLE_DISTANCE_FROM_SIDE_BORDERS;
unsigned int paddle1YPos = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;

//Paddle two XY position
unsigned int paddle2XPos = (SCREEN_WIDTH - 1) - PADDLE_DISTANCE_FROM_SIDE_BORDERS;
unsigned int paddle2YPos = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;

int main()
{
    /*
    * Initialize data structures.
    */

    //Ball XY position
    unsigned int ballXPos = SCREEN_WIDTH / 2;
    unsigned int ballYPos = SCREEN_HEIGHT / 2;

    //Ball XY direction
    int ballXSpeed = 1; //go right
    int ballYSpeed = 1; //go down

    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];

    // Main Loop
    bool bGameOver = false;
    while (!bGameOver)
    {
        // Draw a blank field of size nScreenWidth * nScreenHeight
        drawBlankField(screen);

        //Draw left, center and right vertical borders
        drawVerticalBars(screen);

        //Draw horizontal lines at the top and bottom of the screen
        drawHorizontalBorders(screen);

        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(BALL_SPEED)); // Small Step = 1 Game Tick

        // Read user input (pressing W/S or O/K keys)
        wchar_t controls[] = { L'W', L'S', L'O', L'K' };
        bool bKey[sizeof(controls) / sizeof(wchar_t)];
        for (int k = 0; k < sizeof(bKey) / sizeof(bool); k++)
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)(controls[k]))) != 0;

        // Handle paddle one movement
        handlePaddleOneMovement(screen, bKey);

        // Draw paddle one
        drawPaddleOne(screen);

        // Handle paddle two movement
        handlePaddleTwoMovement(screen, bKey);

        // Draw paddle two
        drawPaddleTwo(screen);

        // Reposition Ball vertically
        repositionBallVertically(ballYPos, ballYSpeed);

        // Reposition Ball horizontally
        repositionBallHorizontally(ballYPos, ballXPos, ballXSpeed);

        // Draw Player 1 and Player 2 Scores
        printPlayerScores(screen);

        //Draw ball
        screen[ballXPos + ballYPos * SCREEN_WIDTH] = L'O';

        checkWinningConditions(screen, bGameOver);

        // Display Frame
        WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);
    }

    // Exiting
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(FINAL_GAMEOVER_PAUSE));
    CloseHandle(hConsole);
    return 0;
}

void drawBlankField(wchar_t* screen)
{
    for (size_t index = 0; index < SCREEN_HEIGHT * SCREEN_WIDTH; index++)
    {
        screen[index] = L' ';
    }
}

void drawVerticalBars(wchar_t* screen)
{
    for (size_t index = 0; index < SCREEN_HEIGHT * SCREEN_WIDTH; index += SCREEN_WIDTH)
    {
        screen[index] = L'|';
    }

    for (size_t index = SCREEN_WIDTH / 2; index < SCREEN_HEIGHT * SCREEN_WIDTH; index += SCREEN_WIDTH)
    {
        screen[index] = L'.';
    }

    for (size_t index = SCREEN_WIDTH - 1; index < SCREEN_HEIGHT * SCREEN_WIDTH; index += SCREEN_WIDTH)
    {
        screen[index] = L'|';
    }
}

void drawHorizontalBorders(wchar_t* screen)
{
    for (size_t index = 0; index < SCREEN_WIDTH; index++)
    {
        screen[index] = L'=';
    }

    for (size_t index = SCREEN_HEIGHT * SCREEN_WIDTH - SCREEN_WIDTH; index < SCREEN_HEIGHT * SCREEN_WIDTH; index++)
    {
        screen[index] = L'=';
    }
}
void handlePaddleOneMovement(wchar_t* screen, const bool bKey[4])
{
    if (bKey[0]) // Player 1 pressed W to go up
    {
        //paddle1YPos is more than 1, i.e. 2 or more.
        if (paddle1YPos > 1)
        {
            paddle1YPos--;
        }
    }
    else if (bKey[1]) // Player 1 pressed S to go down
    {
        //paddle1YPos is less than (SCREEN_HEIGHT-1)-8, i.e. 20 or less.
        if (paddle1YPos < (SCREEN_HEIGHT - 1) - PADDLE_HEIGHT)
        {
            paddle1YPos++;
        }
    }
}

void handlePaddleTwoMovement(wchar_t* screen, const bool bKey[4])
{
    if (bKey[2]) // Player 2 pressed O to go up
    {
        if (paddle2YPos > 1)
        {
            paddle2YPos--;
        }
    }
    else if (bKey[3]) // Player 2 pressed K to go down
    {
        if (paddle2YPos < SCREEN_HEIGHT - 1 - PADDLE_HEIGHT)
        {
            paddle2YPos++;
        }
    }
}

void drawPaddleOne(wchar_t* screen)
{
    for (size_t index = 0; index < PADDLE_HEIGHT; index++)
    {
        screen[(paddle1YPos + index) * SCREEN_WIDTH + paddle1XPos] = L'>';
    }
}

void drawPaddleTwo(wchar_t* screen)
{
    for (size_t index = 0; index < PADDLE_HEIGHT; index++)
    {
        screen[(paddle2YPos + index) * SCREEN_WIDTH + paddle2XPos] = L'<';
    }
}

void repositionBallVertically(unsigned int& ballYPos, int& ballYSpeed)
{
    if (ballYSpeed > 0) //ball going down
    {
        if (ballYPos >= (SCREEN_HEIGHT - 1) - 1)
        {
            ballYSpeed *= -1;
        }
        ballYPos += ballYSpeed;
    }
    else if (ballYSpeed < 0) //ball going up
    {
        if (ballYPos <= 1)
        {
            ballYSpeed *= -1;
        }
        ballYPos += ballYSpeed;
    }
}

void repositionBallHorizontally(const unsigned int ballYPos, unsigned int& ballXPos, int& ballXSpeed)
{
    if (ballXSpeed > 0) //ball going right
    {
        if (ballXPos == paddle2XPos - 1 && ballYPos >= paddle2YPos && ballYPos <= paddle2YPos + PADDLE_HEIGHT)
        {
            ballXSpeed *= -1;
        }
        else if (ballXPos >= SCREEN_WIDTH - 1)
        {
            ballXSpeed *= -1;
            player1Score++;
        }
        ballXPos += ballXSpeed;
    }
    else if (ballXSpeed < 0) //ball going left
    {
        if (ballXPos == paddle1XPos + 1 && ballYPos >= paddle1YPos && ballYPos <= paddle1YPos + PADDLE_HEIGHT)
        {
            ballXSpeed *= -1;
        }
        else if (ballXPos <= 0)
        {
            ballXSpeed *= -1;
            player2Score++;
        }
        ballXPos += ballXSpeed;
    }
}

void printPlayerScores(wchar_t* screen)
{
    screen[SCORE_Y_POSITION * SCREEN_WIDTH + SCORE_X_POSITION] = (wchar_t)(L'0' + player1Score);
    screen[SCORE_Y_POSITION * SCREEN_WIDTH + SCREEN_WIDTH - SCORE_X_POSITION] = (wchar_t)(L'0' + player2Score);
}

void checkWinningConditions(wchar_t* screen, bool& bGameOver)
{
    if (player1Score < WINNING_SCORE && player2Score < WINNING_SCORE)
    {
        return;
    }

    bGameOver = true;
    if (player1Score >= WINNING_SCORE)
    {
        const wchar_t player1WonText[] = { L'P', L'1', L' ', L'W', L'O', L'N', L'!' };
        printPlayerOneWon(screen, player1WonText, sizeof(player1WonText) / sizeof(wchar_t));
    }
    else if (player2Score >= WINNING_SCORE)
    {
        const wchar_t player2WonText[] = { L'P', L'2', L' ', L'W', L'O', L'N', L'!' };
        printPlayerTwoWon(screen, player2WonText, sizeof(player2WonText) / sizeof(wchar_t));
    }
}

void printPlayerOneWon(wchar_t* screen, const wchar_t* text, const unsigned int textLength)
{
    for (size_t index = 0; index < textLength; index++)
    {
        screen[WINNER_Y_POSITION * SCREEN_WIDTH + WINNER_X_POSITION + index] = text[index];
    }
}

void printPlayerTwoWon(wchar_t* screen, const wchar_t* text, const unsigned int textLength)
{
    for (size_t index = 0; index < textLength; index++)
    {
        screen[WINNER_Y_POSITION * SCREEN_WIDTH + SCREEN_WIDTH - WINNER_X_POSITION - textLength + index] = text[index];
    }
}