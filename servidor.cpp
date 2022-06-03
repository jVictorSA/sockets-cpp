#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

//using namespace std;

#define PORTA "27015"
#define DEFAULT_BUFLEN 4096
#define MAX_CLIENTS 5

int loop_mensagens(SOCKET cliente, SOCKET demaisClientes[], int numCliente){
    std::cout << "Thread " << numCliente << " iniciada.\n";
    char bufferReceb[DEFAULT_BUFLEN];
    int resultEnvio;
    int tamBuffReceb = DEFAULT_BUFLEN;
    int resultado;
    //Receber até que o cliente encerre a conexão
    while (true){
        memset(bufferReceb, 0, DEFAULT_BUFLEN);
        resultado = recv(cliente, bufferReceb, tamBuffReceb, 0);
        
        if(resultado > 0){
            std::cout << "Bytes recebidos: " << resultado << "\n";

            for(int i = 0; i < MAX_CLIENTS; i++){
                if (demaisClientes[i] != INVALID_SOCKET && demaisClientes[i] != cliente){
                    resultEnvio = send(demaisClientes[i], bufferReceb, resultado, 0);
                    std::cout << bufferReceb << "\n";
                    
                    if(resultEnvio == SOCKET_ERROR){
                        std::cout << "Falha ao enviar: " << WSAGetLastError() << "\n";
                        closesocket(cliente);
                        WSACleanup();
                        return 1;
                    }
                    std::cout << "Bytes enviados: " << resultEnvio << "\n";
                }else if(resultado == 0){
                    std::cout << "Encerrando conexao... \n";
                }
            }
        }

    };
}


int main() {

    //1-Inicializar o Winsock.
    WSADATA wsaData;

    int resultado;

    resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(resultado != 0){
        std::cout << "Erro na inicializacao do winsock!" << std::endl;
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
        std::cout << "Erro na criacao do socket! \n";
        WSACleanup();
        return 1;
    }

    //Criação do Socket do servidor
    std::cout << "Iniciando servidor...\n\n";
    SOCKET listenSocket = INVALID_SOCKET;

    listenSocket = socket(result -> ai_family,
                          result -> ai_socktype,
                          result -> ai_protocol);
    
    if (listenSocket == INVALID_SOCKET) {
        std::cout <<"Erro na criacao do socket(): " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //3-Conectar o socket.
    std::cout << "Configurando servidor...\n\n";
    resultado = bind(listenSocket, result -> ai_addr, (int)result -> ai_addrlen);
    if(resultado == SOCKET_ERROR){
        std::cout << "Erro na conexao do socket: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    
    //Array que armazena clientes da aplicação
    SOCKET clientes[MAX_CLIENTS];
    int cnt_cliente = 0;   //Contador de clientes conectados. Usado para designar um cliente 
                           //à um índice no array clientes

    std::thread clientes_threads[MAX_CLIENTS];

    //Inicilização do array de clientes
    for(int i = 0; i < MAX_CLIENTS; i++){
        clientes[i] = INVALID_SOCKET;
    }


    std::cout << "Servidor rodando!\n\n";

    while (true){

    //4-Ouvir na porta do socket esperand por um cliente.
    //if(clientes[4] != INVALID_SOCKET) //tratamento de erro máximo de clientes
    if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR){
        std::cout << "Erro ao escutar a porta: " << WSAGetLastError() << "\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    
    //5-Aceitar a conexão de um cliente.
    clientes[cnt_cliente] = accept(listenSocket, NULL, NULL); 
    if(clientes[cnt_cliente] == INVALID_SOCKET){
        std::cout << "falha no aceite da conexao: " << WSAGetLastError() << "\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }else{
        //loop_mensagens(clientes[cnt_cliente], clientes, cnt_cliente);
        clientes_threads[cnt_cliente] = std::thread(loop_mensagens, clientes[cnt_cliente], clientes, cnt_cliente);
        std::cout << "Cliente " << cnt_cliente << " conectado \n";
        cnt_cliente++;
    }
    }
    
    //6-Receber e enviar dados.

    clientes_threads[0].join();
    clientes_threads[1].join();
    clientes_threads[2].join();
    clientes_threads[3].join();
    clientes_threads[4].join();


    //7-Desconectar.
    for (int i = 0; i < MAX_CLIENTS; i++){

        resultado = shutdown(clientes[i], SD_SEND);
            if(resultado == SOCKET_ERROR){
                std::cout << "falha no shutdown: " << WSAGetLastError() << "\n";
                closesocket(clientes[i]);
                WSACleanup();
                return 1;
            }
    }

    for (int i = 0; i < MAX_CLIENTS; i++){
        closesocket(clientes[i]);
    }
    WSACleanup();

    std::cout << "Funcionou! \n";

  return 0;
}