#pragma once
#include <tuple>
#include <vector>
#include <iostream>
#include <sstream>
#include <WS2tcpip.h>
#include "DataStructures.h"
#include "MessageTypes.h"

using namespace std;
class CustomServer
{
private:
    std::vector<player> players;
    int listening;
public:
    CustomServer();
    int Start();
    message Listen(char* dat);
    void Test();
    int SendMessage(player target, int cmd, std::string data);
    int Send(player target, int cmd, char* data);
    int End();
};
