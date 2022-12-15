# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

project(mosquitto)

# Include the Windows Mosquitto installation
if(WIN32)
    set(MOSQUITTO_PATH "$ENV{ProgramFiles}/mosquitto/devel")
    include_directories(${MOSQUITTO_PATH})
    link_directories(${MOSQUITTO_PATH})
endif(WIN32)

link_libraries(mosquitto)

add_executable(mosquitto_telemetry src/mosquitto_telemetry.cpp)
add_executable(mosquitto_device_twin src/mosquitto_device_twin.cpp)
add_executable(mosquitto_subscribe src/mosquitto_subscribe.cpp)