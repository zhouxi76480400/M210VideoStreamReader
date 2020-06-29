//
// Created by dji on 6/29/20.
//

#ifndef CPPTCPDATATRANSFER_TCPSERVER_H
#define CPPTCPDATATRANSFER_TCPSERVER_H
#define LISTEN_MAX_SOCKET_WAITING_NUM  10

#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>


class TCPServer {

public:
    TCPServer(int port);
    virtual ~TCPServer();
    int createService();
    int accept();
    int write(const void *, size_t);
    int getClientFd();
    void closeClientFd();


private:
    int listening_port;
    int serverFd;
    int clientFd;
    struct sockaddr_in serverSockAddr, clientSockAddr;


};


#endif //CPPTCPDATATRANSFER_TCPSERVER_H
