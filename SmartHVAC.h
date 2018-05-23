#ifndef _SMART_HVAC
#define _SMART_HVAC

#include "Arduino.h"
#include "types.h"

class SmartHVAC {
  private:
    HVACMode hvacMode;
    HVACState hvacState;
  public:
    SmartHVAC();

    HVACMode getHVACMode() { return hvacMode; };
    HVACState getHVACState() { return hvacState; };

    void switchOn();
    void switchOff();

    void setHVACMode(HVACMode);
};

#endif
