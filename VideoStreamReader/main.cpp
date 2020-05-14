#include "dji_vehicle.hpp"
#include <iostream>
#include "dji_linux_helpers.hpp"

#ifdef OPEN_CV_INSTALLED
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#endif


#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace DJI::OSDK;
using namespace cv;
using namespace std;

void show_rgb(CameraRGBImage img, char* name) {
    cout << "#### Got image from:\t" << string(name) << endl;
#ifdef OPEN_CV_INSTALLED
    Mat mat(img.height, img.width, CV_8UC3, img.rawData.data(), img.width*3);
    cvtColor(mat, mat, COLOR_RGB2BGR);
    imshow(name,mat);
    cv::waitKey(1);
#endif
}

Vehicle* vehicle;

void my_handler(int);

bool isRunning = true;

int main(int argc, char** argv) {
    // Setup OSDK.
    bool enableAdvancedSensing = true;
    LinuxSetup linuxEnvironment(argc, argv, enableAdvancedSensing);
    vehicle = linuxEnvironment.getVehicle();
    const char *acm_dev = linuxEnvironment.getEnvironment()->getDeviceAcm().c_str();
    vehicle->advancedSensing->setAcmDevicePath(acm_dev);
    if (vehicle == NULL) {
        std::cout << "Vehicle not initialized, exiting.\n";
        return -1;
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
            if(mainCamResult && vehicle->advancedSensing->newMainCameraImageReady()) {
                if(vehicle->advancedSensing->getMainCameraImage(mainImg)) {
                    show_rgb(mainImg, mainName);
                }
            }
            usleep(2e4);
        }
        vehicle->advancedSensing->stopMainCameraStream();
    } else {
        cout << "Failed to open Main Camera" << endl;
    }

    return 0;
}

void my_handler(int s) {
    isRunning = false;
    cout << "Caught signal" << s << endl;
}