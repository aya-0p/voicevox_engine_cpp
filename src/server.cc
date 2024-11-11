#include "./libraries/httplib.h"
#include "./tts_pipeline/kana_converter.h"

int main(void)
{
  init_kana_converter();
  httplib::Server server;
  server.listen("0.0.0.0", 8080);
  return 0;
}
