#include <vdr/plugin.h>
#include "vars.h"

const char *config  = NULL;
const char *langdir = NULL;

const std::string  MusicConfig_Path(void) {
  std::string musicconfig = cPlugin::ConfigDirectory();
  musicconfig = musicconfig + "/moronsuite/music";
  return musicconfig;
}
