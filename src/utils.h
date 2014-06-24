#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#ifdef _SMSHR_DEBUG
void printHex(const void *lpvbits, const unsigned int n);
#else
# define printHex(buf, siz);
#endif

#endif // UTILS_H_INCLUDED
