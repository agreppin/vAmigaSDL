#include "debug.h"

#include <stdarg.h>

#if defined(_MSC_VER) && !defined(NDEBUG)
#include <windows.h>

static ULONG(__stdcall *_vDbgPrintEx)(ULONG ComponentId, ULONG Level, PCCH Format, va_list ap) = NULL;

static BOOL dbg_init() {
  static BOOL init = 0;
  if (!init) {
    HMODULE hm = GetModuleHandle(TEXT("ntdll.dll"));
    if (hm)
      *(FARPROC *)&_vDbgPrintEx = GetProcAddress(hm, "vDbgPrintEx");
    init = TRUE;
  }
  return (_vDbgPrintEx != NULL);
}

/* also print on Visual Studio Output, like OutputDebugString */
void dbg(_Printf_format_string_ const char *fmt, ...) {
  va_list ap, ap2;
  va_start(ap, fmt);
  va_copy(ap2, ap);
  vfprintf(stderr, fmt, ap);
  if (dbg_init())
    _vDbgPrintEx(99, 0, fmt, ap2);
  va_end(ap2);
  va_end(ap);
}
#else
void dbg(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}
#endif
