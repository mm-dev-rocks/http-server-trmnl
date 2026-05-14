/*
 * https://datatracker.ietf.org/doc/html/rfc7230
 *
 * 3.2.  Header Fields
 *
 *    Each header field consists of a case-insensitive field name followed
 *    by a colon (":"), optional leading whitespace, the field value, and
 *    optional trailing whitespace.
 *
 *      header-field   = field-name ":" OWS field-value OWS
 *
 *      field-name     = token
 *      field-value    = *( field-content / obs-fold )
 *      field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
 *      field-vchar    = VCHAR / obs-text
 *
 *      obs-fold       = CRLF 1*( SP / HTAB )
 *                     ; obsolete line folding
 *                     ; see Section 3.2.4
 *
 *    The field-name token labels the corresponding field-value as having
 *    the semantics defined by that header field.  For example, the Date
 *    header field is defined in Section 7.1.1.2 of [RFC7231] as containing
 *    the origination timestamp for the message in which it appears.
 *
 * 3.2.1.  Field Extensibility
 *
 *    Header fields are fully extensible: there is no limit on the
 *    introduction of new field names, each presumably defining new
 *    semantics, nor on the number of header fields used in a given
 *    message.  Existing fields are defined in each part of this
 *    specification and in many other specifications outside this document
 *    set.
 *
 *    New header fields can be defined such that, when they are understood
 *    by a recipient, they might override or enhance the interpretation of
 *    previously defined header fields, define preconditions on request
 *    evaluation, or refine the meaning of responses.
 *
 *    A proxy MUST forward unrecognized header fields unless the field-name
 *    is listed in the Connection header field (Section 6.1) or the proxy
 *    is specifically configured to block, or otherwise transform, such
 *    fields.  Other recipients SHOULD ignore unrecognized header fields.
 *    These requirements allow HTTP's functionality to be enhanced without
 *    requiring prior update of deployed intermediaries.
 *
 *    All defined header fields ought to be registered with IANA in the
 *    "Message Headers" registry, as described in Section 8.3 of [RFC7231].
 *
 * 3.2.2.  Field Order
 *
 *    The order in which header fields with differing field names are
 *    received is not significant.  However, it is good practice to send
 *    header fields that contain control data first, such as Host on
 *    requests and Date on responses, so that implementations can decide
 *    when not to handle a message as early as possible.  A server MUST NOT
 *    apply a request to the target resource until the entire request
 *    header section is received, since later header fields might include
 *    conditionals, authentication credentials, or deliberately misleading
 *    duplicate header fields that would impact request processing.
 *
 *    A sender MUST NOT generate multiple header fields with the same field
 *    name in a message unless either the entire field value for that
 *    header field is defined as a comma-separated list [i.e., #(values)]
 *    or the header field is a well-known exception (as noted below).
 *
 *    A recipient MAY combine multiple header fields with the same field
 *    name into one "field-name: field-value" pair, without changing the
 *    semantics of the message, by appending each subsequent field value to
 *    the combined field value in order, separated by a comma.  The order
 *    in which header fields with the same field name are received is
 *    therefore significant to the interpretation of the combined field
 *    value; a proxy MUST NOT change the order of these field values when
 *    forwarding a message.
 *
 *       Note: In practice, the "Set-Cookie" header field ([RFC6265]) often
 *       appears multiple times in a response message and does not use the
 *       list syntax, violating the above requirements on multiple header
 *       fields with the same name.  Since it cannot be combined into a
 *       single field-value, recipients ought to handle "Set-Cookie" as a
 *       special case while processing header fields.  (See Appendix A.2.3
 *       of [Kri2001] for details.)
 *
 * 3.2.3.  Whitespace
 *
 *    This specification uses three rules to denote the use of linear
 *    whitespace: OWS (optional whitespace), RWS (required whitespace), and
 *    BWS ("bad" whitespace).
 *
 *    The OWS rule is used where zero or more linear whitespace octets
 *    might appear.  For protocol elements where optional whitespace is
 *    preferred to improve readability, a sender SHOULD generate the
 *    optional whitespace as a single SP; otherwise, a sender SHOULD NOT
 *    generate optional whitespace except as needed to white out invalid or
 *    unwanted protocol elements during in-place message filtering.
 *
 *    The RWS rule is used when at least one linear whitespace octet is
 *    required to separate field tokens.  A sender SHOULD generate RWS as a
 *    single SP.
 *
 *    The BWS rule is used where the grammar allows optional whitespace
 *    only for historical reasons.  A sender MUST NOT generate BWS in
 *    messages.  A recipient MUST parse for such bad whitespace and remove
 *    it before interpreting the protocol element.
 *
 *      OWS            = *( SP / HTAB )
 *                     ; optional whitespace
 *      RWS            = 1*( SP / HTAB )
 *                     ; required whitespace
 *      BWS            = OWS
 *                     ; "bad" whitespace
 *
 * 3.2.4.  Field Parsing
 *
 *    Messages are parsed using a generic algorithm, independent of the
 *    individual header field names.  The contents within a given field
 *    value are not parsed until a later stage of message interpretation
 *    (usually after the message's entire header section has been
 *    processed).  Consequently, this specification does not use ABNF rules
 *    to define each "Field-Name: Field Value" pair, as was done in
 *    previous editions.  Instead, this specification uses ABNF rules that
 *    are named according to each registered field name, wherein the rule
 *    defines the valid grammar for that field's corresponding field values
 *    (i.e., after the field-value has been extracted from the header
 *    section by a generic field parser).
 *
 *    No whitespace is allowed between the header field-name and colon.  In
 *    the past, differences in the handling of such whitespace have led to
 *    security vulnerabilities in request routing and response handling.  A
 *    server MUST reject any received request message that contains
 *    whitespace between a header field-name and colon with a response code
 *    of 400 (Bad Request).  A proxy MUST remove any such whitespace from a
 *    response message before forwarding the message downstream.
 *
 *    A field value might be preceded and/or followed by optional
 *    whitespace (OWS); a single SP preceding the field-value is preferred
 *    for consistent readability by humans.  The field value does not
 *    include any leading or trailing whitespace: OWS occurring before the
 *    first non-whitespace octet of the field value or after the last
 *    non-whitespace octet of the field value ought to be excluded by
 *    parsers when extracting the field value from a header field.
 *
 *    Historically, HTTP header field values could be extended over
 *    multiple lines by preceding each extra line with at least one space
 *    or horizontal tab (obs-fold).  This specification deprecates such
 *    line folding except within the message/http media type
 *    (Section 8.3.1).  A sender MUST NOT generate a message that includes
 *    line folding (i.e., that has any field-value that contains a match to
 *    the obs-fold rule) unless the message is intended for packaging
 *    within the message/http media type.
 *
 *    A server that receives an obs-fold in a request message that is not
 *    within a message/http container MUST either reject the message by
 *    sending a 400 (Bad Request), preferably with a representation
 *    explaining that obsolete line folding is unacceptable, or replace
 *    each received obs-fold with one or more SP octets prior to
 *    interpreting the field value or forwarding the message downstream.
 *
 *    A proxy or gateway that receives an obs-fold in a response message
 *    that is not within a message/http container MUST either discard the
 *    message and replace it with a 502 (Bad Gateway) response, preferably
 *    with a representation explaining that unacceptable line folding was
 *    received, or replace each received obs-fold with one or more SP
 *    octets prior to interpreting the field value or forwarding the
 *    message downstream.
 *
 *    A user agent that receives an obs-fold in a response message that is
 *    not within a message/http container MUST replace each received
 *    obs-fold with one or more SP octets prior to interpreting the field
 *    value.
 *
 *    Historically, HTTP has allowed field content with text in the
 *    ISO-8859-1 charset [ISO-8859-1], supporting other charsets only
 *    through use of [RFC2047] encoding.  In practice, most HTTP header
 *    field values use only a subset of the US-ASCII charset [USASCII].
 *    Newly defined header fields SHOULD limit their field values to
 *    US-ASCII octets.  A recipient SHOULD treat other octets in field
 *    content (obs-text) as opaque data.
 *
 * 3.2.5.  Field Limits
 *
 *    HTTP does not place a predefined limit on the length of each header
 *    field or on the length of the header section as a whole, as described
 *    in Section 2.5.  Various ad hoc limitations on individual header
 *    field length are found in practice, often depending on the specific
 *    field semantics.
 *
 *    A server that receives a request header field, or set of fields,
 *    larger than it wishes to process MUST respond with an appropriate 4xx
 *    (Client Error) status code.  Ignoring such header fields would
 *    increase the server's vulnerability to request smuggling attacks
 *    (Section 9.5).
 *
 *    A client MAY discard or truncate received header fields that are
 *    larger than the client wishes to process if the field semantics are
 *    such that the dropped value(s) can be safely ignored without changing
 *    the message framing or response semantics.
 *
 * 3.2.6.  Field Value Components
 *
 *    Most HTTP header field values are defined using common syntax
 *    components (token, quoted-string, and comment) separated by
 *    whitespace or specific delimiting characters.  Delimiters are chosen
 *    from the set of US-ASCII visual characters not allowed in a token
 *    (DQUOTE and "(),/:;<=>?@[\]{}").
 *
 *      token          = 1*tchar
 *
 *      tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
 *                     / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
 *                     / DIGIT / ALPHA
 *                     ; any VCHAR, except delimiters
 *
 *    A string of text is parsed as a single value if it is quoted using
 *    double-quote marks.
 *
 *      quoted-string  = DQUOTE *( qdtext / quoted-pair ) DQUOTE
 *      qdtext         = HTAB / SP /%x21 / %x23-5B / %x5D-7E / obs-text
 *      obs-text       = %x80-FF
 *
 *    Comments can be included in some HTTP header fields by surrounding
 *    the comment text with parentheses.  Comments are only allowed in
 *    fields containing "comment" as part of their field value definition.
 *
 *      comment        = "(" *( ctext / quoted-pair / comment ) ")"
 *      ctext          = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text
 *
 *    The backslash octet ("\") can be used as a single-octet quoting
 *    mechanism within quoted-string and comment constructs.  Recipients
 *    that process the value of a quoted-string MUST handle a quoted-pair
 *    as if it were replaced by the octet following the backslash.
 *
 *      quoted-pair    = "\" ( HTAB / SP / VCHAR / obs-text )
 *
 *    A sender SHOULD NOT generate a quoted-pair in a quoted-string except
 *    where necessary to quote DQUOTE and backslash octets occurring within
 *    that string.  A sender SHOULD NOT generate a quoted-pair in a comment
 *    except where necessary to quote parentheses ["(" and ")"] and
 *    backslash octets occurring within that comment.
 *
 */

