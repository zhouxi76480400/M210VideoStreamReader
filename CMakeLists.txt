cmake_minimum_required(VERSION 3.15)
project(M210VideoStreamReader)

include_directories("inc")

set(CMAKE_CXX_STANDARD 11)

add_executable(M210VideoStreamReader
        main.cpp
        ${PROJECT_SOURCE_DIR}/linux_helper/dji_linux_environment.hpp
        ${PROJECT_SOURCE_DIR}/linux_helper/dji_linux_environment.cpp
        ${PROJECT_SOURCE_DIR}/linux_helper/dji_linux_helpers.hpp
        ${PROJECT_SOURCE_DIR}/linux_helper/dji_linux_helpers.cpp
        )

target_link_libraries(M210VideoStreamReader ${PROJECT_SOURCE_DIR}/lib/libdjiosdk-core.a pthread)