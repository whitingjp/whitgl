void whitgl_logit(const char *file, const int line, const char *str, ...);
#define WHITGL_LOG(...) whitgl_logit(__FILE__, __LINE__, __VA_ARGS__)
