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
# define _TRAILER_DBG_INFO(trl) D("mag: 0x%08x | myOffset: 0x%08x (+%lu) | rlEntryP: 0x%08x", trl->magicNr, trl->myOffset, trl->mySize, trl->rlEntryPt)
# define _PE_DBG_INFO(pehdr) D("arch: 0x%03x (%s) | sections: %d | sectionOffset: %d | entryPoint: 0x%04x", \
  pehdr->pNtHeader->FileHeader.Machine, \
  (pehdr->pNtHeader->FileHeader.Machine == 0x14c ? "i386" : "non-i386"), \
  pehdr->pNtHeader->FileHeader.NumberOfSections, \
  pehdr->SectionOffset, \
  pehdr->pNtHeader->OptionalHeader.AddressOfEntryPoint)
#else
# define _TRAILER_DBG_INFO(trl)
# define _PE_DBG_INFO(pehdr)
# define D(fmt, ...)
# define D2(str)
# define E(fmt, ...)
# define E2(str)
# define PE(str)
#endif

#endif // LOG_H_INCLUDED
