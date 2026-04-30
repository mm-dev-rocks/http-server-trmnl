#include "http_parse.h"
#include "platform.h"
#include "test.h"
#include <string.h>

/*
 * http_parse_request_line() should:
 *   - extract method, path, version from "GET /foo HTTP/1.1\r\n"
 *   - return 0 on success, -1 on failure
 *   - never write past the supplied buffer sizes
 *   - treat malformed input as an error
 */

int main(void) {
    // Make a filename representing the current system
    char filename[256];
    snprintf(filename, sizeof(filename), "test_results/%s-%s-%s-%s.txt", PLATFORM_OS, PLATFORM_ARCH,
             PLATFORM_LIBC, PLATFORM_CSTD);
    // `freopen` redirects stdout to the file
    freopen(filename, "w", stdout);

    printf("=== http_parse_request_line ===\n");

    // --- HAPPY PATH ---

    TEST("parses a simple GET request") {
        struct http_request_line r;
        char input[] = "GET /index.html HTTP/1.1\r\n";
        // `rc` = Return Code, C convention for return value of function that signals
        // success/failure with an int.
        int rc = http_parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.method, "GET") == 0);
        CHECK(strcmp(r.path, "/index.html") == 0);
        CHECK(strcmp(r.version, "HTTP/1.1") == 0);
    }
    END_TEST;

    TEST("parses a POST request") {
        struct http_request_line r;
        char input[] = "POST /submit HTTP/1.0\r\n";
        int rc = http_parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.method, "POST") == 0);
        CHECK(strcmp(r.path, "/submit") == 0);
        CHECK(strcmp(r.version, "HTTP/1.0") == 0);
    }
    END_TEST;

    TEST("parses a root path") {
        struct http_request_line r;
        char input[] = "GET / HTTP/1.1\r\n";
        int rc = http_parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.path, "/") == 0);
    }
    END_TEST;

    TEST("parses a path with query string") {
        struct http_request_line r;
        char input[] = "GET /search?q=hello HTTP/1.1\r\n";
        int rc = http_parse_request_line(input, &r);
        CHECK(rc == 0);
        CHECK(strcmp(r.path, "/search?q=hello") == 0);
    }
    END_TEST;

    // --- ERROR CASES ---

    TEST("returns -1 for empty input") {
        struct http_request_line r;
        char input[] = "";
        int rc = http_parse_request_line(input, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    TEST("returns -1 when method is missing") {
        struct http_request_line r;
        char input[] = "/index.html HTTP/1.1\r\n";
        int rc = http_parse_request_line(input, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    TEST("returns -1 when version is missing") {
        struct http_request_line r;
        char input[] = "GET /index.html\r\n";
        int rc = http_parse_request_line(input, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    TEST("returns -1 for NULL input") {
        struct http_request_line r;
        int rc = http_parse_request_line(NULL, &r);
        CHECK(rc == -1);
    }
    END_TEST;

    PRINT_SUMMARY();
    return 0;
}
