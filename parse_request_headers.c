#include "parse_request_headers.h"
#include "parse_utils.h"
#include <string.h>

static int parse_name(const char **p, char *dest, int maxlen) {
    int len = copy_to_delim_or_eol(*p, dest, maxlen, ':');
    if (len < 0) {
        return -1;
    }
    *p += len;
    return len;
}

static int parse_value(const char **p, char *dest, int maxlen) {
    int len = copy_to_eol(*p, dest, maxlen);
    if (len < 0) {
        return -1;
    }
    *p += len;
    return len;
}

int parse_request_header(const char **pp, struct http_request_header *out) {

    // Guard clauses: reject obviously bad input early
    if (*pp == NULL || **pp == '\0') {
        return -1;
    }

    // Pointer to walk through `src`
    const char *p = *pp;

    // Fail if header name starts with whitespace
    // TODO The HTTP spec defines valid header name characters as "token" characters ---
    // alphanumeric plus a specific set of symbols (!#$%&'*+-.^_|~`). So a fully correct check would
    // reject anything outside that set, not just whitespace.
    if (*p == ' ' || *p == '\t') {
        return -1;
    }

    // Token 1: name
    if (parse_name(&p, out->name, HTTP_HEADER_NAME_MAXLEN) < 0) {
        return -1;
    }
    // Must be followed by a colon
    if (*p != ':') {
        return -1;
    }
    // Found the colon, skip it
    p++;

    // Skip Optional White Space after colon
    skip_lws(&p);

    // Token 2: value
    int value_len = parse_value(&p, out->value, HTTP_HEADER_VALUE_MAXLEN);
    if (value_len < 0) {
        return -1;
    }
    // Skip Optional White Space after value
    trim_trailing_lws(out->value, value_len);

    // After both tokens we expect either \r\n, \n or \0.
    // Anything else means there was a 3rd token or junk on the line.
    if (*p == '\r') {
        // Advance
        p++;
    }
    if (*p == '\n') {
        // Advance again if was \r\n
        p++;
    } else if (*p != '\0') {
        return -1;
    }

    *pp = p;

    return 0;
}

int parse_request_headers(const char *src, struct http_request_header *out) {
    const char *p = src;
    int i;
    for (i = 0; i < MAX_HTTP_HEADERS; i++) {
        if (parse_request_header(&p, &out[i]) == -1) {
            // Failed on first header, bad input
            if (i == 0) {
                return -1;
            }
            break;
        }
    }

    return i;
}
