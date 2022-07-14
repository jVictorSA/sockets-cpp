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

Para executar o projeto, abra dois terminais e em um terminal execute o servidor (SEMPRE deve ser executado sempre antes do cliente) passando o seu endereço de IPv4 como argumento e no outro terminal execute o cliente passando seu endereço de IPv4 como argumento.  
É possível fazer esta execução da seguinte forma no terminal:

````
Para o servidor:  
servidor.exe localhost  
ou  
servidor.exe 127.0.0.1 (ou qualquer que seja seu endereço de IPv4)  

Para o cliente:  
cliente.exe <IPv4 onde o servidor está rodando> ou  
cliente.exe localhost (caso o servidor esteja rodando no localhost)
````
