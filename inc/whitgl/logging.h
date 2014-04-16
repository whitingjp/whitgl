void jpw_logit(const char *file, const int line, const char *str, ...);
#define JPW_LOG(...) jpw_logit(__FILE__, __LINE__, __VA_ARGS__)
