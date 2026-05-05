#include "http_parse.h"
#include <string.h>

/*
 * copy_token - copy at most (maxlen - 1) chars from src into dst, stopping at the first space, \r,
 * \n, or \0.  Always null-terminates dst.
 *
 * Returns the number of characters copied (not counting the terminator), or -1 if the token was
 * empty or too long for the buffer.
 *
 * This is a local helper; it is not exposed in the header.
 */
static int copy_token(const char *src, char *dst, int maxlen) {
    int i = 0;

    while (src[i] != ' ' && src[i] != '\r' && src[i] != '\n' && src[i] != '\0') {
        if (i == maxlen - 1) {
            // Token too long to fit with null terminator
            dst[0] = '\0';
            return -1;
        }
        dst[i] = src[i];
        i++;
    }

    dst[i] = '\0';

    // empty token is an error
    return (i == 0) ? -1 : i;
}

//
static int parse_token(const char **p, char *dest, int maxlen) {
    int len = copy_token(*p, dest, maxlen);
    if (len < 0) {
        return -1;
    }
    *p += len;
    return 0;
}

int http_parse_request_line(const char *src, struct http_request_line *out) {
    // Pointer to walk through `src`
    const char *p;

    // Guard clauses: reject obviously bad input early
    if (src == NULL || src[0] == '\0') {
        return -1;
    }

    p = src;

    // Token 1: method
    if (parse_token(&p, out->method, HTTP_METHOD_MAXLEN) < 0) {
        return -1;
    }

    if (*p != ' ') {
        // Must be followed by a single space
        return -1;
    }
    // Skip the space
    p++;

    // Token 2: path
    if (parse_token(&p, out->path, HTTP_PATH_MAXLEN) < 0) {
        return -1;
    }

    if (*p != ' ') {
        return -1;
    }
    p++;

    // Token 3: HTTP version
    if (parse_token(&p, out->version, HTTP_VERSION_MAXLEN) < 0) {
        return -1;
    }

    // After the three tokens we expect either \r\n or \n.
    // Anything else means there was a 4th token or junk on the line.
    if (*p == '\r') {
        p++;
    }
    if (*p != '\n' && *p != '\0') {
        return -1;
    }

    return 0;
}
