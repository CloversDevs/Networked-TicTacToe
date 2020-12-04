#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


enum TicTacState
{
    EWAITING,
    EDRAW,
    EWON1,
    EWON2,
};

class Pattern
{
public:
    int shape[9] = {
        0,0,0,
        0,0,0,
        0,0,0,
    };
    Pattern(int pos1, int pos2, int pos3)
    {
        shape[pos1] = 1;
        shape[pos2] = 1;
        shape[pos3] = 1;
    }
    int Test(int* board)
    {
        int winner = 0;
        for (int i = 0; i < 9; i++) {
            // Only check where shape is 1
            if (shape[i] != 1)
            {
                continue;
            }

            // If the space is empty we know already that noone completes this shape
            if (board[i] == 0)
            {
                return 0;
            }

            // First player to appear is assigned possible winner
            if (winner == 0)
            {
                winner = board[i];
            }

            // If the possible winner does not occupy every space where shape is 1, they've failed
            if (winner != board[i])
            {
                return 0;
            }
        }
        return winner;
    }
};

class TicTac
{
public:
    int board[9] = {
        0,0,0,
        0,0,0,
        0,0,0,
    };
    int currentPlayer = 1;
    TicTac()
    {

    }
    TicTac(std::string description)
    {
        Set(description);
    }
    void Set(std::string description)
    {
        for (int i = 0; i < 9; i++)
        {
            board[i] = description[i] - '0';
        }
    }
    TicTacState GetState()
    {
        int won = TestWhoWon();
        if (won == -1)
        {
            return EDRAW;
        }

        if (won == 1)
        {
            return EWON1;
        }

        if (won == 2)
        {
            return EWON2;
        }

        if (isFull())
        {
            return EDRAW;
        }

        return EWAITING;
    }

    bool isFull()
    {
        for (int i = 0; i < 9; i++) {
            if (board == 0) {
                return false;
            }
        }
        return true;
    }
    bool TryPlay(int player, int position)
    {
        if (player != currentPlayer)
        {
            return false;
        }

        if (board[position] != 0)
        {
            return false;
        }
        
        board[position] = player;

        if (currentPlayer == 1)
        {
            currentPlayer = 2;
        }
        else
        {
            currentPlayer = 1;
        }
    }
    int TestWhoWon()
    {
        vector<Pattern> winShapes;

        winShapes.push_back(Pattern(0, 1, 2));
        winShapes.push_back(Pattern(3, 4, 5));
        winShapes.push_back(Pattern(6, 7, 8));

        winShapes.push_back(Pattern(0, 3, 6));
        winShapes.push_back(Pattern(1, 4, 7));
        winShapes.push_back(Pattern(2, 5, 8));

        winShapes.push_back(Pattern(0, 4, 8));
        winShapes.push_back(Pattern(2, 4, 6));
        int index = 0;
        int gameWinner = 0;
        for (auto& shape : winShapes)
        {
            int shapeWinner = shape.Test(board);
            if (shapeWinner == 0)
            {
                //DebugLog("Pattern Test Result[" + std::to_string(index) + "]: None");
                continue;
            }
            if (gameWinner != 0 && gameWinner != shapeWinner)
            {
                //DebugLog("Pattern Test Result -> DRAW");
                return -1;
            }

            gameWinner = shapeWinner;
            //DebugLog("Pattern Test Result[" + std::to_string(index) + "]: " + std::to_string(shapeWinner));
            index++;
        }
        //DebugLog("Pattern Test Result -> " + std::to_string(gameWinner));
        return gameWinner;
    }

    void Render();
    std::string ToString();
};