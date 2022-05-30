#include <winsock2.h>
#include <ws2tcpip.h>
//#include <string.h>
#include <iostream>

/* Necessário para conectar as respectivas libs (bibliotecas), MAS NÃO FUNCIONOU*/
#pragma comment (lib, "Ms2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using namespace std;

int __cdecl main(int argc, char **argv){
    WSADATA wsa_data;
    SOCKET ConnectSocket = INVALID_SOCKET;

    //ESTA STRUCT DEVE SER ESTUDADA MELHOR
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    cout << "Digite uma mensagem:" << endl;
    char buffer_nome[DEFAULT_BUFLEN];
    cin.getline(buffer_nome, DEFAULT_BUFLEN);
    cin.ignore();
    //cout << "Buffer de envio: "<<buffer_nome<<endl;

    int resultado;
    int tamanho_buffer_rec = DEFAULT_BUFLEN;

    //Validando paramêtros
    if (argc != 2){
        cout << "Uso: "<<argv[0]<<endl;
        return 1;
    }

    //Inicializando Winsock
    resultado = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (resultado!=0){
        cout << "WSAStartup falhou: "<<resultado<<endl;
        return 1;
    }

    //ISSO QUE ENVOLVE ESTA STRUCT DEVE SER APROFUNDADO MAIS
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // Especifica um socket de fluxo
    hints.ai_protocol = IPPROTO_TCP; // Especifica o protocolo tcp

    //Inicializando endereço do servidor junto com a porta
    resultado = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (resultado!=0){
        cout << "getaddrinfo falhou com o erro: "<< resultado <<endl;
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Criando socket para se conectar ao servidor
        ConnectSocket = socket(ptr->ai_family, //NÃO FAÇO IDEIA O QUE ISSO SIGNIFICA
                                ptr->ai_socktype, //Especifica que é um socket stream
                                ptr->ai_protocol); // Especifica o protocolo TCP
        if (ConnectSocket == INVALID_SOCKET) {
            cout<<"Socket falhou com erro: "<<WSAGetLastError()<<endl;
            WSACleanup();
            return 1;
        }

        // Conectando ao servidor
        resultado = connect( ConnectSocket,
                             ptr->ai_addr, //NÂO COMPREENDI BEM
                             (int)ptr->ai_addrlen); //NÂO COMPREENDI BEM
        if (resultado == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    
    freeaddrinfo(result);

    // Enviando mensagem do buffer digitado inicialmente
    const char *inicial_mensagem = buffer_nome;
    resultado = send(ConnectSocket,
     inicial_mensagem,
     (int)strlen(inicial_mensagem), //PODE SER NECESSÁRIA A LIB string.h, SE NÃO A FUNÇÃO DEVE ESTA EM OUTROS CABEALHOS JÁ DEFINIDOS
     0);
    if (resultado == SOCKET_ERROR){
        cout << "Envio falhado com erro: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    cout << "Bytes enviados: "<<resultado<<endl;

    //Desligando conexões após a mensagem inicial ser inserida
    resultado = shutdown(ConnectSocket, SD_SEND);
    if (resultado==SOCKET_ERROR){
        cout<<"Falha no desligamento, erro: "<< WSAGetLastError() <<endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receber informações até a conexão ser encerrada
    // NÃO ENTENDI BEM ISSO
    do {
        resultado = recv(ConnectSocket, inicial_mensagem, tamanho_buffer_rec, 0);
        if (resultado>0){
            cout << "Bytes recebidos: "<< resultado<<endl;
        }
        else if (resultado==0){
            cout << "Conexão fechada."<<endl;
        }
        else{
            cout << "recv falhou com o erro: "<<WSAGetLastError()<<endl;
        }
    } while (resultado>0);

    //Liberando memória
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}