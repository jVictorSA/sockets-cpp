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
#define DEBUG if(1)

void JoinName(char *destination, char msg[], char *name){
    int len_msg = strlen(msg);
    int len_name = strlen(name);
    //DEBUG{cout<<"dest:"<<destination<<", msg:"<<msg<<", name:"<<name<<endl;}
    int i = 0;
    for (i=0; i<len_name;++i){
        destination[i] = name[i];
        DEBUG{cout<<"i:"<<i<<destination<<endl;}
    }
    
    destination[i] = ':';
    destination[i+1] = ' ';
    //DEBUG{cout<<"dest:"<<destination<<", msg:"<<msg<<", name:"<<name<<endl;}
    int j = i+2;
    int acc = 0;
    //DEBUG{cout<<"STEP INTERMEDIARY"<<endl;}
    for (j=i+2; j<len_name+len_msg+3; ++j){
        //DEBUG{cout<<"dest:"<<destination<<", msg:"<<msg<<", name:"<<name<<endl;}
        destination[j] = msg[acc];
        acc+=1;
    }
    destination[j+1] = '\0';

}

int sender(SOCKET cliente, char *name, int *state){
    char recvbuf[DEFAULT_BUFLEN];
    int sentRes;
    int recvbuflen = DEFAULT_BUFLEN;

    while (true){
        string message;
        cout << "Diga algo se quiser ou digite 1 para sair:"<<endl;
        getline(cin, message);
        //const int n = message.length();
        char sendMes[DEFAULT_BUFLEN];

        DEBUG{cout<<"first:"<<message[0]<<endl;}
        if(message[0]=='1'){*state=2; return 2;}

        DEBUG{cout << "STEP 5"<<endl;}
        strcpy(sendMes, message.c_str());
        DEBUG{cout << "sendMes:"<<sendMes<<endl;}  
        DEBUG{cout << "STEP 6"<<endl;}
        //Putting user name:
        char sendMes2[DEFAULT_BUFLEN];
        JoinName(sendMes2, sendMes, name);
        DEBUG{cout << "STEP 7"<<endl;}  
        
        // Send an initial buffer
        sentRes = send(cliente, sendMes2, (int)strlen(sendMes), 0);

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

int receiver(SOCKET cliente, int *state){
        
    char recvbuf[DEFAULT_BUFLEN];
    int recvRes;
    int recvbuflen = DEFAULT_BUFLEN;

    while(true){
        memset(recvbuf, 0, DEFAULT_BUFLEN);
        
        recvRes = recv(cliente, recvbuf, recvbuflen, 0);
        DEBUG{cout << "recv:"<<recvRes<<endl;}
        if ( recvRes > 0 ){
            DEBUG{std::cout << "Bytes recebidos: " << recvRes << "\n";}
            std::cout << endl<<"Msg recebida: " << recvbuf << "\n\n";
        }
        else if ( recvRes == 0 ){
            std::cout << "Connection closed\n";
            if (*state==2){return 2;}
            //continue; //It's continue step the recvRes update
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
    int iResult;

    //User name:
    char name[DEFAULT_BUFLEN];
    cout << "Digite seu nome:"<<endl;
    cin.getline(name, DEFAULT_BUFLEN);
    //getchar();
    DEBUG{cout << "STEP 4"<<endl;}

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

    DEBUG{cout << "STEP 1 - initial of threads"<<endl;}
    int *state = 0;
    thread sendThread(sender, ConnectSocket, name, state);
    thread recThread(receiver, ConnectSocket, state);

    DEBUG{cout << "STEP 2 - into in threads, state:"<<state<<endl;}
    // Receive until the peer closes the connection
    if (*state==2){    
        // cleanup
        closesocket(ConnectSocket);
        WSACleanup();

        return 0;
    }

    sendThread.join();
    recThread.join();    
    
    DEBUG{cout << "STEP 3 - Dead of program"<<endl;}

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}