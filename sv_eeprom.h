#include <EEPROM.h>
#include "types.h"

configStruct getConfiguration() {
  configStruct conf;
  EEPROM.begin(sizeof(struct configStruct));
  EEPROM.get(0, conf);
  EEPROM.end();
  return conf;
}

