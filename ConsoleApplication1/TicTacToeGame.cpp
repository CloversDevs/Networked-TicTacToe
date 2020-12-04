#include "TicTacToeGame.h"

void TicTac::Render()
{
    std::cout << "       ///       ///      \n";
    std::cout << "   ";
    std::cout << board[0];
    std::cout << "   ///   ";
    std::cout << board[1];
    std::cout << "   ///   ";
    std::cout << board[2];
    std::cout << "   \n";
    std::cout << "       ///       ///      \n";
    std::cout << "////////////////////////////\n";
    std::cout << "       ///       ///      \n";
    std::cout << "   ";
    std::cout << board[3];
    std::cout << "   ///   ";
    std::cout << board[4];
    std::cout << "   ///   ";
    std::cout << board[5];
    std::cout << "   \n";
    std::cout << "       ///       ///      \n";
    std::cout << "////////////////////////////\n";
    std::cout << "       ///       ///      \n";
    std::cout << "   ";
    std::cout << board[6];
    std::cout << "   ///   ";
    std::cout << board[7];
    std::cout << "   ///   ";
    std::cout << board[8];
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
