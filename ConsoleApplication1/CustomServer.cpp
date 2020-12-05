#include "CustomServer.h"


player* CustomServer::GetPlayerById(int id)
{
    for (auto& p : players)
    {
        if (p.playerId == id)
        {
            return &p;
        }
    }
    return NULL;
}

NetworkedMatch* CustomServer::GetMatchById(int id)
{
    for (auto& m : currentGames)
    {
        if (m.MatchId == id)
        {
            return &m;
        }
    }
    return NULL;
}

CustomServer::CustomServer()
{

}
int CustomServer::Start()
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
    //hint.sin_addr.S_un.S_addr = ADDR_ANY;
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
    if (bindResult == SOCKET_ERROR) {
        cerr << "winsoc binding failed" << endl;
        return -1;
    }
}
message CustomServer::Listen(char* dat)
{
    message msg;
    sockaddr client;
    int clientSize = sizeof(client);
    memset((char*)&msg, 0, sizeof(msg));
    memset(&client, 0, sizeof(client));

    // Block until receiving bytes from sowmehere
    cout << "SERVER IS LISTENING..." << endl;
    int bytesIn = recvfrom(listening, (char*)&msg, sizeof(msg), 0, &client, &clientSize);
    
    for (int i = 0; i < 256; i++)
    {
        dat[i] = client.sa_data[i];
        if (client.sa_data[i] == '\0')
        {
            cout << "BREAK AT '" << i << "'" << endl;
            break;
        }
    }
    cout << "RECEIVED FROM CLIENT WITH ADDRESS DATA '" << client.sa_data << "'"<< endl;
    return msg;
}
void RequestName(SOCKET listening, const sockaddr client)
{
    message msg = message();
    msg.cmd = EServer_RequestName;
    char myWord[] = { '\0' };
    strcpy_s(msg.data, 1, myWord);

    std::cout << "Sending " << msg.data;
    int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &(client), sizeof(client));
    cout << "sent bytes: " << sendOk << endl;

    if (sendOk == SOCKET_ERROR)
    {
        cout << "Send error!" << sendOk << " -> " << WSAGetLastError() << endl;
        return;
    }

    cout << "Name requested!" << endl;

}

void SendBoard(SOCKET listening, const sockaddr client, bool isPlayerTurn, std::string board)
{
    message msg = message();
    msg.cmd = EServer_MatchUpdate;
    char playerTurn = '0';
    if (isPlayerTurn)
    {
        playerTurn = '1';
    }
    char myWord[] = { playerTurn, board[0], board[1], board[2], board[3], board[4], board[5], board[6], board[7], board[8], '\0' };
    strcpy_s(msg.data, 11, myWord);

    std::cout << "Sending " << msg.data;
    int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &(client), sizeof(client));
    cout << "sent bytes: " << sendOk << endl;

    if (sendOk == SOCKET_ERROR)
    {
        cout << "Send error!" << sendOk << " -> " << WSAGetLastError() << endl;
        return;
    }
}

void CustomServer::UpdateMatchOnClients(SOCKET listening, NetworkedMatch n)
{
    message msg = message();
    msg.cmd = EServer_RequestName;
    char myWord[] = { '\0' };
    strcpy_s(msg.data, 1, myWord);
    auto p1 = GetPlayerById(n.player1Id);
    auto p2 = GetPlayerById(n.player2Id);
    std::cout << "Sending DRAWING to p1["<< n.player1Id <<"] '" << p1->name << "' {" << p1->socketAddr.sa_data << "}" << msg.data;
    auto isPlayer1Turn = n.Game.currentPlayer == n.player1Id;
    auto boardString = n.Game.ToString();
    SendBoard(listening, p1->socketAddr, isPlayer1Turn, boardString);
    SendBoard(listening, p2->socketAddr, !isPlayer1Turn, boardString);
}

