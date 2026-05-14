#include "parse_request_headers.h"
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

    printf("=== parse_request_headers ===\n");

    // --- HAPPY PATH ---

    TEST("parses a single header") {
        struct http_request_header h;
        char input[] = "Host: developer.mozilla.org\r\n";
        // `rc` = Return Code, C convention for return value of function that signals
        // success/failure with an int.
        int rc = parse_request_headers(input, &h);
        CHECK(rc == 1);
        CHECK(strcmp(h.name, "Host") == 0);
        CHECK(strcmp(h.value, "developer.mozilla.org") == 0);
    }
    END_TEST;

    TEST("returns -1 for space before header name") {
        // TODO To prevent waste Count the headers first (e.g. count \r\n occurrences), then malloc
        // exactly that many.
        struct http_request_header h[MAX_HTTP_HEADERS];
        char input[] = " Host: developer.mozilla.org\r\n";
        int rc = parse_request_headers(input, &h[0]);
        printf("rc=%d\n", rc);
        CHECK(rc == -1);
    }
    END_TEST;

    TEST("parses 7 headers") {
        // TODO To prevent waste Count the headers first (e.g. count \r\n occurrences), then malloc
        // exactly that many.
        struct http_request_header h[MAX_HTTP_HEADERS];
        char input[] =
            "Host: developer.mozilla.org\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X "
            "10.9; rv:50.0) Gecko/20100101 Firefox/50.0\r\nAccept: "
            "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: "
            "en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br\r\nReferer: "
            "https://developer.mozilla.org/testpage.html\r\nConnection: keep-alive \r\n";
        // `rc` = Return Code, C convention for return value of function that signals
        // success/failure with an int.
        int rc = parse_request_headers(input, &h[0]);
        printf("rc=%d\n", rc);
        CHECK(rc == 7);
        CHECK(strcmp(h[0].name, "Host") == 0);
        CHECK(strcmp(h[0].value, "developer.mozilla.org") == 0);
        CHECK(strcmp(h[6].name, "Connection") == 0);
        CHECK(strcmp(h[6].value, "keep-alive") == 0);
    }
    END_TEST;

    PRINT_SUMMARY();
    RETURN_SUCCESS();
}
