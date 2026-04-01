#include "bsp.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_telemetry.h"

void bsp_init()
{
	// 初始化屏幕
	lcd_init();
	
	// 初始化触摸
	atk_md0700_touch_init();
	
	// 初始化数传底层接收机制
	bsp_telemetry_init();
}
