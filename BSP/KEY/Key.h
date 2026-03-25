#ifndef __KEY_H
#define __KEY_H

#include "main.h"
typedef enum {
    KEY_NONE = 0,
    KEY_UP_PRES,   // PA0 (Active High)
    KEY0_PRES,     // PE2 (Active Low)
    KEY1_PRES,     // PE3 (Active Low)
    KEY2_PRES      // PE4 (Active Low)
} KEY_ID_t;

KEY_ID_t KEY_Scan(uint8_t mode);

#endif
