#include "telemetry_parse.h"
#include "common/mavlink.h"
#include "data_broker.h"

void Telemetry_Parse_Buffer(uint8_t *p_data, uint16_t len)
{
    mavlink_message_t msg;
    mavlink_status_t status;

    for (uint16_t i = 0; i < len; i++)
    {
        if (mavlink_parse_char(MAVLINK_COMM_0, p_data[i], &msg, &status))
        {
            switch (msg.msgid)
            {
                case MAVLINK_MSG_ID_ATTITUDE: 
                {
                    mavlink_attitude_t att;
                    mavlink_msg_attitude_decode(&msg, &att);
                    /* 发布姿态数据 (转为角度制更直观) */
                    Broker_Publish_Attitude(att.roll * 57.3f, att.pitch * 57.3f, att.yaw * 57.3f); 
                    break;
                }
                case MAVLINK_MSG_ID_SYS_STATUS: 
                {
                    mavlink_sys_status_t sys_status;
                    mavlink_msg_sys_status_decode(&msg, &sys_status);
                    /* 发布电池数据 */
                    Broker_Publish_Battery(sys_status.voltage_battery);
                    break;
                }
            }
        }
    }
}
