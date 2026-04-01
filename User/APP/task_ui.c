#include "task_ui.h"
#include "app_main.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "data_broker.h"  /* 引入 Broker */

void TaskUI_Entry(void *argument)
{
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init(); 

    /* 创建显示数据的 Label */
    lv_obj_t * label_att = lv_label_create(lv_scr_act());
    lv_obj_align(label_att, LV_ALIGN_CENTER, 0, -20);
    lv_label_set_text(label_att, "Waiting for Telemetry...");

    lv_obj_t * label_bat = lv_label_create(lv_scr_act());
    lv_obj_align(label_bat, LV_ALIGN_CENTER, 0, 20);
    lv_label_set_text(label_bat, "Battery: -- mV");

    VehicleAttitude_t ui_att;
    VehicleSysStatus_t ui_sys;

    while (1)
    {
        /* 1. 订阅最新数据 */
        Broker_Subscribe_Attitude(&ui_att);
        Broker_Subscribe_Battery(&ui_sys);

        /* 2. 更新 LVGL UI (注意：%f 打印在单片机上通常较消耗资源，且需要开启编译支持，这里转为整数显示进行安全测试) */
        lv_label_set_text_fmt(label_att, "Roll:%d Pitch:%d Yaw:%d", 
                              (int)ui_att.roll, (int)ui_att.pitch, (int)ui_att.yaw);
                              
        lv_label_set_text_fmt(label_bat, "Battery: %d mV", ui_sys.battery_mv);

        /* 3. 运行 LVGL 核心 */
        lv_timer_handler();
        osDelay(5); 
    }
}
