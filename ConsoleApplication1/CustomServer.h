#pragma once
#include <tuple>
#include <vector>
#include <iostream>
#include <sstream>
#include <WS2tcpip.h>
#include "DataStructures.h"
#include "MessageTypes.h"
#include "NetworkedMatch.h"

using namespace std;
class CustomServer
{
private:
    std::vector<player> players;
    player* waitingPlayer = NULL;
    std::vector<NetworkedMatch> currentGames;
    int listening;
    int playerIds = -1;
    int matchIds = -1;
public:
    player* GetPlayerById(int id);
    NetworkedMatch* GetMatchById(int id);
    CustomServer();
    int Start();
    message Listen(char* dat);
    void Test();
    int SendMessage(player target, int cmd, std::string data);
    int Send(player target, int cmd, char* data);
    int End();
    void UpdateMatchOnClients(SOCKET listening, NetworkedMatch n);
    NetworkedMatch CreateMatch(player p1, player p2, int matchIndex);
};
