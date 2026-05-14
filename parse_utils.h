#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H
int copy_to_delim_or_eol(const char *src, char *dst, int maxlen, char delim);
int copy_to_eol(const char *src, char *dst, int maxlen);
int skip_lws(const char **p);
void trim_trailing_lws(char *str, int len);
#endif // PARSE_UTILS_H
