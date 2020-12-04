
#include <iostream>
#include<WS2tcpip.h>
#include <vector>
#include <sstream>
#include <string>
#include <tuple>
#include "TicTacToeGame.h"
#pragma comment (lib, "ws2_32.lib")

enum States
{
    EExit = -1,
    EStartState = 0,
    EServerState = 1,
    EClientState = 2,
};

enum ClientMessages
{
    EClient_Hello = 0,         //
    EClient_Name = 1,        // name
    EClient_RequestRefresh = 2,//
    EClient_RequestPlay = 3,   //
    EClient_RequestMove = 4,   // position
    EClient_RequestRematch = 5,//
};

enum ServerMessages
{   
    EServer_RequestName = 0,   //
    EServer_Lobby = 1,         // player1,player2,player3;player4,player5
    EServer_MatchStart = 2,    // yourturn, vsplayername
    EServer_MatchUpdate = 3,   // yourturn, board
    EServer_MatchWon = 4,      // board
    EServer_MatchLost = 5,     // board
    EServer_BadInput = 6,      // reason
};

struct message {
    byte cmd;
    char data[255];
};

struct player {
    char name[16] = "undefined";
    sockaddr socketAddr;
};

using namespace std;

void DebugLog(string str)
{
    //return;
    std::cout << "DEBUG: ";
    std::cout << str;
    std::cout << "\n";
}
class CustomServer
{
private:
    vector<player> players;
    int listening;
public:
    CustomServer()
    {

    }
    int Start()
    {
        // Initialize WinSock
        WSADATA data;
        WORD ver = MAKEWORD(2, 2);
        int wsOk = WSAStartup(ver, &data);
        if (wsOk != 0) {
            cerr << "Winsock initialization error" << endl;
            return -1;
        }

        // Create listening socket
        listening = socket(AF_INET, SOCK_DGRAM, 0);

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
            cerr << "winsoc binding failed" << endl;
            return -1;
        }
    }
    std::tuple <char*, message> Listen()
    {
        // Receive data
        message msg;

        sockaddr client;
        int clientSize = sizeof(client);
        memset((char*)&msg, 0, sizeof(msg));
        memset(&client, 0, sizeof(client));

        // Block until receiving bytes from sowmehere
        cout << "SERVER IS LISTENING..." << endl;
        int bytesIn = recvfrom(listening, (char*)&msg, sizeof(msg), 0, &client, &clientSize);
        return std::make_tuple(client.sa_data, msg);
    }
    void Test()
    {
        Start();
        while (true)
        {
            auto playerMessage = Listen();

            bool playerExists = false;
            player* source = NULL;
            for (auto& p : players)
            {
                if (memcmp(p.socketAddr.sa_data, std::get<0>(playerMessage), 6) == 0) {
                    cout << "Player Returns! " << std::get<0>(playerMessage) << endl;
                    source = &p;
                    break;
                }
            }

            if (source == NULL)
            {
                source = new player();
                //memcpy(p.name, msg.data, sizeof p.name);
                memcpy(&source->socketAddr, &std::get<1>(playerMessage), sizeof(std::get<1>(playerMessage)));
                players.push_back(*source);
                cout << "Player, I need your name!" << endl;
                string s = "Hello";
                SendMessage(*source, EServer_RequestName, s);
                cout << "Name requested!" << endl;
                return;
            }
            auto msg = std::get<1>(playerMessage);
            //;
            switch (msg.cmd)
            {
            case EClient_Name:
                memcpy(source->name, msg.data, sizeof source->name);
                cout << "Set player name" << msg.data << "->" << source->name << endl;
            break;
            }
        }
        
        End();
    }
    int SendMessage(player target, int cmd, std::string data)
    {
        char* cstr = new char[data.length() + 1];
        strcpy_s(cstr, data.length() + 1, data.c_str());
        return Send(target, cmd, cstr);
    }
    int Send(player target, int cmd, char* data)
    {
        message msg = message();
        msg.cmd = cmd;
        strcpy_s(msg.data, strlen(data) + 1, data);

        std::cout << "Sending " << msg.data;
        int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &(target.socketAddr), sizeof(sockaddr));
        cout << "sent bytes: " << sendOk << endl;

        if (sendOk == SOCKET_ERROR)
        {
            cout << "Send error!" << sendOk;
            return -1;
        }
    }
    int End()
    {
        return 0;
    }
};
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
            /*
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
            */
        default:
            break;
        }

    }
    std::cout << "Hello World!\n";
}

class CustomClient
{
private:
    SOCKET _sendSocket = INVALID_SOCKET;
    sockaddr_in _receiveAddress;
public:
    CustomClient(PCSTR ip, int port)
    {
        // Bind socket to ip-port pair
        _receiveAddress.sin_family = AF_INET;
        _receiveAddress.sin_port = htons(port);
        inet_pton(AF_INET, ip, &_receiveAddress.sin_addr);
    }

    int Start()
    {
        int iResult;
        WSADATA wsaData;

        char SendBuf[1024];
        int BufLen = 1024;

        //----------------------
        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != NO_ERROR) {
            wprintf(L"WSAStartup failed with error: %d\n", iResult);
            return 1;
        }

        //---------------------------------------------
        // Create a socket for sending data
        _sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sendSocket == INVALID_SOCKET) {
            wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
    }

    message Listen()
    {
        message msg;
        memset((char*)&msg, 0, sizeof(msg));

        // Block until receiving bytes from sowmehere
        cout << "CLIENT IS LISTENING..." << endl;
        int SenderAddrSize = sizeof(_receiveAddress);
        int iResult = recvfrom(_sendSocket,
            (char*)&msg, sizeof(msg), 0, (SOCKADDR*)&_receiveAddress, &SenderAddrSize);
        return msg;
    }

    int SendMessage(int cmd, std::string data)
    {
        char* cstr = new char[data.length() + 1];
        strcpy_s(cstr, data.length() + 1, data.c_str());
        return SendMessage(cmd, cstr);
    }

    int SendMessage(int cmd, char* data)
    {
        wprintf(L"Sending a datagram to the receiver...\n");
        message msg = message();
        msg.cmd = cmd;
        strcpy_s(msg.data, strlen(data) + 1, data);
        int iResult = sendto(_sendSocket,
            (char*)&msg, sizeof(msg), 0, (SOCKADDR*)&_receiveAddress, sizeof(_receiveAddress));
        if (iResult == SOCKET_ERROR) {
            wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
            closesocket(_sendSocket);
            WSACleanup();
            return 1;
        }
    }

    int End()
    {
        wprintf(L"Finished sending. Closing socket.\n");
        int iResult = closesocket(_sendSocket);
        if (iResult == SOCKET_ERROR) {
            wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        //---------------------------------------------
        // Clean up and quit.
        wprintf(L"Exiting.\n");
        WSACleanup();
        return 0;
    }
};


States StartState()
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

States ServerState()
{
    DebugLog(" Enter Server state");
    auto server = CustomServer();
    //CreateServer();
    server.Test();
    return EStartState;
}

States ClientState()
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
    States currentState = EStartState;
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