NetworkedMatch CustomServer::CreateMatch(player p1, player p2, int matchId)
{
    auto match = NetworkedMatch(p1.playerId, p2.playerId, matchId);
    //p1.matchId = matchId;
    //p2.matchId = matchId;
    cout << "----------------------" << endl;
    cout << " A " << endl;
    cout << "    MATCH" << endl;
    cout << "          HAS" << endl;
    cout << "              STARTED " << endl;
    cout << "----------------------" << endl;
    return match;
}
void CustomServer::Test()
{
    Start();
    while (true)
    {
        message msg;
        sockaddr client;
        int clientSize = sizeof(client);
        memset((char*)&msg, 0, sizeof(msg));
        memset(&client, 0, sizeof(client));

        // Block until receiving bytes from sowmehere
        cout << endl;
        cout << "----------------------" << endl;
        cout << "SERVER IS LISTENING..." << endl;
        cout << "Players: " << players.size() << endl;
        cout << "Matches: " << currentGames.size() << endl;
        cout << "----------------------" << endl;
        cout << endl;
        int bytesIn = recvfrom(listening, (char*)&msg, sizeof(msg), 0, &client, &clientSize);
        cout << "New command from client " << "-> {" << (int)(msg.cmd) << "} DATA:'" << client.sa_data << "'" << endl;

        player* source = NULL;
        
        auto playerIndex = 0;
        for (auto& p : players)
        {
            if (memcmp(p.socketAddr.sa_data, client.sa_data, 6) == 0) {
                cout << "Message from player " << playerIndex << client.sa_data << endl;
                source = &p;
                break;
            }
            playerIndex++;
        }

        if (source == NULL)
        {
            source = new player();
            source->playerId = ++playerIds;
            memcpy(&source->socketAddr, &client, sizeof(client));
            cout << "Added player: '" << source->socketAddr.sa_data << "'!" << endl;
            players.push_back(*source);

            cout << "Request player["<< playerIndex <<"] their name" << endl;
            RequestName(listening, client);
            continue;
        }

        if (msg.cmd != EClient_Name && source->name == "undefined")
        {
            cout << "Request player[" << playerIndex << "] their name" << endl;
            RequestName(listening, client);
            continue;
        }


        switch (msg.cmd)
        {
            case EClient_Name:
                cout << "Set player[" << playerIndex << "] name changed from '" << source->name << "' to '" << msg.data << "'"<< endl;
                memcpy(source->name, msg.data, sizeof source->name);
                if (waitingPlayer == NULL) {
                    waitingPlayer = source;
                    //waitingPlayer->playerId = source->playerId;
                   // memcpy(waitingPlayer->name, source->name, sizeof source->name);
                    //memcpy(&waitingPlayer->socketAddr, &source->socketAddr, sizeof source->socketAddr);
                    cout << "Waiting for opponent" << endl;
                }
                else
                {
                    cout << "MatchStart! " << source->name << " vs " << waitingPlayer->name << endl;
                    int matchId = ++matchIds;
                    memcpy(&waitingPlayer->matchId, &matchId, sizeof matchId);
                    memcpy(&source->matchId, &matchId, sizeof matchId);
                    auto match = CreateMatch(*source, *waitingPlayer, matchIds);
                    currentGames.push_back(match);
                    waitingPlayer = NULL;
                    UpdateMatchOnClients(listening, match);
                }
                break;
            case EClient_RequestMove:
            {
                auto match = GetMatchById(source->matchId);
                if (match->TryPlay(source->matchId, 0))
                {
                    UpdateMatchOnClients(listening, *match);
                }
            }
        }
    }

    End();
}

int CustomServer::SendMessage(player target, int cmd, std::string data)
{
    char* cstr = new char[data.length() + 1];
    strcpy_s(cstr, data.length() + 1, data.c_str());
    return Send(target, cmd, cstr);
}
int CustomServer::Send(player target, int cmd, char* data)
{
    message msg = message();
    msg.cmd = cmd;
    char myWord[] = { 'H' , 'e' , 'l' , 'l' , 'o' , '\0' };
    strcpy_s(msg.data, 6, myWord);

    std::cout << "Sending " << msg.data;
    int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &(target.socketAddr), sizeof(target.socketAddr));
    cout << "sent bytes: " << sendOk << endl;

    if (sendOk == SOCKET_ERROR)
    {
        cout << "Send error!" << sendOk << " -> " << WSAGetLastError() << endl;
        return -1;
    }
}
int CustomServer::End()
{
    return 0;
}