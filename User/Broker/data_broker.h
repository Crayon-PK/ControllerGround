#ifndef __DATA_BROKER_H
#define __DATA_BROKER_H

#include "data_model.h"

void Broker_Init(void);
void Broker_Publish_Attitude(float roll, float pitch, float yaw);
void Broker_Subscribe_Attitude(VehicleAttitude_t *out_att);
void Broker_Publish_Battery(uint16_t voltage_mv);
void Broker_Subscribe_Battery(VehicleSysStatus_t *out_sys);

#endif
