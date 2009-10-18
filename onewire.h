#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include "inttypes.h"

typedef struct
{
    uint8_t devid;
    uint8_t sn0;
    uint8_t sn1;
    uint8_t sn2;
    uint8_t sn3;
    uint8_t sn4;
    uint8_t sn5;
    uint8_t crc;
} onewireId;

void onewireInit(void);

char onewireGetID(onewireId* id);

#endif /* _ONEWIRE_H_ */

