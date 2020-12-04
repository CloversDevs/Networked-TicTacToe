#include "CustomServer.h"


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
std::tuple <char*, message> CustomServer::Listen()
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
void CustomServer::Test()
{
    Start();
    while (true)
    {
        auto playerMessage = Listen();
        auto sockData = std::get<0>(playerMessage);
        auto msg = std::get<1>(playerMessage);

        bool playerExists = false;
        player* source = NULL;
        for (auto& p : players)
        {
            if (memcmp(p.socketAddr.sa_data, sockData, 6) == 0) {
                cout << "Player Returns! " << sockData << endl;
                source = &p;
                break;
            }
        }

        if (source == NULL)
        {
            source = new player();
            //memcpy(p.name, msg.data, sizeof p.name);
            memcpy(&source->socketAddr, &msg, sizeof(msg));
            players.push_back(*source);

            cout << "Player, I need your name!" << endl;
            string s = "Hello";
            SendMessage(*source, EServer_RequestName, s);
            cout << "Name requested!" << endl;
            return;
        }
        
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