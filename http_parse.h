/*
 * 
 * https://datatracker.ietf.org/doc/html/rfc2616#section-4
 * 
 * 4 HTTP Message
 * 4.1 Message Types
 * 
 * HTTP messages consist of requests from client to server and responses from server to client.
 * 
 *    HTTP-message   = Request | Response     ; HTTP/1.1 messages
 * 
 * Request (section 5) and Response (section 6) messages use the generic message format of RFC 822 [9]
 * for transferring entities (the payload of the message). Both types of message consist of a
 * start-line, zero or more header fields (also known as "headers"), an empty line (i.e., a line with
 * nothing preceding the CRLF) indicating the end of the header fields, and possibly a message-body.
 * 
 *     generic-message = start-line
 *                       *(message-header CRLF)
 *                       CRLF
 *                       [ message-body ]
 *     start-line      = Request-Line | Status-Line
 * 
 * In the interest of robustness, servers SHOULD ignore any empty line(s) received where a Request-Line
 * is expected. In other words, if the server is reading the protocol stream at the beginning of a
 * message and receives a CRLF first, it should ignore the CRLF.
 * 
 * Certain buggy HTTP/1.0 client implementations generate extra CRLF's after a POST request. To restate
 * what is explicitly forbidden by the BNF, an HTTP/1.1 client MUST NOT preface or follow a request
 * with an extra CRLF.
 * 
 */

#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

/*
 * Maximum lengths chosen to be generous but bounded.
 * Real servers enforce tighter limits (RFC 7230 recommends at least 8000
 * bytes for a request target; we keep it small for learning purposes).
 */
#define HTTP_METHOD_MAXLEN 16
#define HTTP_PATH_MAXLEN 256
#define HTTP_VERSION_MAXLEN 16

/*
 * Holds the three tokens from an HTTP request line:
 *   GET /path HTTP/1.1\r\n
 *   ^^^ ^^^^^ ^^^^^^^^^^^^
 */
struct http_request_line {
    // In C, fixed-size char arrays used to hold strings get called buffers, so these char arrays are buffers.
    char method[HTTP_METHOD_MAXLEN];
    char path[HTTP_PATH_MAXLEN];
    char version[HTTP_VERSION_MAXLEN];
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
int http_parse_request_line(const char *src, struct http_request_line *out);

#endif // HTTP_PARSE_H
