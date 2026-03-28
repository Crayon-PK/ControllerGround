#include "bsp.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"

void bsp_init()
{
	lcd_init();
	atk_md0700_touch_init();
}
