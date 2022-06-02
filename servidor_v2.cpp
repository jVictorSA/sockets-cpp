#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define PORTA "27015"
#define DEFAULT_BUFLEN 512

int task1() {

    while(true){
        //1-Inicializar o Winsock.
        WSADATA wsaData;

        int resultado;

        resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if(resultado != 0){
            cout << "Erro na inicializacao do winsock!" << endl;
            return 1;
        }

        //2-Criar um socket.
        struct addrinfo *result = NULL, *ptr = NULL, hints;

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;          //Explicita o uso do protocolo IPv4
        hints.ai_socktype = SOCK_STREAM;    //Explicita o uso de streams no socket
        hints.ai_protocol = IPPROTO_TCP;    //Explicita o uso do protocolo TCP
        hints.ai_flags = AI_PASSIVE;

        resultado = getaddrinfo(NULL, PORTA, &hints, &result);
        if(resultado != 0){
            cout << "Erro na criacao do socket! \n";
            WSACleanup();
            return 1;
        }

        //Criação do Socket
        SOCKET listenSocket = INVALID_SOCKET;

        listenSocket = socket(result -> ai_family,
                            result -> ai_socktype,
                            result -> ai_protocol);
        
        if (listenSocket == INVALID_SOCKET) {
            cout <<"Erro na criacao do socket(): " << WSAGetLastError() << "\n";
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        //3-Conectar o socket.
        resultado = bind(listenSocket, result -> ai_addr, (int)result -> ai_addrlen);
        if(resultado == SOCKET_ERROR){
            cout << "Erro na conexao do socket: " << WSAGetLastError() << "\n";
            freeaddrinfo(result);
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        freeaddrinfo(result);
        
        
        //4-Ouvir na porta do socket esperand por um cliente.
        if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR){
            cout << "Erro ao escutar a porta: " << WSAGetLastError() << "\n";
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        
        //5-Aceitar a conexão de um cliente.
        //TALVEZ ESTA PARTE TENHA QUE SER MOVIDA COMLETAMENTE PARA O LOOP. TALVEZ N PQ O COMANDO PARA NO ACCEPT?
        SOCKET cliente;

        cliente = INVALID_SOCKET;

        cliente = accept(listenSocket, NULL, NULL); 
        if(cliente == INVALID_SOCKET){
            cout << "falha no aceite da conexao: " << WSAGetLastError() << "\n";
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        
        //6-Receber e enviar dados.
        char bufferReceb[DEFAULT_BUFLEN];
        int resultEnvio;
        int tamBuffReceb = DEFAULT_BUFLEN;

        //Receber até que o cliente encerre a conexão
        do{
            resultado = recv(cliente, bufferReceb, tamBuffReceb, 0);
            if(resultado > 0){
                cout << "Bytes recebidos: " << resultado << "\n";

                resultEnvio = send(cliente, bufferReceb, resultado, 0);
                
                if(resultEnvio == SOCKET_ERROR){
                    cout << "Falha ao enviar: " << WSAGetLastError() << "\n";
                    closesocket(cliente);
                    WSACleanup();
                    return 1;
                }
                cout << "Bytes enviados: " << resultEnvio << "\n";
            }else if(resultado == 0){
                cout << "Encerrando conexao... \n";
            }else{
                cout << "recv falhou: " << WSAGetLastError() << "\n";
                closesocket(cliente);
                WSACleanup();
                return 1;
            }

        }while(resultado > 0);

        //7-Desconectar.
        resultado = shutdown(cliente, SD_SEND);
            if(resultado == SOCKET_ERROR){
                cout << "falha no shutdown: " << WSAGetLastError() << "\n";
                closesocket(cliente);
                WSACleanup();
                return 1;
            }

        closesocket(cliente);
        WSACleanup();

        cout << "Funcionou! \n";

    //return 0;
    }
}


int main(){
    thread first(task1);

    //Alguns códigos podem ser introduzidos na intermedio
    cout << "Em processo principal..."<<endl;

    first.join();

    return 0;
}