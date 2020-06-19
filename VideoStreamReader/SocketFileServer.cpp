//
// Created by zhouxi on 2020/06/17.
//

#include "SocketFileServer.h"

#include <iostream>

SocketFileServer::SocketFileServer() {
    serverFd = -1;
    clientFd = -1;
    std::cout << "OnCreate" << std::endl;
}

SocketFileServer::~SocketFileServer() {
    if(serverFd > 0) {
        closeClientFd();
        unlink(SOCKET_PATH);
    }
    std::cout << "OnDestory" << std::endl;
}

int SocketFileServer::createService() {
    //
    int client_sock_addr_struct_size = sizeof(struct sockaddr_un);
    memset(&client_sock_addr_un, 0, client_sock_addr_struct_size);
    //
    serverFd = socket(AF_UNIX,SOCK_STREAM,0);
    if (serverFd < 0) {
        perror("Cannot create socket");
        exit(-1);
    }
    //
    memset(&server_sock_addr_un, 0, client_sock_addr_struct_size);
    server_sock_addr_un.sun_family = AF_UNIX;
    strcpy(server_sock_addr_un.sun_path, SOCKET_PATH);
    //
    unlink(SOCKET_PATH);
    if(bind(serverFd,(const sockaddr *)&server_sock_addr_un, client_sock_addr_struct_size) < 0) {
        close(serverFd);
        perror("Cannot bind the server socket");
        exit(-1);
    }
    if(listen(serverFd, LISTEN_MAX_SOCKET_NUM) < 0) {
        close(serverFd);
        perror("Cannot listen the server socket");
        exit(-1);
    }
    //
//    int flags = 0;
//    fcntl(serverFd, F_GETFL, &flags);
//    fcntl(serverFd, F_SETFL, O_NONBLOCK | flags);
//    std::cout << flags << std::endl;




    return serverFd;
}

int SocketFileServer::accept() {
    std::cout << "waiting accept" << std::endl;
    int client_sock_addr_struct_size = sizeof(struct sockaddr_un);
    clientFd = ::accept(serverFd, (struct sockaddr *)&client_sock_addr_un, (socklen_t *)&client_sock_addr_struct_size);
    std::cout << "accepted: fd=" << clientFd << std::endl;
    return clientFd;
}



int SocketFileServer::read(void * buffer, unsigned int size) {
    int len = 0;
    if(clientFd > 0) {


    }
    return len;
}

int SocketFileServer::write(void * buffer, unsigned int size) {
    int len = 0;
    if(clientFd > 0) {
        len = ::write(clientFd, buffer, size);
    }
    return len;
}

void SocketFileServer::closeClientFd() {
    if(clientFd > 0) {
        close(clientFd);
        clientFd = -1;
    }
}

int SocketFileServer::getClientFd() {
    return clientFd;
}