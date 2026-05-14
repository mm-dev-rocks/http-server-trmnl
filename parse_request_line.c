#include "parse_request_line.h"
#include "parse_utils.h"
#include <string.h>

static int parse_token(const char **p, char *dest, int maxlen) {
    int len = copy_to_delim_or_eol(*p, dest, maxlen, ' ');
    if (len < 0) {
        return -1;
    }
    *p += len;
    return 0;
}

int parse_request_line(const char *src, struct http_request_line *out) {

    // Guard clauses: reject obviously bad input early
    if (src == NULL || src[0] == '\0') {
        return -1;
    }

    // Pointer to walk through `src`
    const char *p = src;

    // Token 1: method
    if (parse_token(&p, out->method, HTTP_METHOD_MAXLEN) < 0) {
        return -1;
    }
    // Must be followed by a single space
    if (*p != ' ') {
        return -1;
    }
    // Found the space, skip it
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
