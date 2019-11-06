#ifndef __ICONV_H__
#define __ICONV_H__

#include <string.h>
#ifdef WIN32
#include <malloc.h>
#endif
#include "YIM.h"

#ifdef WIN32
extern const char* gb23122utf8(const char* gb2312);
#define UTEXT(str) gb23122utf8(str)
extern XString Local_to_Unicode(const char* in, int len);
extern XString UTF8_to_Unicode(const char* in, int len);
extern std::string Unicode_to_UTF8(const wchar_t* in, int len);
  

#else
#define UTEXT(str) str

#endif

#endif
