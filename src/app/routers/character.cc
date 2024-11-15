#include <string>
#include "../../libraries/httplib.h"
#include "./character.h"

std::string ResourceEndpoint = "_resources";

std::string get_resource_baseurl(httplib::Request *request)
{
  std::string host = request->remote_addr;
  if (request->has_header("Host"))
    host = request->get_header_value("Host");
  return "http://" + host + "/" + ResourceEndpoint;
}

std::vector<Speaker> characters_to_speakers(std::vector<Character> *characters)
{
  std::vector<Speaker> speakers;
  for (auto &&character : *characters)
  {
    Speaker speaker;
    speaker.name = std::string(character.name);
    speaker.speaker_uuid = std::string(character.uuid);
    for (auto &&s : *character.talk_styles)
    {
      SpeakerStyle style;
      style.id = s.id;
      style.name = std::string(s.name);
      style.type = std::string(s.type);
      speaker.styles->emplace_back(style);
    }
    for (auto &&s : *character.sing_styles)
    {
      SpeakerStyle style;
      style.id = s.id;
      style.name = std::string(s.name);
      style.type = std::string(s.type);
      speaker.styles->emplace_back(style);
    }
    speaker.version = std::string(character.version);
    speaker.supported_features->permitted_synthesis_morphing = std::string(character.supported_features->permitted_synthesis_morphing);
  }
  return speakers;
}

void router_character(httplib::Server *server)
{
  server->Get("/speakers", [](const httplib::Request &req, httplib::Response &res) {
    std::string core_version = req.has_param("core_version") ? req.get_param_value("core_version") : "";
  });
}
