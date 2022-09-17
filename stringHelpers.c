#include <stdlib.h>
#include <string.h>

// https://stackoverflow.com/questions/37132549/implementation-of-strdup-in-c-programming
char *strdup(const char *src) {
    char *str;
    char *p;
    int len = 0;

    while (src[len])
        len++;
    str = (char* ) malloc(sizeof(char)*(len + 1));
    p = str;
    while (*src)
        *p++ = *src++;
    *p = '\0';
    return str;
}

// http://cvsweb.openbsd.org/cgi-bin/cvsweb/src/lib/libc/string/strlen.c?rev=1.7&content-type=text/x-cvsweb-markup
size_t strlen(const char *str) {
    const char *s;
    for (s = str; *s; ++s);
    return (s - str);
}

// https://stackoverflow.com/questions/34873209/implementation-of-strcmp
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *substr(const char *input, int offset, int len) {
	int input_len = strlen(input);
	if (offset > input_len) {
		return 0;
	}
	char *output = (char*) malloc(sizeof(char) * (len + 1));
	for (int i = 0; i < len; i++) {
		output[i] = input[offset+i];
	}
	output[len] = '\0';
	return output;
}

char *append_char_to_string(char *lhs, char rhs) {
	int len_lhs = strlen(lhs);
	char *retval = (char *) malloc(sizeof(char) * (len_lhs + 2));
	strcpy(retval, lhs);
	retval[len_lhs] = rhs;
	retval[len_lhs+1] = '\0';
	free(lhs);
	return retval;
}

char *append_string(char *lhs, char *rhs) {
	char *retval = (char *) malloc(sizeof(char) * (strlen(lhs) + strlen(rhs) + 2));
	strcpy(retval, lhs);
	strcat(retval, rhs);
    free(lhs);
	return retval;
}