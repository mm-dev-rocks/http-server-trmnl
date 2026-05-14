#ifndef TEST_H
#define TEST_H

/*
 * Minimal test harness - no dependencies
 *
 * Usage:
 *   TEST("it parses the method") {
 *       CHECK(strcmp(req.method, "GET") == 0);
 *   }
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef CONFIG_IDF_TARGET_ESP32
// In `app_main` on esp32 we can't return ints (there's no OS to receive them)
#define RETURN_FAILURE()
#define RETURN_SUCCESS()
#else
#define RETURN_FAILURE() return 1
#define RETURN_SUCCESS() return 0
#endif

// Global counters - defined in test runner's main() translation unit
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name)                                                                                 \
    do {                                                                                           \
        int _test_ok = 1;                                                                          \
        tests_run++;                                                                               \
        printf(" - %s ... ", name);                                                                \
        fflush(stdout);

#define CHECK(expr)                                                                                \
    if (!(expr)) {                                                                                 \
        _test_ok = 0;                                                                              \
        printf("\n      FAIL: %s:%d: %s", __FILE__, __LINE__, #expr);                              \
    }

#define END_TEST                                                                                   \
    if (_test_ok) {                                                                                \
        tests_passed++;                                                                            \
        printf("PASS\n");                                                                          \
    } else {                                                                                       \
        tests_failed++;                                                                            \
        printf("\n");                                                                              \
    }                                                                                              \
    }                                                                                              \
    while (0)

#define PRINT_SUMMARY()                                                                            \
    do {                                                                                           \
        printf("\n%d/%d passed\n", tests_passed, tests_run);                                       \
        if (tests_failed) {                                                                        \
            printf(", %d FAILED\n", tests_failed);                                                 \
            RETURN_FAILURE();                                                                      \
        }                                                                                          \
        printf("\n");                                                                              \
    } while (0)

#endif // TEST_H
