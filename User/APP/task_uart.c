#include "task_uart.h"
#include "usart.h"
#include "cmsis_os.h"
#include <string.h>

/* 确保 extern 声明 */
extern DMA_HandleTypeDef hdma_usart3_rx; 
extern UART_HandleTypeDef huart3;

#define UART_RX_BUF_SIZE 256
uint8_t uart_rx_buf[UART_RX_BUF_SIZE];

void TaskUART_Entry(void *argument)
{
    // 初始化：清理缓冲区
    memset(uart_rx_buf, 0, UART_RX_BUF_SIZE);
    
    // 开启空闲中断
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    // 启动 DMA 接收
    HAL_UART_Receive_DMA(&huart3, uart_rx_buf, UART_RX_BUF_SIZE);

    while (1)
    {
        // 等待信号
        uint32_t flags = osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
        
        if (flags == 0x01)
        {
            // 1. 停止当前 DMA 传输以固定数据
            uint16_t len = UART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
            
            if (len > 0) 
            {
                // 2. 回显测试：原样发回数据
                HAL_UART_Transmit_DMA(&huart3, uart_rx_buf, len);
                
                // 3. 这里可以加一个延时或者等待发送完成，防止重启接收太快冲掉数据
                osDelay(5); 
            }
            
            // 4. 彻底重置 DMA 接收状态
            HAL_UART_DMAStop(&huart3);
            memset(uart_rx_buf, 0, UART_RX_BUF_SIZE);
            HAL_UART_Receive_DMA(&huart3, uart_rx_buf, UART_RX_BUF_SIZE);
        }
    }
}
