cmake_minimum_required(VERSION 4.0)
project(Bat_Sonar_Listener C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Add executable
add_executable(Bat_Sonar_Listener
        main.c
        src/capture/capture.h src/capture/capture.c
        src/config/config.h src/config/config.c
)

# Include directories
include_directories(
        /usr/include
        /usr/local/include
)

# Link directories
link_directories(
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib
)

# Link libraries manually
target_link_libraries(Bat_Sonar_Listener
        portaudio
        paho-mqtt3c
        cjson
        ssl
        crypto
)
