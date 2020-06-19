#include "dji_vehicle.hpp"
#include <iostream>
#include "dji_linux_helpers.hpp"
//#include <pthread.h>
#include "SocketFileServer.h"

//#ifdef OPEN_CV_INSTALLED
//#include "opencv2/opencv.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#endif


#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace DJI::OSDK;
//using namespace cv;
using namespace std;

SocketFileServer server;

void show_rgb(CameraRGBImage img, char* name) {
//    cout << "#### Got image from:\t" << string(name) << endl;
//#ifdef OPEN_CV_INSTALLED
//    Mat mat(img.height, img.width, CV_8UC3, img.rawData.data(), img.width*3);
//    cvtColor(mat, mat, COLOR_RGB2BGR);
//    imshow(name,mat);
//    cv::waitKey(1);
//#endif
    void * data = img.rawData.data();
    size_t size = img.height * img.width * 3;
//    cout << "Got img data: " << data << " size: " << size << "tid=" << pthread_self() << endl;
    int write_size = server.write(data,size);
//    cout << "write: " << write_size << endl;
}

void signal_pipe_handler(int sig) {
    cout << "SIGPIPE !" << endl;
    server.closeClientFd();
}

Vehicle* vehicle;

void my_handler(int);

bool isRunning = true;

int main(int argc, char** argv) {
    signal(SIGPIPE,signal_pipe_handler);
    int ret = server.createService();
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
                if(server.getClientFd() >= 0) {
                    if(mainCamResult && vehicle->advancedSensing->newMainCameraImageReady()) {
                        if(vehicle->advancedSensing->getMainCameraImage(mainImg)) {
                            show_rgb(mainImg, mainName);
                        }
                    }
                    usleep(2e4);
                } else {
                    server.accept();
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