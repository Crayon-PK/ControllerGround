#ifndef __DATA_MODEL_H
#define __DATA_MODEL_H

#include <stdint.h>

/* 姿态数据模型 */
typedef struct {
    float roll;
    float pitch;
    float yaw;
} VehicleAttitude_t;

/* 系统状态数据模型 */
typedef struct {
    uint16_t battery_mv; // 电池电压 (毫伏)
} VehicleSysStatus_t;

#endif
