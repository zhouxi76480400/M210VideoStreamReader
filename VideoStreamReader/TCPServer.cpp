//
// Created by dji on 6/29/20.
//

#include "TCPServer.h"
#include <iostream>


TCPServer::TCPServer(int port) {
    listening_port = port;
    serverFd = -1;
    clientFd = -1;
    std::cout << "on Create" << std::endl;
}

TCPServer::~TCPServer() {
    if(serverFd >= 0) {
        std::cout << "closing server fd ..." << std::endl;
        close(serverFd);

    }
    std::cout << "on Destory" << std::endl;
}

int TCPServer::createService() {
    if(listening_port < 0 || listening_port > 65535) {
        printf("not a valid port number");
        exit(-1);
    }
    size_t sockaddr_in_size = sizeof(struct sockaddr_in);
    memset(&serverSockAddr, 0, sockaddr_in_size);
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverFd < 0) {
        printf("can not create server socket fd");
        exit(-1);
    }
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_port = htons(listening_port);
    serverSockAddr.sin_addr.s_addr = INADDR_ANY;
    //
    if(bind(serverFd, (struct sockaddr *)&serverSockAddr, sockaddr_in_size) < 0) {
        close(serverFd);
        printf("Cannot bind the server socket");
        exit(-1);
    }
    //
    if(listen(serverFd, LISTEN_MAX_SOCKET_WAITING_NUM) < 0) {
        close(serverFd);
        printf("Cannot listen the server socket");
        exit(-1);
    }
    return serverFd;
};

int TCPServer::accept() {
    std::cout << "waiting for accept" << std::endl;
    socklen_t sockaddr_in_size = sizeof(struct sockaddr_in);
    clientFd = ::accept(serverFd, (struct sockaddr *)&clientSockAddr, &sockaddr_in_size);
    std::cout << "accepted: fd=" << clientFd << std::endl;
    return clientFd;
}

int TCPServer::write(const void * buffer, size_t size) {
    int len = 0;
    if(clientFd > 0) {
        len = ::send(clientFd, buffer, size, 0);
    }
    return len;
}

int TCPServer::getClientFd() {
    return clientFd;
}

void TCPServer::closeClientFd() {
    if(clientFd > 0) {
        close(clientFd);
        clientFd = -1;
    }
}
