#include "task_ui.h"
#include "app_main.h"
#include "bsp_lcd.h"
#include "cmsis_os.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h" // 引入输入设备接口

extern volatile uint8_t g_lvgl_init_done;

// 按钮点击事件回调函数
static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text(label, "Clicked!"); // 点击后改变文本
    }
}

void TaskUI_Entry(void *argument)
{
    // 1. 初始化 LVGL 核心与显示接口
    lv_init();
    lv_port_disp_init();
    
    // 2. 初始化触摸输入设备接口
    lv_port_indev_init(); 

	g_lvgl_init_done = 1;

    // 3. 创建一个按钮用于测试触摸
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_center(btn); // 居中显示
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL); // 绑定事件

    // 4. 在按钮上添加文本
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Touch Me");
    lv_obj_center(label);

    while (1)
    {
        lv_timer_handler();
        osDelay(5); 
    }
}
