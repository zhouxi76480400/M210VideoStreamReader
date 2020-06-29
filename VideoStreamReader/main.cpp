#include "dji_vehicle.hpp"
#include <iostream>
#include "dji_linux_helpers.hpp"

#include "SocketFileServer.h"
#include "TCPServer.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

#define kTrueWidth 640
#define kTrueHeight 512


#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace DJI::OSDK;
using namespace std;

bool isTCP = true;
SocketFileServer server;
TCPServer server_tcp(20002);

void show_rgb(CameraRGBImage img, char* name) {
    void * data = img.rawData.data();
    size_t size = img.height * img.width * 3;
    //run mat
    Mat mat(img.height, img.width, CV_8UC3, data, img.width*3);
    cvtColor(mat, mat, COLOR_RGB2BGR);
    // crop
    Mat newImage;
    int tmpWidth = (((float)kTrueWidth / (float)kTrueHeight) * (float)img.height);
    int start_x = ((float)img.width - (float)tmpWidth) / 2.0f;
    Rect rect(start_x, 0, tmpWidth, img.height);
    newImage = mat(rect);
    mat.release();
    mat = newImage;
    newImage.release();
    // resize
    resize(mat, newImage, Size(kTrueWidth, kTrueHeight), 0, 0, INTER_LINEAR);
    mat.release();
    mat = newImage;
    newImage.release();
    // get data and size
    void * mat_data = mat.data;
    int data_size = kTrueWidth * kTrueHeight * 3 * sizeof(uint8_t);
    //
    if(isTCP) {
        server_tcp.write(mat_data,data_size);
    } else {
        server.write(mat_data,data_size);
    }
    mat.release();
}

void signal_pipe_handler(int sig) {
    cout << "SIGPIPE !" << endl;
    if(isTCP) {
        server_tcp.closeClientFd();
    } else {
        server.closeClientFd();
    }
}

Vehicle* vehicle;

void my_handler(int);

bool isRunning = true;

int main(int argc, char** argv) {
    signal(SIGPIPE,signal_pipe_handler);
    int ret = -1;
    if(isTCP) {
        ret = server_tcp.createService();
    }else {
        ret = server.createService();
    }
    if(ret >= 0) {
        // Setup OSDK.
        bool enableAdvancedSensing = true;
        LinuxSetup linuxEnvironment(argc, argv, enableAdvancedSensing);
        vehicle = linuxEnvironment.getVehicle();
        const char *acm_dev = linuxEnvironment.getEnvironment()->getDeviceAcm().c_str();
        vehicle->advancedSensing->setAcmDevicePath(acm_dev);
        if (vehicle == nullptr) {
            perror("Vehicle not initialized, exiting.");
            exit(-1);
        }
        bool mainCamResult = vehicle->advancedSensing->startMainCameraStream();
        if(mainCamResult) {
            CameraRGBImage mainImg;
            char mainName[] = "MAIN_CAM";

            struct sigaction sigIntHandler;

            sigIntHandler.sa_handler = my_handler;
            sigemptyset(&sigIntHandler.sa_mask);
            sigIntHandler.sa_flags = 0;

            sigaction(SIGINT, &sigIntHandler, NULL);

            isRunning = true;

            while (isRunning) {
                int client_fd = -1;
                if(isTCP) {
                    client_fd = server_tcp.getClientFd();
                } else {
                    client_fd = server.getClientFd();
                }
                if(client_fd >= 0) {
                    if(mainCamResult && vehicle->advancedSensing->newMainCameraImageReady()) {
                        if(vehicle->advancedSensing->getMainCameraImage(mainImg)) {
                            show_rgb(mainImg, mainName);
                        }
                    }
                    usleep(2e4);
                } else {
                    if(isTCP) {
                        server_tcp.accept();
                    }else {
                        server.accept();
                    }
                }
            }
            vehicle->advancedSensing->stopMainCameraStream();
        } else {
            perror("Failed to open Main Camera");
            exit(-1);
        }
    } else {
        perror("cannot create unix socket");
        exit(-1);
    }
    return 0;
}

void my_handler(int s) {
    isRunning = false;
    cout << "Caught signal" << s << endl;
}