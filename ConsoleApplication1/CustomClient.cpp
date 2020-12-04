#include "CustomClient.h"

CustomClient::CustomClient(PCSTR ip, int port)
{
    // Bind socket to ip-port pair
    _receiveAddress.sin_family = AF_INET;
    _receiveAddress.sin_port = htons(port);
    inet_pton(AF_INET, ip, &_receiveAddress.sin_addr);
}

int CustomClient::Start()
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

message CustomClient::Listen()
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

int CustomClient::SendMessage(int cmd, std::string data)
{
    char* cstr = new char[data.length() + 1];
    strcpy_s(cstr, data.length() + 1, data.c_str());
    return SendMessage(cmd, cstr);
}

int CustomClient::SendMessage(int cmd, char* data)
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

int CustomClient::End()
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