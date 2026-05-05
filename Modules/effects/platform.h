#ifndef MUTALISK_EFFECTS__PLATFORM_H_
#define MUTALISK_EFFECTS__PLATFORM_H_

#include "cfg.h"
#include <Modules/mutalisk/platform.h>

#if defined WIN32
#  include "dx9Platform.h"
#else
  MUTALISK_NOT_IMPLEMENTED("Platform not supported");
#endif

#endif // MUTALISK_EFFECTS__PLATFORM_H_
