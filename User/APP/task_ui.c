#include "task_ui.h"
#include "app_main.h"
#include "bsp_lcd.h"
#include "cmsis_os.h"
#include "lvgl.h"
#include "lv_port_disp.h"

void TaskUI_Entry(void *argument)
{
    // 1. 初始化 LVGL 核心与显示接口
    lv_init();
    lv_port_disp_init();

    // 2. 创建一个简单的 LVGL 标签用于测试
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "LVGL Porting Test OK!");
    lv_obj_center(label); // 将标签居中显示

    while (1)
    {
        // 3. LVGL 任务调度核心，处理绘图和事件
        lv_timer_handler();
        
        // 4. 释放 CPU，延时 5ms (假设 FreeRTOS Tick 为 1ms)
        osDelay(5); 
    }
}
