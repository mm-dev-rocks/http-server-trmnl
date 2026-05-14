/*
 * https://datatracker.ietf.org/doc/html/rfc7230
 *
 * 3.1.  Start Line
 *
 *    An HTTP message can be either a request from client to server or a
 *    response from server to client.  Syntactically, the two types of
 *    message differ only in the start-line, which is either a request-line
 *    (for requests) or a status-line (for responses), and in the algorithm
 *    for determining the length of the message body (Section 3.3).
 *
 *    In theory, a client could receive requests and a server could receive
 *    responses, distinguishing them by their different start-line formats,
 *    but, in practice, servers are implemented to only expect a request (a
 *    response is interpreted as an unknown or invalid request method) and
 *    clients are implemented to only expect a response.
 *
 *      start-line     = request-line / status-line
 *
 * 3.1.1.  Request Line
 *
 *    A request-line begins with a method token, followed by a single space
 *    (SP), the request-target, another single space (SP), the protocol
 *    version, and ends with CRLF.
 *
 *      request-line   = method SP request-target SP HTTP-version CRLF
 *
 *    The method token indicates the request method to be performed on the
 *    target resource.  The request method is case-sensitive.
 *
 *      method         = token
 *
 *    The request methods defined by this specification can be found in
 *    Section 4 of [RFC7231], along with information regarding the HTTP
 *    method registry and considerations for defining new methods.
 *
 *    The request-target identifies the target resource upon which to apply
 *    the request, as defined in Section 5.3.
 *
 *    Recipients typically parse the request-line into its component parts
 *    by splitting on whitespace (see Section 3.5), since no whitespace is
 *    allowed in the three components.  Unfortunately, some user agents
 *    fail to properly encode or exclude whitespace found in hypertext
 *    references, resulting in those disallowed characters being sent in a
 *    request-target.
 *
 *    Recipients of an invalid request-line SHOULD respond with either a
 *    400 (Bad Request) error or a 301 (Moved Permanently) redirect with
 *    the request-target properly encoded.  A recipient SHOULD NOT attempt
 *    to autocorrect and then process the request without a redirect, since
 *    the invalid request-line might be deliberately crafted to bypass
 *    security filters along the request chain.
 *
 *    HTTP does not place a predefined limit on the length of a
 *    request-line, as described in Section 2.5.  A server that receives a
 *    method longer than any that it implements SHOULD respond with a 501
 *    (Not Implemented) status code.  A server that receives a
 *    request-target longer than any URI it wishes to parse MUST respond
 *    with a 414 (URI Too Long) status code (see Section 6.5.12 of
 *    [RFC7231]).
 *
 *    Various ad hoc limitations on request-line length are found in
 *    practice.  It is RECOMMENDED that all HTTP senders and recipients
 *    support, at a minimum, request-line lengths of 8000 octets.
 *
 */

#ifndef PARSE_REQUEST_LINE_H
#define PARSE_REQUEST_LINE_H

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
    // In C, fixed-size char arrays used to hold strings get called buffers, so these char arrays
    // are buffers.
    char method[HTTP_METHOD_MAXLEN];
    char path[HTTP_PATH_MAXLEN];
    char version[HTTP_VERSION_MAXLEN];
};

/*
 * parse_request_line - parse the first line of an HTTP request.
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
int parse_request_line(const char *src, struct http_request_line *out);

#endif // PARSE_REQUEST_LINE_H
