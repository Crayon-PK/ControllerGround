#include "Key.h"

/**
 * @brief  读取按键状态
 * @param  mode: 0-不支持连续按, 1-支持连续按
 * @retval 返回按键 ID
 */
KEY_ID_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_up = 1; // 按键按开标志
    if (mode) key_up = 1;      // 支持连按

    // 读取各引脚状态 (注意 PA0 是高电平有效，PE 是低电平有效)
    uint8_t k_up   = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    uint8_t k0     = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
    uint8_t k1     = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
    uint8_t k2     = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);

    if (key_up && (k_up == 1 || k0 == 0 || k1 == 0 || k2 == 0)) 
    {
        key_up = 0; // 标记按键已按下
        if (k_up == 1) return KEY_UP_PRES;
        if (k0 == 0)   return KEY0_PRES;
        if (k1 == 0)   return KEY1_PRES;
        if (k2 == 0)   return KEY2_PRES;
    } 
    else if (k_up == 0 && k0 == 1 && k1 == 1 && k2 == 1) 
    {
        key_up = 1; // 按键松开
    }
    return KEY_NONE;
}
