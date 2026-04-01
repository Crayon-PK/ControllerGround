#include "app_main.h"
#include "task_ui.h"
#include "task_ctrl.h"
#include "task_telemetry.h"
#include "data_broker.h"

// 定义消息队列句柄
osMessageQueueId_t KeyQueue;

// 任务句柄定义
osThreadId_t TaskUI_Handler;
osThreadId_t TaskCtrl_Handler;
osThreadId_t TaskTelemetry_Handler; // 新增句柄

// 任务属性定义
const osThreadAttr_t TaskUI_attributes = {
  .name = "Task_UI",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t TaskCtrl_attributes = {
  .name = "Task_Ctrl",
  .stack_size = 128 * 4, 
  .priority = (osPriority_t) osPriorityAboveNormal,
};

const osThreadAttr_t TaskTelemetry_attributes = {
  .name = "Task_Tele",
  .stack_size = 1024 * 4, // MAVLink 解析需要一定栈空间
  .priority = (osPriority_t) osPriorityHigh, // 解析任务优先级要高
};

void App_TaskCreate(void)
{
    // 0. 初始化数据中间件 (必须在任务启动前)
    Broker_Init();

    // 1. 创建消息队列
    KeyQueue = osMessageQueueNew(10, sizeof(uint8_t), NULL);

    // 2. 创建各个任务
    TaskUI_Handler = osThreadNew((osThreadFunc_t)TaskUI_Entry, NULL, &TaskUI_attributes);
    TaskCtrl_Handler = osThreadNew((osThreadFunc_t)TaskCtrl_Entry, NULL, &TaskCtrl_attributes);
    TaskTelemetry_Handler = osThreadNew((osThreadFunc_t)TaskTelemetry_Entry, NULL, &TaskTelemetry_attributes); // 创建数传任务
	
    osThreadExit();
}
