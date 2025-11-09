#include "utilities.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "../../include/device.h"

int in_array(const unsigned *arr, const size_t len, const int value) {
    for (size_t i = 0; i < len; i++) {
        if (arr[i] == value)
            return 1;
    }
    return 0;
}

int input_device_id(const AvailableDevice *devices) {
    int c;
    printf("\u25b6 Please enter device #: \n");
    while ((c = getchar()) != EOF && !isspace(c)) {
        const int n = c - '0';
        if (in_array(devices->device_ids, devices->device_count, n)) {
            return n;
        }
    }
    printf("Not found. Using default # 0\n");
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
    printf("  Created by Ian Gabaraev — 2025\n");
    printf("  Part of the Bat Sonar Suite Project\n");
    printf("\033[1;36m========================================\033[0m\n\n");
};

void set_hostname(SystemSummary *_s) {
    char name[256];
    if (gethostname(name, sizeof(name)) == 0) {
        strncpy(_s->hostname, name, sizeof(name) - 1);
    }
}

void set_cpu_model(SystemSummary *_s) {
    FILE *f = fopen("/proc/cpuinfo", "r");
    char line[256];
    if (!f)
        return;

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "model name", 10) == 0) {
            for (int i = 0; line[i] != '\0'; i++) {
                if (line[i] == '\n') {
                    line[i] = '\0';
                    break;
                }
            }
            snprintf(_s->cpu_name, sizeof(_s->cpu_name), "%s", line + 13);
            break;
        }
    }
    fclose(f);
}

void set_ram(SystemSummary *_s) {
    struct sysinfo info;
    if (sysinfo(&info) == 0)
        _s->ram_mb = (info.totalram * info.mem_unit) / (1024.0 * 1024.0);
}

void collect_info(SystemSummary *summary) {
    set_hostname(summary);
    set_cpu_model(summary);
    set_ram(summary);
}
