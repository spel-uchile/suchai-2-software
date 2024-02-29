//
// Created by carlos on 2/29/24.
//

#include "drivers-sim/cdh.h"

int get_obc_tem()
{
    uint8_t send[1] = {0x01};
    int recv = 0;
    iface_transaction(send, 1, (uint8_t*)&recv, sizeof(recv));
    return recv;
}