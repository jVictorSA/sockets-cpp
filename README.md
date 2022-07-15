# Sockets-cpp
Projeto sobre sockets de rede para a disciplina de Redes de Computadores.  
O código roda somente em sistemas Windows, devido ao fato de que o C++ não dispõe de recursos nativos de sockets, e optamos por utilizar a biblioteca de sockets do windows (WinSock)  

Para compilar o código do servidor:
```
g++ servidor.cpp -lWs2_32 -o servidor.exe  
ou  
clang++ servidor.cpp -lWs2_32 -o servidor.exe
```

Para compilar código do cliente:
```
g++ cliente.cpp -lWs2_32 -lMswsock -lAdvApi32 -o cliente.exe  
ou  
clang++ cliente.cpp -lWs2_32 -lMswsock -lAdvApi32 -o cliente.exe
```

Para executar o projeto, será necessário abrir dois terminais: um para o servidor e outro para o cliente.  
Ademais o servidor deve **SEMPRE** ser executado antes do cliente.  
Para executar o servidor, passe o seu endereço de IPv4 como argumento  
Para executar o cliente, passe o endereço de IPv4 em que o servidor está rodando como argumento.  
Exemplos de como executar ambos programas no terminal são mostrados a seguir:

````
Para o servidor:  
servidor.exe localhost  
ou  
servidor.exe 127.0.0.1 (ou qualquer que seja seu endereço de IPv4)  

Para o cliente:  
cliente.exe <IPv4 onde o servidor está rodando>  
ou  
cliente.exe localhost (caso o servidor esteja rodando no localhost)
````
