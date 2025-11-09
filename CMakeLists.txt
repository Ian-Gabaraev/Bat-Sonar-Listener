cmake_minimum_required(VERSION 3.16)
project(Bat_Sonar_Listener C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Source files ---
set(SRC_FILES
        main.c

        # -- C source code
        src/capture.c
        src/config/config.c
        src/config/config.h
        src/utilities.c
        src/fsb_processing.c
        src/psb_processing.c
        src/process.c
        src/uplink.c
        src/logger.c

        # --- KissFFT source code
        src/dsp/kiss_fft/kiss_fft.c
        src/dsp/kiss_fft/kiss_fftr.c
        src/dsp/kiss_fft/kiss_fft.h
        src/dsp/kiss_fft/kiss_fftr.h

        # --- Public headers ---
        include/buffer.h
        include/audio.h
        include/system.h
        include/config.h
        include/device.h
        include/process.h
        include/uplink.h
        include/utilities.h
        include/logger.h
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
