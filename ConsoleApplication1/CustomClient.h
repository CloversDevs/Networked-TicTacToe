#pragma once
#include "DataStructures.h"
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <sstream>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
class CustomClient
{
private:
    SOCKET _sendSocket = INVALID_SOCKET;
    sockaddr_in _receiveAddress;
public:
    CustomClient(PCSTR ip, int port);

    int Start();

    message Listen();

    int SendMessage(int cmd, std::string data);

    int SendMessage(int cmd, char* data);

    int End();
};


