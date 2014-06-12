#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifdef SMSHR_DEBUG
# define PKG_NAME "w32smshr"
# ifndef D
#  define LOG_HDR printf( PKG_NAME "(%s.%d): ", __FILE__, __LINE__)
#  define D(fmt, ...) LOG_HDR; printf(fmt, __VA_ARGS__); printf("\r\n");
#  define D2(str) LOG_HDR; printf(str); printf("\r\n");
#  define PE(str) perror(str);
# endif
#else
# define D(fmt, ...)
# define D2(str)
# define PE(str)
#endif

#endif // LOG_H_INCLUDED
