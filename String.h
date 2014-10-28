#pragma once

#define String_startsWith(s, match) (strstr((s), (match)) == (s))
#define String_contains_i(s1, s2) (strcasestr(s1, s2) != NULL)

char *String_cat(const char *s1, const char *s2);

char *String_trim(const char *in);

extern int String_eq(const char *s1, const char *s2);

char **String_split(const char *s, char sep, int *n);

void String_freeArray(char **s);

char *String_getToken(const char *line, const unsigned short int numMatch);
