typedef unsigned long	size_t;
char *strdup(const char *src);
size_t strlen(const char *str);
int strcmp(const char* s1, const char* s2);
char *substr(const char *input, int offset, int len);
char *append_char_to_string(char *lhs, char rhs);
char *append_string(char *lhs, char *rhs);