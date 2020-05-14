cmake_minimum_required(VERSION 2.8)
project(VideoStreamReader)

set(ONBOARDSDK_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../osdk-core")

set(MODULES_HEADER_DIR "${ONBOARDSDK_SOURCE}/modules/inc/payload")

set(FLIGHT_MODULES_HEADER_DIR "${ONBOARDSDK_SOURCE}/modules/inc/flight")

add_definitions(-DADVANCED_SENSING)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread -g -O0")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ONBOARDSDK_SOURCE}/cmake-modules/")

find_package(LibUSB REQUIRED)
include_directories(${LIBUSB_1_INCLUDE_DIRS})

set(HELPER_FUNCTIONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)

include_directories(${ONBOARDSDK_SOURCE}/api/inc)
include_directories(${ONBOARDSDK_SOURCE}/utility/inc)
include_directories(${ONBOARDSDK_SOURCE}/hal/inc)
include_directories(${ONBOARDSDK_SOURCE}/protocol/inc)
include_directories(${ONBOARDSDK_SOURCE}/platform/linux/inc)
include_directories(${HELPER_FUNCTIONS_DIR})
include_directories(${MODULES_HEADER_DIR})
include_directories(${FLIGHT_MODULES_HEADER_DIR})

find_package( OpenCV QUIET )
if (OpenCV_FOUND)
    message( "\n${PROJECT_NAME}...")
    message( STATUS "Found OpenCV installed in the system, will use it to display image in AdvancedSensing APIs")
    message( STATUS " - Includes: ${OpenCV_INCLUDE_DIRS}")
    message( STATUS " - Libraries: ${OpenCV_LIBRARIES}")
    add_definitions(-DOPEN_CV_INSTALLED)
else()
    message( STATUS "Did not find OpenCV in the system, image data is inside RecvContainer as raw data")
endif ()

add_executable(${PROJECT_NAME}
        ${HELPER_FUNCTIONS_DIR}/dji_linux_environment.cpp
        ${HELPER_FUNCTIONS_DIR}/dji_linux_helpers.cpp
        main.cpp
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
        djiosdk-core
        )

if (OpenCV_FOUND)
    target_link_libraries(${PROJECT_NAME}
            ${OpenCV_LIBS}
            )
endif ()