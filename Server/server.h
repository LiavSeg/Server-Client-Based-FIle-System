#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"
#include "response_handler.h"
#include "request_handler.h"
#include<iostream>
#include<thread>

class WSA{
public:
    WSA();
    ~WSA();
};

class Server{
    SOCKET s;
    SOCKET client_sock;
  
    public:
    Server();
    ~Server();
    void start();
    void accept_client_request(SOCKET client_sock);
};

#endif//SERVER_H
