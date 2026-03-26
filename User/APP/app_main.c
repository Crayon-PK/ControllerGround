#include "app_main.h"
#include "task_ui.h"
#include "task_ctrl.h"

// 定义消息队列句柄
osMessageQueueId_t KeyQueue;

// 任务句柄定义
osThreadId_t TaskUI_Handler;
osThreadId_t TaskCtrl_Handler;

// 任务属性定义
const osThreadAttr_t TaskUI_attributes = {
  .name = "Task_UI",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t TaskCtrl_attributes = {
  .name = "Task_Ctrl",
  .stack_size = 128 * 4, 
  .priority = (osPriority_t) osPriorityAboveNormal,
};

void App_TaskCreate(void)
{
    // 1. 创建消息队列，长度为 10，每个消息大小为 1 字节
    KeyQueue = osMessageQueueNew(10, sizeof(uint8_t), NULL);
    if (KeyQueue == NULL) {
        // 队列创建失败处理
        return;
    }

    // 2. 创建 LCD 显示任务
    TaskUI_Handler = osThreadNew((osThreadFunc_t)TaskUI_Entry, NULL, &TaskUI_attributes);

    // 3. 创建按键扫描任务
    TaskCtrl_Handler = osThreadNew((osThreadFunc_t)TaskCtrl_Entry, NULL, &TaskCtrl_attributes);

    osThreadExit();
}
