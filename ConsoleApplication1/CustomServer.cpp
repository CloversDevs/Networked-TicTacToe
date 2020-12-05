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
    std::cout << "Board to send " << board << endl;
    std::cout << "Sending " << msg.data;
    int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &(client), sizeof(client));
    cout << "sent bytes: " << sendOk << endl;

    if (sendOk == SOCKET_ERROR)
    {
        cout << "Send error!" << sendOk << " -> " << WSAGetLastError() << endl;
        return;
    }
}

void SendGameResult(SOCKET listening, const sockaddr client, int winner, std::string board)
{
    message msg = message();
    switch (winner)
    {
    case 0:
        msg.cmd = EServer_MatchDraw;
        break;
    case 1:
        msg.cmd = EServer_MatchLost;
        break;
    case 2:
        msg.cmd = EServer_MatchWon;
        break;
    }

    char myWord[] = { board[0], board[1], board[2], board[3], board[4], board[5], board[6], board[7], board[8], '\0' };
    strcpy_s(msg.data, 10, myWord);

    std::cout << "Sending " << msg.data;
    int sendOk = sendto(listening, (char*)&msg, sizeof(msg), 0, &(client), sizeof(client));
    cout << "sent bytes: " << sendOk << endl;

    if (sendOk == SOCKET_ERROR)
    {
        cout << "Send error!" << sendOk << " -> " << WSAGetLastError() << endl;
        return;
    }
}

void SendOpponentName(SOCKET listening, const sockaddr client, std::string name)
{
    message msg = message();
    msg.cmd = EServer_MatchStart;
    strcpy_s(msg.data, name.length() + 1, name.c_str());

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
    auto isPlayer1Turn = n.Game.currentTeam == n.GetPlayerTeamFromId(p1->playerId);
    auto boardString = n.Game.ToString();

    auto gameState = n.Game.GetState();
    switch (gameState)
    {
    case EWON1:
        SendGameResult(listening, p1->socketAddr, 2, boardString);
        SendGameResult(listening, p2->socketAddr, 1, boardString);
        break;
    case EWON2:
        SendGameResult(listening, p1->socketAddr, 1, boardString);
        SendGameResult(listening, p2->socketAddr, 2, boardString);
        break;
    case EDRAW:
        SendGameResult(listening, p1->socketAddr, 0, boardString);
        SendGameResult(listening, p2->socketAddr, 0, boardString);
        break;
    default:
        SendBoard(listening, p1->socketAddr, isPlayer1Turn, boardString);
        SendBoard(listening, p2->socketAddr, !isPlayer1Turn, boardString);
        break;
    }
}

