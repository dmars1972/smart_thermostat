#include "SmartHVAC.h"

SmartHVAC::SmartHVAC()
{
  hvacMode = NONE;
  hvacState = HVAC_OFF;

  return;
}

void SmartHVAC::setHVACMode(HVACMode m)
{
  if(hvacState == HVAC_ON)
    switchOff();

  hvacMode = m;

  return;
}

void SmartHVAC::switchOn()
{
// Need to turn on pins here.
  hvacState = HVAC_ON;

  return;
}

void SmartHVAC::switchOff()
{
// Need to turn off pins here.

  hvacState = HVAC_OFF;

  return;
}

