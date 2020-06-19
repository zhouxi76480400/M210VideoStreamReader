//
// Created by zhouxi on 2020/06/17.
//

#ifndef CPPFILESENDER_SOCKETFILESERVER_H
#define CPPFILESENDER_SOCKETFILESERVER_H

#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET_PATH "/tmp/sock_file_dji_osdk_camera.sock"
#define LISTEN_MAX_SOCKET_NUM  20

class SocketFileServer {

public:
    SocketFileServer();
    virtual ~SocketFileServer();
    int createService();
    int accept();
    int read(void *, unsigned int);
    int write(void *, unsigned int);
    void closeClientFd();
    int getClientFd();

private:
    int serverFd;
    int clientFd;
    struct sockaddr_un server_sock_addr_un, client_sock_addr_un;
};





#endif //CPPFILESENDER_SOCKETFILESERVER_H
