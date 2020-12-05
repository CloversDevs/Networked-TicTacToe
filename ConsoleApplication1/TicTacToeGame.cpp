#include "TicTacToeGame.h"

char TeamToChar(int team)
{
    if (team == 1)
    {
        return 'X';
    }
    else if (team == 2)
    {
        return 'O';
    }
    return ' ';
}

void TicTac::Render()
{
    std::cout << "       ///       ///      \n";
    std::cout << "   ";
    std::cout << TeamToChar(board[0]);
    std::cout << "   ///   ";
    std::cout << TeamToChar(board[1]);
    std::cout << "   ///   ";
    std::cout << TeamToChar(board[2]);
    std::cout << "   \n";
    std::cout << "       ///       ///      \n";
    std::cout << "////////////////////////////\n";
    std::cout << "       ///       ///      \n";
    std::cout << "   ";
    std::cout << TeamToChar(board[3]);
    std::cout << "   ///   ";
    std::cout << TeamToChar(board[4]);
    std::cout << "   ///   ";
    std::cout << TeamToChar(board[5]);
    std::cout << "   \n";
    std::cout << "       ///       ///      \n";
    std::cout << "////////////////////////////\n";
    std::cout << "       ///       ///      \n";
    std::cout << "   ";
    std::cout << TeamToChar(board[6]);
    std::cout << "   ///   ";
    std::cout << TeamToChar(board[7]);
    std::cout << "   ///   ";
    std::cout << TeamToChar(board[8]);
    std::cout << "   \n";
    std::cout << "       ///       ///      \n";
}

std::string TicTac::ToString()
{
    string str = string();
    for (int i = 0; i < 9; i++)
    {
        str = str + std::to_string(board[i]);
    }
    return str;
}