NetworkedMatch CustomServer::CreateMatch(player p1, player p2, int matchId)
{    
    cout << "----------------------" << endl;
    cout << " A " << endl;
    cout << "    MATCH" << endl;
    cout << "          HAS" << endl;
    cout << "              STARTED " << endl;
    cout << "----------------------" << endl;
    if (rand() % 100 > 50)
    {
        return NetworkedMatch(p1.playerId, p2.playerId, matchId);
    }
    else
    {
        return NetworkedMatch(p2.playerId, p1.playerId, matchId);
    }
}
void CustomServer::Run()
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
                cout << "EClient_Name!" << endl;
                cout << "Set player[" << playerIndex << "] name changed from '" << source->name << "' to '" << msg.data << "'"<< endl;
                memcpy(source->name, msg.data, sizeof source->name);
                if (!playerIsWaiting) {                    
                    memcpy(&waitingPlayer.name, &source->name, sizeof source->name);
                    memcpy(&waitingPlayer.playerId, &source->playerId, sizeof source->playerId);
                    memcpy(&waitingPlayer.socketAddr, &source->socketAddr, sizeof source->socketAddr);
                    playerIsWaiting = true;
                    cout << "Waiting for opponent" << endl;
                }
                else
                {
                    cout << "MatchStart! " << source->name << " vs " << waitingPlayer.name << endl;
                    int matchId = ++matchIds;
                    playerIsWaiting = false;
                    auto p = GetPlayerById(waitingPlayer.playerId);
                    memcpy(&p->matchId, &matchId, sizeof matchId);
                    memcpy(&source->matchId, &matchId, sizeof matchId);
                    auto match = CreateMatch(*source, *p, matchIds);
                    currentGames.push_back(match);
                    SendOpponentName(listening, source->socketAddr, p->name);
                    SendOpponentName(listening, p->socketAddr, source->name);
                    UpdateMatchOnClients(listening, match);
                }
                break;
            case EClient_RequestMove:
            {
                cout << "EClient_RequestMove!" << endl;
                auto match = GetMatchById(source->matchId);
                if (!match->TryPlay(source->playerId, msg.data[0]))
                {
                    cout << "Illegal move!" << endl;
                }
                else {
                    cout << "Legal move!" << endl;
                }
                UpdateMatchOnClients(listening, *match);
                break;
            }
            case EClient_RequestRematch:
            {
                cout << "EClient_RequestRematch!" << endl;
                auto match = GetMatchById(source->matchId);
                match->PlayerRequestsRematch(source->playerId);
                if (match->BothPlayersWantRematch())
                {
                    match->Reset();
                    UpdateMatchOnClients(listening, *match);
                }
                else if(match->APlayerRejectedRematch()){
                    //source becomes waitingPlayer or starts game with waitingPlayer;
                    if (!playerIsWaiting) {
                        memcpy(&waitingPlayer.name, &source->name, sizeof source->name);
                        memcpy(&waitingPlayer.playerId, &source->playerId, sizeof source->playerId);
                        memcpy(&waitingPlayer.socketAddr, &source->socketAddr, sizeof source->socketAddr);
                        playerIsWaiting = true;
                        cout << "Waiting for opponent" << endl;
                    }
                    else
                    {
                        cout << "MatchStart! " << source->name << " vs " << waitingPlayer.name << endl;
                        int matchId = ++matchIds;
                        playerIsWaiting = false;
                        auto p = GetPlayerById(waitingPlayer.playerId);
                        memcpy(&p->matchId, &matchId, sizeof matchId);
                        memcpy(&source->matchId, &matchId, sizeof matchId);
                        auto match = CreateMatch(*source, *p, matchIds);
                        currentGames.push_back(match);
                        SendOpponentName(listening, source->socketAddr, p->name);
                        SendOpponentName(listening, p->socketAddr, source->name);
                        UpdateMatchOnClients(listening, match);
                    }
                }
                break;
            }
            case EClient_RejectRematch:
            {
                cout << "EClient_RejectRematch!" << endl;
                auto match = GetMatchById(source->matchId);
                auto abandonTeam = match->GetPlayerTeamFromId(source->playerId);
                if (match->APlayerWantsARematch())
                {
                    player* antisource;
                    if (abandonTeam == 1)
                    {
                        antisource = GetPlayerById(match->player2Id);
                    } else {
                        antisource = GetPlayerById(match->player1Id);
                    }
                    //antisource becomes waitingPlayer or starts game with waitingPlayer;       
                    if (!playerIsWaiting) {
                        memcpy(&waitingPlayer.name, &antisource->name, sizeof antisource->name);
                        memcpy(&waitingPlayer.playerId, &antisource->playerId, sizeof antisource->playerId);
                        memcpy(&waitingPlayer.socketAddr, &antisource->socketAddr, sizeof antisource->socketAddr);
                        playerIsWaiting = true;
                        cout << "Waiting for opponent" << endl;
                    }
                    else
                    {
                        cout << "MatchStart! " << antisource->name << " vs " << waitingPlayer.name << endl;
                        int matchId = ++matchIds;
                        playerIsWaiting = false;
                        auto p = GetPlayerById(waitingPlayer.playerId);
                        memcpy(&p->matchId, &matchId, sizeof matchId);
                        memcpy(&antisource->matchId, &matchId, sizeof matchId);
                        auto match = CreateMatch(*antisource, *p, matchIds);
                        currentGames.push_back(match);
                        SendOpponentName(listening, antisource->socketAddr, p->name);
                        SendOpponentName(listening, p->socketAddr, antisource->name);
                        UpdateMatchOnClients(listening, match);
                    }
                }
                if (abandonTeam == 1)
                {
                    match->player1WantsRematch = -1;
                }
                else {
                    match->player2WantsRematch = -1;
                }
                
                break;
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