#ifndef PARSE_REQUEST_HEADERS_H
#define PARSE_REQUEST_HEADERS_H

// trying to be small and efficient, when a value exceeds limit reject the whole request with 431
// (Request Header Fields Too Large).
#define HTTP_HEADER_NAME_MAXLEN 64
#define HTTP_HEADER_VALUE_MAXLEN 1024
#define MAX_HTTP_HEADERS 8

struct http_request_header {
    char name[HTTP_HEADER_NAME_MAXLEN];
    char value[HTTP_HEADER_VALUE_MAXLEN];
};

/*
 * parse_request_headers - parse the entire group of headers into individual headers.
 *
 * @src  - null-terminated string containing all the headers.
 *         The string is treated as read-only; we work on a copy internally.
 * @out  - caller-supplied array of structs to fill in.
 *
 * Returns  number of headers parsed on success.
 * TODO
 * Returns -1 if src is NULL, empty, or does not contain [CRITERA FOR RECOGNISING GROUP OF HEADERS].
 *
 * On failure, the contents of *out are undefined.
 */
int parse_request_headers(const char *src, struct http_request_header *out);

/*
 * parse_request_header - parse a single header for an HTTP request.
 *
 * @src  - null-terminated string containing a header.
 *         The string is treated as read-only; we work on a copy internally.
 * @out  - caller-supplied struct to fill in.
 *
 * Returns  0 on success.
 * Returns -1 if src is NULL, empty, or does not contain a name, followed by a colon, followed by
 * more test (the value).
 *
 * On failure, the contents of *out are undefined.
 */
int parse_request_header(const char **p, struct http_request_header *out);

#endif // PARSE_REQUEST_HEADERS_H
