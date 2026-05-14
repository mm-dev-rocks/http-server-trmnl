#include "parse_request_line.h"
#include "platform.h"
#include "test.h"
#include <string.h>

/*
 * parse_request_line() should:
 *   - extract method, path, version from "GET /foo HTTP/1.1\r\n"
 *   - return 0 on success, -1 on failure
 *   - never write past the supplied buffer sizes
 *   - treat malformed input as an error
 */

// ESP-IDF uses `app_main` instead of `main`
// On a hosted platform, main() returns an int to the operating system — that's the exit code. 0
// means success, non-zero means something went wrong. The OS receives it and can act on it, for
// example your Makefile uses || exit 1 to stop if a test binary returns non-zero. On the ESP32
// there's no OS to return to. app_main() is called by ESP-IDF's startup code and when it returns,
// the firmware either loops forever or reboots — there's nowhere to send a return code. So void
// makes sense.
#ifdef CONFIG_IDF_TARGET_ESP32
void app_main(void)
#else
int main(void)
#endif
{
#ifndef CONFIG_IDF_TARGET_ESP32
    const char *filename = getenv("TEST_OUTPUT_FILE");
    if (filename) {
        freopen(filename, "a", stdout);
    }
#endif

    printf("=== parse_request_line ===\n");

    // --- HAPPY PATH ---

    TEST("parses a simple GET request") {
        struct http_request_line r;
        char input[] = "GET /index.html HTTP/1.1\r\n";
        // `rc` = Return Code, C convention for return value of function that signals
        // success/failure with an int.
        int rc = parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.method, "GET") == 0);
        CHECK(strcmp(r.path, "/index.html") == 0);
        CHECK(strcmp(r.version, "HTTP/1.1") == 0);
    }
    END_TEST;

    TEST("parses a POST request") {
        struct http_request_line r;
        char input[] = "POST /submit HTTP/1.0\r\n";
        int rc = parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.method, "POST") == 0);
        CHECK(strcmp(r.path, "/submit") == 0);
        CHECK(strcmp(r.version, "HTTP/1.0") == 0);
    }
    END_TEST;

    TEST("parses a root path") {
        struct http_request_line r;
        char input[] = "GET / HTTP/1.1\r\n";
        int rc = parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.path, "/") == 0);
    }
    END_TEST;

    TEST("parses a path with query string") {
        struct http_request_line r;
        char input[] = "GET /search?q=hello HTTP/1.1\r\n";
        int rc = parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.path, "/search?q=hello") == 0);
    }
    END_TEST;

    // --- ERROR CASES ---

    TEST("returns -1 for empty input") {
        struct http_request_line r;
        char input[] = "";
        int rc = parse_request_line(input, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    TEST("returns -1 when method is missing") {
        struct http_request_line r;
        char input[] = "/index.html HTTP/1.1\r\n";
        int rc = parse_request_line(input, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    TEST("returns -1 when version is missing") {
        struct http_request_line r;
        char input[] = "GET /index.html\r\n";
        int rc = parse_request_line(input, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    TEST("returns -1 for NULL input") {
        struct http_request_line r;
        int rc = parse_request_line(NULL, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    PRINT_SUMMARY();
    RETURN_SUCCESS();
}
