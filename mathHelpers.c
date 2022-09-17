#include <ctype.h>
#include "./stringHelpers.h"

/*int isdigit(char c) {
    return c >= '0' && c <= '9';
}*/
int atoi(const char *c)
{
    int value = 0;
    int sign = 1;
    if( *c == '+' || *c == '-' )
    {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c))
    {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}
int abs(int num) {
	return num < 0 ? -num : num;
}
double fabs (double num) {
	return num < 0.0 ? -num : num;
}
int min(int x, int y) {
	return x < y ? x : y;
}
int max(int x, int y) {
	return x > y ? x : y;
}
int clamp(int value, int minval, int maxval) {
	return min(max(value, minval), maxval);
}
double fmin(double x, double y) {
	return x < y ? x : y;
}
double fmax(double x, double y) {
	return x > y ? x : y;
}
char* itoa__(int val, int base){ // h/t https://stackoverflow.com/a/3987783
	if (val == -1) {
		return strdup("-1");
	}
	if (val == 0) {
		return strdup("0");
	}
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}