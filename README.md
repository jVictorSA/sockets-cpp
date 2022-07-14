# Sockets-cpp
Projeto sobre sockets de rede para a disciplina de Redes de Computadores.

Para compilar o código do servidor:
```
g++ servidor.cpp -lWs2_32 -o servidor.exe
```

Para compilar código do cliente:
```
g++ cliente.cpp -lWs2_32 -lMswsock -lAdvApi32 -o cliente.exe
```

Para executar o projeto, abra dois terminais e em um terminal execute o servidor (deve ser executado sempre antes do cliente) e no outro terminal execute o cliente

