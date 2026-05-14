#include "platform.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        /* No arguments: detect from compile-time macros */
        printf("test_results/%s-%s-%s-%s.txt\n", PLATFORM_OS, PLATFORM_ARCH, PLATFORM_LIBC,
               PLATFORM_CSTD);
    } else if (argc == 5) {
        /* Four arguments: os arch libc cstd — for ESP32 script */
        printf("test_results/%s-%s-%s-%s.txt\n", argv[1], argv[2], argv[3], argv[4]);
    } else {
        fprintf(stderr, "Usage: platform_id [os arch libc cstd]\n");
        return 1;
    }
    return 0;
}
