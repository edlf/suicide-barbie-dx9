
#ifndef NEWAGE_BASE_COMMON_CONFIG_H
#define NEWAGE_BASE_COMMON_CONFIG_H

#if defined(WIN32)
#	include <windows.h>
#endif

#if defined(WIN32)
// Disable "vsprintf() is deprecated, use vsprintf_s() instead" family of warnings
#	pragma warning (disable : 4996)
#endif

#endif // NEWAGE_BASE_COMMON_CONFIG_H
