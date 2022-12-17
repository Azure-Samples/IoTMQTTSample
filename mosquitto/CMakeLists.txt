# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.10)

project(mosquitto)

# include the Windows Mosquitto installation
if(WIN32)
    include_directories("$ENV{ProgramFiles}/mosquitto/devel")
    link_directories("$ENV{ProgramFiles}/mosquitto/devel")
endif(WIN32)

link_libraries(mosquitto)
add_compile_definitions(CERT_PATH="${CMAKE_SOURCE_DIR}/../")

add_executable(mosquitto_telemetry src/mosquitto_telemetry.cpp)
add_executable(mosquitto_device_twin src/mosquitto_device_twin.cpp)
add_executable(mosquitto_subscribe src/mosquitto_subscribe.cpp)