#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


#ifdef _SMSHR_DEBUG
# define PKG_NAME "w32smshr"
# ifndef D
#  define LOG_DBG_HDR printf("[DBG]:" PKG_NAME "(%s.%d): ", __FILE__, __LINE__)
#  define LOG_ERR_HDR fprintf(stderr, "[ERR]:" PKG_NAME "(%s.%d): ", __FILE__, __LINE__)
#  define D(fmt, ...) LOG_DBG_HDR; printf(fmt, __VA_ARGS__); printf("\r\n");
#  define D2(str) LOG_DBG_HDR; printf(str); printf("\r\n");
#  define E(fmt, ...) LOG_ERR_HDR; fprintf(stderr, fmt, __VA_ARGS__); fprintf(stderr, "\r\n");
#  define E2(str) LOG_ERR_HDR; fprintf(stderr, str); fprintf(stderr, "\r\n");
#  define PE(str) perror(str);
# endif
#else
# define D(fmt, ...)
# define D2(str)
# define E(fmt, ...)
# define E2(str)
# define PE(str)
#endif

#endif // LOG_H_INCLUDED
