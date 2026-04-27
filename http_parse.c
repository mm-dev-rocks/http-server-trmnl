#include "http_parse.h"
#include <string.h>

/*
 * copy_token - copy at most (max-1) chars from src into dst, stopping at
 * the first space, \r, \n, or \0.  Always null-terminates dst.
 *
 * Returns the number of characters copied (not counting the terminator),
 * or -1 if the token was empty or too long for the buffer.
 *
 * This is a local helper; it is not exposed in the header.
 */
static int copy_token(const char *src, char *dst, int max) {
    int i = 0;

    while (src[i] != ' ' && src[i] != '\r' && src[i] != '\n' && src[i] != '\0') {
        if (i >= max - 1) {
            // token longer than buffer
            dst[0] = '\0';
            return -1;
        }
        dst[i] = src[i];
        i++;
    }

    dst[i] = '\0';
    return (i == 0) ? -1 : i; /* empty token is an error */
}

int http_parse_request_line(const char *src, struct http_request_head *out) {
    const char *p; /* walking pointer through src */
    int len;

    /* --- guard clauses: reject obviously bad input early --- */
    if (src == NULL || src[0] == '\0') {
        return -1;
    }

    p = src;

    /* --- token 1: method --- */
    len = copy_token(p, out->method, HTTP_METHOD_MAX);
    if (len < 0) {
        return -1;
    }

    p += len; /* advance past the method chars */

    if (*p != ' ') {
        return -1; /* must be followed by a single space */
    }
    p++; /* skip the space */

    /* --- token 2: path --- */
    len = copy_token(p, out->path, HTTP_PATH_MAX);
    if (len < 0) {
        return -1;
    }
    p += len;

    if (*p != ' ') {
        return -1;
    }
    p++;

    /* --- token 3: HTTP version --- */
    len = copy_token(p, out->version, HTTP_VERSION_MAX);
    if (len < 0) {
        return -1;
    }
    p += len;

    /*
     * After the three tokens we expect either \r\n or \n.
     * Anything else means there was a 4th token or junk on the line.
     */
    if (*p == '\r') {
        p++;
    }
    if (*p != '\n' && *p != '\0') {
        return -1;
    }

    return 0;
}
