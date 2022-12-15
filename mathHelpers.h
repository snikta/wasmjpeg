//int isdigit(char c);
int atoi(const char *c);
int abs(int num);
double fabs (double num);
int min(int x, int y);
int max(int x, int y);
int clamp(int value, int minval, int maxval);
double fclamp(
    double value,
    double minval,
    double maxval
);
double fmin(double x, double y);
double fmax(double x, double y);
char* itoa__(int val, int base);