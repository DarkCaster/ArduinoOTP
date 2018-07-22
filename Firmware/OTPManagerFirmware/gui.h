#ifndef GUI_H
#define GUI_H

#ifdef STANDALONE_APP
#include "arduino-defines.h"
#else
#include <Arduino.h>
#endif

class GuiU8G2
{
  public:
    void Init(uint8_t addr=0x78);
};

#endif
