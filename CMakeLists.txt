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
        src/utilities/utilities.c
        src/utilities/utilities.h
        src/buffer/ssb_processing.c
        src/process/process.c
        src/process/process.h
        src/dsp/kiss_fft/kiss_fft.c
        src/dsp/kiss_fft/kiss_fftr.c
        src/dsp/kiss_fft/kiss_fft.h
        src/dsp/kiss_fft/kiss_fftr.h
        src/uplink/uplink.c
        src/uplink/uplink.h
        include/buffer.h
        include/audio.h
        include/system.h
        include/reader.h
        include/config.h
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
        paho-mqtt3cs
        cjson
        ssl
        crypto
        m
)

# --- Definitions ---
target_compile_definitions(Bat_Sonar_Listener PRIVATE ENABLE_GUI)
