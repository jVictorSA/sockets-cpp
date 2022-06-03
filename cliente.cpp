#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "27015"

int sender(SOCKET cliente){
    char recvbuf[DEFAULT_BUFLEN];
    int sentRes;
    int recvbuflen = DEFAULT_BUFLEN;

    while (true){
            string message;
            std::cout << "Digite a mensagem:";
            getline(cin, message);
            const int n = message.length();
            char sendMes[n+1];
            strcpy(sendMes, message.c_str());

            // Send an initial buffer
            sentRes = send(cliente, sendMes, (int)strlen(sendMes), 0);
            std::cout << sentRes << "\n";

            if (sentRes == SOCKET_ERROR) {
                std::cout << "send failed with error: " << WSAGetLastError() << "\n";
                closesocket(cliente);
                WSACleanup();
                return 1;
            }

            std::cout << "Bytes Sent: " << sentRes << "\n";
            //std::cout << sendMes << "\n";

            // shutdown the connection since no more data will be sent
            /*iResult = shutdown(cliente, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
                closesocket(cliente);
                WSACleanup();
                return 1;
            }*/

        
    }

    return 0;
}

int receiver(SOCKET cliente){
    while(true){

    char recvbuf[DEFAULT_BUFLEN];
    int recvRes;
    int recvbuflen = DEFAULT_BUFLEN;
    
    recvRes = recv(cliente, recvbuf, recvbuflen, 0);
        std::cout << recvbuf << "\n";
        if ( recvRes > 0 ){
            std::cout << "Bytes received: " << recvRes << "\n";
            std::cout << "Msg recebida" << recvbuf << "\n";
        }
        else if ( recvRes == 0 ){
            std::cout << "Connection closed\n";
            continue;
        }
        
    }

    return 0;
}


int __cdecl main(int argc, char **argv) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = "Enviei esta mensagem de um cliente";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Validate the parameters
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << "server-name\n";
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << "\n";
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        std::cout << "getaddrinfo failed with error: "<< iResult << "\n";
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cout << "socket failed with error: "<< WSAGetLastError() << " \n";
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Unable to connect to server!\n";
        WSACleanup();
        return 1;
    }

    thread sendThread(sender, ConnectSocket);
    thread recThread(receiver, ConnectSocket);

    // Receive until the peer closes the connection
    
    sendThread.join();
    recThread.join();    

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}