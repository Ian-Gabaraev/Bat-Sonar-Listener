cmake_minimum_required(VERSION 3.16)
project(Bat_Sonar_Listener C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Source files ---
set(SRC_FILES
        main.c
        src/capture/capture.c
        src/capture/capture.h
        src/config/config.c
        src/config/config.h
        src/buffer/ring_buffer.c
        src/buffer/ring_buffer.h
        src/utilities/utilities.c
        src/utilities/utilities.h
        src/buffer/synchronous_single_buffer.c
        src/buffer/synchronous_single_buffer.h
        src/process/process.c
        src/process/process.h
        src/dsp/kiss_fft/kiss_fft.c
        src/dsp/kiss_fft/kiss_fftr.c
        src/dsp/kiss_fft/kiss_fft.h
        src/dsp/kiss_fft/kiss_fftr.h
)

# --- Executable ---
add_executable(Bat_Sonar_Listener ${SRC_FILES})

# --- Include directories ---
target_include_directories(Bat_Sonar_Listener PRIVATE
        /usr/include
        /usr/local/include
)

# --- Link directories ---
link_directories(
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib
)

# --- Link everything ---
target_link_libraries(Bat_Sonar_Listener
        pthread
        portaudio
        paho-mqtt3c
        cjson
        ssl
        crypto
)

# --- Definitions ---
target_compile_definitions(Bat_Sonar_Listener PRIVATE ENABLE_GUI)
