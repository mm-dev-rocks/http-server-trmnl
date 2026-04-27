#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

/*
 * Maximum lengths chosen to be generous but bounded.
 * Real servers enforce tighter limits (RFC 7230 recommends at least 8000
 * bytes for a request target; we keep it small for learning purposes).
 */
#define HTTP_METHOD_MAX 16
#define HTTP_PATH_MAX 256
#define HTTP_VERSION_MAX 16

/*
 * Holds the three tokens from an HTTP request line:
 *   GET /path HTTP/1.1\r\n
 *   ^^^  ^^^^  ^^^^^^^^
 */
struct http_request_head {
    char method[HTTP_METHOD_MAX];
    char path[HTTP_PATH_MAX];
    char version[HTTP_VERSION_MAX];
};

/*
 * http_parse_request_line - parse the first line of an HTTP request.
 *
 * @src  - null-terminated string containing (at least) the request line.
 *         The string is treated as read-only; we work on a copy internally.
 * @out  - caller-supplied struct to fill in.
 *
 * Returns  0 on success.
 * Returns -1 if src is NULL, empty, or does not contain exactly three
 *            space-separated tokens ending with \r\n or \n.
 *
 * On failure, the contents of *out are undefined.
 */
int http_parse_request_line(const char *src, struct http_request_head *out);

#endif /* HTTP_PARSE_H */
