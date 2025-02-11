#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
extern void dbg(_Printf_format_string_ const char *fmt, ...);
#else /* __GNUC__ */
extern void dbg(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
#endif

#ifdef __cplusplus
} // extern "C"
#endif
