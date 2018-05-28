#ifndef _SETTINGS
#define _SETTINGS

class Settings
{
  private:
    unsigned char currentSetTemp;

  public:
    Settings() {};

    unsigned char getCurrentSetTemp() { return currentSetTemp; };
    void setCurrentSetTemp(unsigned char t) { currentSetTemp = t; return; };
};

#endif
