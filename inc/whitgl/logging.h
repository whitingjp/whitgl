#ifndef WHITGL_LOGGING_H_
#define WHITGL_LOGGING_H_

void whitgl_logit(const char *file, const int line, const char *str, ...);
#define WHITGL_LOG(...) whitgl_logit(__FILE__, __LINE__, __VA_ARGS__)

#endif // WHITGL_LOGGING_H_