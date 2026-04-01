#ifndef __BSP_TELEMETRY_H
#define __BSP_TELEMETRY_H

#include <stdint.h>

#define TELEMETRY_RX_BUF_SIZE 256

/* 暴露接收缓冲区供应用层读取 */
extern uint8_t telemetry_rx_buf[TELEMETRY_RX_BUF_SIZE];

void bsp_telemetry_init(void);

#endif
