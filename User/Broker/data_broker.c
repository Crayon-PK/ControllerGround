#include "data_broker.h"
#include "cmsis_os.h"

static VehicleAttitude_t current_attitude = {0};
static VehicleSysStatus_t current_sys_status = {0};

static osMutexId_t broker_mutex = NULL;

void Broker_Init(void)
{
    broker_mutex = osMutexNew(NULL);
}

void Broker_Publish_Attitude(float roll, float pitch, float yaw)
{
    if (osMutexAcquire(broker_mutex, osWaitForever) == osOK) {
        current_attitude.roll = roll;
        current_attitude.pitch = pitch;
        current_attitude.yaw = yaw;
        osMutexRelease(broker_mutex);
    }
}

void Broker_Subscribe_Attitude(VehicleAttitude_t *out_att)
{
    if (osMutexAcquire(broker_mutex, osWaitForever) == osOK) {
        *out_att = current_attitude;
        osMutexRelease(broker_mutex);
    }
}

void Broker_Publish_Battery(uint16_t voltage_mv)
{
    if (osMutexAcquire(broker_mutex, osWaitForever) == osOK) {
        current_sys_status.battery_mv = voltage_mv;
        osMutexRelease(broker_mutex);
    }
}

void Broker_Subscribe_Battery(VehicleSysStatus_t *out_sys)
{
    if (osMutexAcquire(broker_mutex, osWaitForever) == osOK) {
        *out_sys = current_sys_status;
        osMutexRelease(broker_mutex);
    }
}
