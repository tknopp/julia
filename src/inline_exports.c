#ifdef _OS_WINDOWS_
#define STDCALL __stdcall
# ifdef IMPORT_EXPORTS
#  define DLLEXPORT __declspec(dllimport)
# else
#  define DLLEXPORT __declspec(dllexport)
# endif
#else
#define STDCALL
#define DLLEXPORT __attribute__ ((visibility("default")))
#endif

#define STATIC_INLINE_OR_EXPORT DLLEXPORT

#include "julia.h"
