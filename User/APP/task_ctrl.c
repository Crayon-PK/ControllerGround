#include "task_ctrl.h"
#include "app_main.h"
#include "bsp_key.h"      
#include "cmsis_os.h"

void TaskCtrl_Entry(void *argument)
{
    KEY_ID_t key_val;

    while (1)
    {
        key_val = KEY_Scan(1); 

        if (key_val != KEY_NONE) 
        {
            osMessageQueuePut(KeyQueue, &key_val, 0, 0); 
        }
        osDelay(20); 
    }
}
