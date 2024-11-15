#ifndef VOICEVOX_ENGINE_ROUTER_CHARACTER
#define VOICEVOX_ENGINE_ROUTER_CHARACTER
#include <string>
#include "../../libraries/httplib.h"
#include "../../metas/metas.h"
#include "../../metas/metas_store.h"
std::string get_resource_baseurl(httplib::Request *request);
std::vector<Speaker> characters_to_speakers(std::vector<Character> *characters);
void router_character(httplib::Server *server);
#endif
