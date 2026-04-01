#include "bsp_telemetry.h"
#include "usart.h"

/* 定义全局接收缓冲区 */
uint8_t telemetry_rx_buf[TELEMETRY_RX_BUF_SIZE];

void bsp_telemetry_init(void)
{
    /* 开启DMA循环接收 */
    HAL_UART_Receive_DMA(&huart3, telemetry_rx_buf, TELEMETRY_RX_BUF_SIZE);
    
    /* 开启串口空闲中断 */
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
}
