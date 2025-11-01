#include "utilities.h"

#include <stdio.h>

int in_array(const unsigned *arr, const size_t len, const int value) {
    for (size_t i = 0; i < len; i++) {
        if (arr[i] == value)
            return 1;
    }
    return 0;
}

void suppress_alsa_errors() {
    if (!freopen("/dev/null", "w", stderr)) {
        perror("freopen failed");
    }
}

void help() {
    printf("\n");
    printf("\033[1;36m========================================\033[0m\n");
    printf("\033[1;32m        🦇 Bat Sonar Listener v1.0\033[0m\n");
    printf("\033[1;36m========================================\033[0m\n\n");

    printf("Real-time ultrasonic capture and analysis engine.\n");
    printf("Captures ultrasonic data from compatible microphones (e.g., Pettersson "
           "U256)\n");
    printf("and performs live frequency analysis and telemetry relay via MQTT.\n\n");

    printf("\033[1;33mUsage:\033[0m\n");
    printf("  Bat_Sonar_Listener [options]\n\n");

    printf("\033[1;33mOptions:\033[0m\n");
    printf("  \033[1;32m--help, -h\033[0m               Show this help message and "
           "exit\n");
    printf("  \033[1;32m--config <file>\033[0m          Path to JSON configuration "
           "file\n");
    printf("  \033[1;32m--device <id>\033[0m            Specify audio device ID "
           "(default: auto)\n");
    printf("  \033[1;32m--buffer <size>\033[0m          Set audio buffer size in "
           "samples\n");
    printf("  \033[1;32m--mqtt\033[0m                   Enable MQTT telemetry "
           "output\n");
    printf("  \033[1;32m--gui\033[0m                    Enable Raylib-based "
           "visualization window\n");
    printf("  \033[1;32m--outfile <file>\033[0m         Save recorded samples to "
           "WAV file\n");
    printf("  \033[1;32m--verbose\033[0m                Print detailed runtime "
           "logs\n\n");

    printf("\033[1;33mExample:\033[0m\n");
    printf("  ./Bat_Sonar_Listener --config config.json --mqtt --verbose\n\n");

    printf("\033[1;33mDescription:\033[0m\n");
    printf("  The Bat Sonar Listener continuously samples ultrasonic frequencies up "
           "to\n");
    printf("  256 kHz and performs fast signal analysis (RMS, dominant frequency, "
           "etc.).\n");
    printf("  Data can be relayed to remote systems for visualization, storage, or "
           "cloud\n");
    printf("  processing. Ideal for bioacoustics research and field monitoring "
           "setups.\n\n");

    printf("\033[1;36m========================================\033[0m\n");
    printf("  Created by Ian — 2025\n");
    printf("  Part of the Bat Sonar Suite Project\n");
    printf("\033[1;36m========================================\033[0m\n\n");
};
