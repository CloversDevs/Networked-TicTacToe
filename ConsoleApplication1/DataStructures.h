#pragma once
#include <cstddef>
#include <WinSock2.h>

struct message {
    byte cmd;
    char data[255];
};

struct player {
    char name[16] = "undefined";
    sockaddr socketAddr;
};
