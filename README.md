# Sockets-cpp
Projeto sobre sockets de rede para a disciplina de Redes de Computadores.

Para compilar use o servidor:
```
g++ servidor.cpp -lWs2_32 -o <nome do arquivo executavél que você desejar>
```

Para compilar use o cliente:
```
g++ cliente.cpp -lWs2_32 -lMswsock -lAdvApi32 -o cliente
```

Para executar o projeto, abra dois terminais e em um terminal execute o servidor (deve ser executado sempre antes do cliente) e no outro terminal execute o cliente

