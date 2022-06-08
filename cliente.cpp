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

void JoinName(char *destination, char msg[], string name){
    int len_msg = strlen(msg);
    int len_name = name.length();
    //DEBUG{cout<<"dest:"<<destination<<", msg:"<<msg<<", name:"<<name<<endl;}
    int i = 0;
    for (i=0; i<len_name;++i){
        destination[i] = name[i];
        //DEBUG{cout<<"i:"<<i<<destination<<endl;}
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

int sender(SOCKET cliente){
    char recvbuf[DEFAULT_BUFLEN];
    int sentRes;
    int recvbuflen = DEFAULT_BUFLEN;
    string nome;

    cout << "Digite seu nome: ";
    getline(cin, nome);
    cout << nome << ", para sair digite \"exit()\"\n";

    while (true){
        string message;
        getline(cin, message);
        const int n = message.length();
        char sendMes[n+1];
        strcpy(sendMes, message.c_str());

        //Checar se o usuário deseja sair do programa
        if(message.compare("exit()") == 0){
            sentRes = send(cliente, sendMes, (int)strlen(sendMes), 0);
            return 0;
        }
        
        char sendMes2[DEFAULT_BUFLEN];

        JoinName(sendMes2, sendMes, nome);

        //Enviar mensagens
        sentRes = send(cliente, sendMes2, (int)strlen(sendMes2), 0);

        if (sentRes == SOCKET_ERROR) {
            std::cout << "send failed with error: " << WSAGetLastError() << "\n";
            closesocket(cliente);
            WSACleanup();
            return 1;
        }
    }

    return 0;
}

int receiver(SOCKET cliente){     
    char recvbuf[DEFAULT_BUFLEN];
    int recvRes;
    int recvbuflen = DEFAULT_BUFLEN;

    while(true){
        memset(recvbuf, 0, DEFAULT_BUFLEN);
        
        recvRes = recv(cliente, recvbuf, recvbuflen, 0);
        if ( recvRes > 0 ){
            std::string s(recvbuf);

                //checar se é para encerrar a thread
                if(s.compare("exit()") == 0){
                    return 0;
                }

            std::cout << recvbuf << "\n\n";
        }
        else if ( recvRes == 0 ){
            std::cout << "Conexão encerrada\n";
            continue;
        }
    }

    return 0;
}


int main(int argc, char **argv) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;
    
    //Validando parâmetros de entrada
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << "server-name\n";
        return 1;
    }

    //Inicializando o Winsock
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

    //Tentar conectar à um IP, até que seja aceito
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        //Fazer o bind do socket
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cout << "socket failed with error: "<< WSAGetLastError() << " \n";
            WSACleanup();
            return 1;
        }

        //Conectar ao servidor
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

    //Esperar o cliente encerrar a conexão, ou algum erro no servidor ocorrer
    sendThread.join();
    recThread.join();    
    
    //Limpeza do socket
    closesocket(ConnectSocket);
    WSACleanup();

    cout << "\n Obrigado por usar o nosso chat!!\n Volte Sempre!";

    return 0;
}