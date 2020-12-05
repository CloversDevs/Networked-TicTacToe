
#include <string>
#include <iostream>
#include "CustomClient.h"
#include "CustomServer.h"
#include "TicTacToeGame.h"
#include "MessageTypes.h"
#include "StateTypes.h"
#include "States.cpp"
using namespace std;


void DebugLog(string str)
{
    //return;
    std::cout << "DEBUG: ";
    std::cout << str;
    std::cout << "\n";
}

int StartState()
{
    DebugLog("Enter Main Server state");
    std::cout << "//////////////////////////////////\n";
    std::cout << "///   WELCOME TO TIC TAC TOE   ///\n";
    std::cout << "//////////////////////////////////\n";
    std::cout << "\n";
    bool exit = false;
    while (!exit) {
        std::cout << "[1] Server or [2] Client?\n";
        string input;
        std::cin >> input;
        if (input == "1")
        {
            std::cout << "Selected Server\n";
            return EServerState;
        }
        if (input == "2")
        {
            std::cout << "Selected Client\n";
            return EClientState;
        }
    }
    return EStartState;
}

int ServerState()
{
    DebugLog(" Enter Server state");
    auto server = CustomServer();
    //CreateServer();
    server.Run();
    return EStartState;
}

int ClientState()
{
    DebugLog("Enter Client state");
    CustomClient cli = CustomClient("127.0.0.1", 8900);
    cli.Start();
    bool connected = true;
    TicTac ticTac = TicTac();

    cli.SendMessage(EClient_Hello, "Hello");
    string opponentName = "NoName";
    while (connected)
    {
        message response = cli.Listen();
        switch (response.cmd)
        {
        case EServer_RequestName:
        {
            system("CLS");
            DebugLog("What's your name?");
            string input = string();
            std::cin >> input;
            cli.SendMessage(EClient_Name, input);
            break;
        }
        case EServer_MatchStart:
            system("CLS");
            std::cout << "Playing against: " << response.data;
            opponentName = response.data;
            break;
        case EServer_MatchUpdate:
        {
            system("CLS");
            DebugLog("Playing against " + opponentName);
            std::string boardDescription = std::string();

            for (auto i = 1; i < 10; i++)
            {
                boardDescription += response.data[i];
            }
            ticTac = TicTac(boardDescription);
            ticTac.Render();
            if (response.data[0] == '1')
            {
                DebugLog("It's my turn!");
                string input = string();
                std::cin >> input;
                byte in = 256;
                if (input == "1")
                {
                    in = 0;
                }
                if (input == "2")
                {
                    in = 1;
                }
                if (input == "3")
                {
                    in = 2;
                }
                if (input == "4")
                {
                    in = 3;
                }
                if (input == "5")
                {
                    in = 4;
                }
                if (input == "6")
                {
                    in = 5;
                }

                if (input == "7")
                {
                    in = 6;
                }

                if (input == "8")
                {
                    in = 7;
                }
                if (input == "9")
                {
                    in = 8;
                }
                char in2 = (char)in;
                char* pChar = &in2;
                cout << "Sending '" << pChar[0] << "'";
                cli.SendMessage(EClient_RequestMove, pChar);
                
            }
            else {
                DebugLog("It's the opponent's turn!");
            }
            break;
        }
        case EServer_MatchLost:
        {
            system("CLS");
            std::string boardDescription = std::string();
            boardDescription += response.data[0];
            boardDescription += response.data[1];
            boardDescription += response.data[2];
            boardDescription += response.data[3];
            boardDescription += response.data[4];
            boardDescription += response.data[5];
            boardDescription += response.data[6];
            boardDescription += response.data[7];
            boardDescription += response.data[8];

            ticTac = TicTac(boardDescription);
            ticTac.Render();

            bool validInput = false;
            string selection = "";
            while (!validInput) {
                DebugLog("You lost! [R]ematch or [Q]uit?");
                std::cin >> selection;
                validInput = selection == "R" || selection == "r" || selection == "Q" || selection == "q";
            }
            if (selection == "R" || selection == "r")
            {
                cli.SendMessage(EClient_RequestRematch, " ");
            }
            else
            {
                cli.SendMessage(EClient_RejectRematch, " ");
                connected = false;
            }
            break;
        }
        case EServer_MatchWon:
        {
            system("CLS");
            std::string boardDescription = std::string();
            boardDescription += response.data[0];
            boardDescription += response.data[1];
            boardDescription += response.data[2];
            boardDescription += response.data[3];
            boardDescription += response.data[4];
            boardDescription += response.data[5];
            boardDescription += response.data[6];
            boardDescription += response.data[7];
            boardDescription += response.data[8];

            ticTac = TicTac(boardDescription);
            ticTac.Render();
            
            bool validInput = false;
            string selection = "";
            while (!validInput) {
                DebugLog("You won! [R]ematch or [Q]uit?");
                std::cin >> selection;
                validInput = selection == "R" || selection == "r" || selection == "Q" || selection == "q";
            }
            if (selection == "R" || selection == "r")
            {
                cli.SendMessage(EClient_RequestRematch, " ");
            }
            else
            {
                cli.SendMessage(EClient_RejectRematch, " ");
                connected = false;
            }
            break;
        }
        default:
            break;
        }
    }
    cli.End();

    return EExit;
}

int main()
{
    int currentState = EStartState;
    while (currentState != EExit)
    {
        switch (currentState)
        {
        case EStartState:
            currentState = StartState();
            break;
        case EServerState:
            currentState = ServerState();
            break;
        case EClientState:
            currentState = ClientState();
            break;
        default:
            break;
        }
    }
    return 0;
}