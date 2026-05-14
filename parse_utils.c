/*
 * copy_to_delim_or_eol - copy at most (maxlen - 1) chars from src into dst, stopping at the first
 * `delim`,
 * \r, \n, or \0.  Always null-terminates dst.
 *
 * Returns the number of characters copied (not counting the terminator), or -1 if the token was
 * empty or too long for the buffer.
 */
int copy_to_delim_or_eol(const char *src, char *dst, int maxlen, char delim) {
    int i = 0;

    while (src[i] != delim && src[i] != '\r' && src[i] != '\n' && src[i] != '\0') {
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

/*
 * copy_to_eol - copy at most (maxlen - 1) chars from src into dst, stopping at
 * \r, \n, or \0.  Always null-terminates dst.
 *
 * Returns the number of characters copied (not counting the terminator), or -1 if the token was
 * empty or too long for the buffer.
 */
int copy_to_eol(const char *src, char *dst, int maxlen) {
    int i = 0;

    while (src[i] != '\r' && src[i] != '\n' && src[i] != '\0') {
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

// Move pointer past linear white space
// Linear WhiteSpace is the RFC term for spaces and tabs.
void skip_lws(const char **p) {
    while (**p == ' ' || **p == '\t') {
        (*p)++;
    }
}

// Trim linear white space off the end of a string
void trim_trailing_lws(char *str, int len) {
    for (int i = len - 1; i > -1; i--) {
        if (str[i] == ' ' || str[i] == '\t') {
            str[i] = '\0';
        } else {
            break;
        }
    }
}
