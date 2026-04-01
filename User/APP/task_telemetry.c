#include "app_main.h"
#include "bsp_telemetry.h"
#include "telemetry_parse.h"
#include "usart.h"
#include "cmsis_os.h"

extern DMA_HandleTypeDef hdma_usart3_rx;

void TaskTelemetry_Entry(void *argument)
{
    uint32_t last_pos = 0;
    uint32_t curr_pos = 0;
    
    while (1)
    {
        /* 阻塞等待串口空闲中断发来的标志位 0x01 */
        osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);

        /* 计算 DMA 接收位置并处理回卷 */
        curr_pos = TELEMETRY_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);

        if (curr_pos != last_pos) 
        {
            if (curr_pos > last_pos) {
                Telemetry_Parse_Buffer(&telemetry_rx_buf[last_pos], curr_pos - last_pos);
            } else {
                Telemetry_Parse_Buffer(&telemetry_rx_buf[last_pos], TELEMETRY_RX_BUF_SIZE - last_pos);
                Telemetry_Parse_Buffer(&telemetry_rx_buf[0], curr_pos);
            }
            last_pos = curr_pos; 
        }
    }
}
