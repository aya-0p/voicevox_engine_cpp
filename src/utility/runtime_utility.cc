#include "../defines.h"

bool is_development()
{
#ifdef VOICEVOX_ENGINE_DEVELOPMENT
  return true;
#else
  return false;
#endif
}
