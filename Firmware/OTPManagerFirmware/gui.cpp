#include <U8g2lib.h>
#include <U8x8lib.h>
#include "gui.h"

static U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void GuiU8G2::Init(uint8_t addr)
{
  u8g2.setI2CAddress(addr);
  u8g2.begin();
  //remove remaining code
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_crox2cb_tf);
    u8g2.drawStr(0,24,"Hello World!");
  } while ( u8g2.nextPage() );
}
