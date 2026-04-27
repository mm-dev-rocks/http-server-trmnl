#ifndef TEST_H
#define TEST_H

/*
 * Minimal test harness - no dependencies, just C89 standard library.
 *
 * Usage:
 *   TEST("it parses the method") {
 *       CHECK(strcmp(req.method, "GET") == 0);
 *   }
 */

#include <stdio.h>
#include <stdlib.h>

/* Global counters - defined in test runner's main() translation unit */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name)                                                             \
  do {                                                                         \
    int _test_ok = 1;                                                          \
    tests_run++;                                                               \
    printf("  [ ] %s ... ", name);                                             \
    fflush(stdout);

#define CHECK(expr)                                                            \
  if (!(expr)) {                                                               \
    _test_ok = 0;                                                              \
    printf("\n      FAIL: %s:%d: %s", __FILE__, __LINE__, #expr);              \
  }

#define END_TEST                                                               \
  if (_test_ok) {                                                              \
    tests_passed++;                                                            \
    printf("OK\n");                                                            \
  } else {                                                                     \
    tests_failed++;                                                            \
    printf("\n");                                                              \
  }                                                                            \
  }                                                                            \
  while (0)

#define PRINT_SUMMARY()                                                        \
  do {                                                                         \
    printf("\n%d/%d passed", tests_passed, tests_run);                         \
    if (tests_failed) {                                                        \
      printf(", %d FAILED\n", tests_failed);                                   \
      return 1;                                                                \
    }                                                                          \
    printf("\n");                                                              \
  } while (0)

#endif /* TEST_H */
