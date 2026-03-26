#ifndef __APP_MAIN_H
#define __APP_MAIN_H

#include "cmsis_os.h"

extern osMessageQueueId_t KeyQueue;

// 对外暴露的 App 初始化接口
void App_TaskCreate(void);

#endif
