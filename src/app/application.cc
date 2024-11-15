#include "../libraries/httplib.h"
#include "./routers/character.h"
#include "./routers/tts_pipeline.h"
#include "../tts_pipeline/tts_engine.h"

void serve(TTSEngineManager *tts_engines)
{
  httplib::Server server;
  router_character(&server);
  router_tts_pipeline(&server, tts_engines);
  server.listen("0.0.0.0", 8080);
}
