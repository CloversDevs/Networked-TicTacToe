
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
    server.Test();
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
    while (connected)
    {
        message response = cli.Listen();
        switch (response.cmd)
        {
        case EServer_RequestName:
        {
            DebugLog("What's your name?");
            string input = string();
            std::cin >> input;
            cli.SendMessage(EClient_Name, input);
            break;
        }
        case EServer_Lobby:
            DebugLog("Waiting for opponent!");
            break;
        case EServer_MatchStart:
            DebugLog("Playing against x!");
            break;
        case EServer_MatchUpdate:
            DebugLog("Update!");
            //ticTac = TicTac(msg.data);
            //ticTac.Render();
            break;
        case EServer_MatchLost:
            DebugLog("You lost! [R]ematch or [Q]uit?");
            //ticTac = TicTac(msg.data);
            //ticTac.Render();
            break;
        case EServer_MatchWon:
            DebugLog("You won! [R]ematch or [Q]uit?");
            //ticTac = TicTac(msg.data);
            //ticTac.Render();
            break;
        default:
            break;
        }
    }



    cli.End();

    return EStartState;
}

int main()
{
    TicTac game = TicTac("000100200");

    
    game.Render();
    if (!game.TryPlay(3, 1))
    {
        DebugLog("Invalid Move!");
    }
    game.Render();
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


/*
int CreateServer()
{
    vector<player> players;


    // Initialize WinSock
    WSADATA data;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &data);
    if (wsOk != 0) {
        cerr << "Winsock initialization error" << endl;
        return -1;
    }

    // Create listening socket
    int listening = socket(AF_INET, SOCK_DGRAM, 0);

    if (listening == INVALID_SOCKET) {
        cerr << "Invalid socket" << endl;
        return -1;
    }

    // Bind socket to ip-port pair
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(8900);
    hint.sin_addr.S_un.S_addr = ADDR_ANY;
    // inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
    if (bindResult == SOCKET_ERROR) {
        cerr << "Socket binding failed" << endl;
        return -1;
    }



    // Receive data
    message msg;

    sockaddr client;
    int clientSize = sizeof(client);
    player* sender = NULL;

    while (true) {

        memset((char*)&msg, 0, sizeof(msg));
        memset(&client, 0, sizeof(client));

        // Block until receiving bytes from sowmehere
        cout << "SERVER IS LISTENING..." << endl;
        int bytesIn = recvfrom(listening, (char*)&msg, sizeof(msg), 0, &client, &clientSize);

        switch (msg.cmd)
        {
        case EClient_Hello:
        {
            bool alreadyExists = false;
            for (auto& p : players)
            {
                if (memcmp(p.socketAddr.sa_data, client.sa_data, 6) == 0) {
                    cout << "Player Returns! " << client.sa_data << endl;
                    alreadyExists = true;
                    break;
                }
            }
            if (alreadyExists)
            {

            }
            else {
                cout << "nuevo jugador{" << client.sa_data << "}: " << msg.data << endl;
                player pa = player();
                //memcpy(p.name, msg.data, sizeof p.name);
                memcpy(&pa.socketAddr, &client, clientSize);
                players.push_back(pa);
                cout << "jugadores en la sala: " << players.size() << endl;
            }
            break;
        }
        case EClient_Name:
            for (auto& p : players)
            {
                if (memcmp(p.socketAddr.sa_data, client.sa_data, 6) == 0) {
                    cout << "Player Set name! " << client.sa_data << endl;
                    memcpy(p.name, msg.data, sizeof p.name);
                    break;
                }
            }
            cout << "Player not found!! " << endl;
            break;
        case 2:
            for (auto& p : players)
            {

                if (memcmp(p.socketAddr.sa_data, client.sa_data, 6) == 0) {
                    sender = &p;
                    cout << p.name << " : " << msg.data << endl;
                }

                int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &(p.socketAddr), sizeof(sockaddr));
                cout << "sent bytes: " << sendOk << endl;

                if (sendOk == SOCKET_ERROR)
                {
                    cout << "Send error!" << sendOk;
                    return -1;
                }
            }

            //
            //
            //

            // broadcast
            if (sender != NULL) {
                for (auto& p : players)
                {
                    int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &sender->socketAddr, sizeof(sockaddr));
                    if (sendOk == SOCKET_ERROR) {
                        cout << "Sending error!" << sendOk;
                        return -1;
                    }
                }
            };
            break;
            
        default:
            break;
        }

    }
}
*/