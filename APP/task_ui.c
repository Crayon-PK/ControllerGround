#include "task_ui.h"
#include "app_main.h"
#include "lcd.h"
#include "cmsis_os.h"

void TaskUI_Entry(void *argument)
{
    uint8_t received_key;
    uint32_t press_count = 0; 

    lcd_clear(WHITE);
    lcd_show_string(10, 10, 200, 16, 16, "FreeRTOS RTOS Test", RED);
    lcd_show_string(10, 30, 200, 16, 16, "Key Value: ", BLUE);
    lcd_show_string(10, 50, 200, 16, 16, "Total Press: 0", BLUE);

    while (1)
    {
        if (osMessageQueueGet(KeyQueue, &received_key, NULL, osWaitForever) == osOK)
        {
            press_count++;
            
            lcd_show_num(90, 30, received_key, 1, 16, RED);
            lcd_show_num(100, 50, press_count, 5, 16, RED);
        }
    }
}
