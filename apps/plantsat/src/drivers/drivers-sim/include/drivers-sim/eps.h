//
// Created by carlos on 2/29/24.
//

#ifndef SUCHAI_FLIGHT_SOFTWARE_EPS_H
#define SUCHAI_FLIGHT_SOFTWARE_EPS_H

#include "drivers-sim/interface.h"

#define SIM_EPS_ID 0X02
#define SIM_EPS_ADDR_HKP 0X00
#define SIM_EPS_ADDR_SET 0X01

typedef struct eps_hk_read{
    int vbat;
    int current_in;
    int current_out;
    int temp;
} eps_hk_read_t;

/**
 * Read EPS housekeeping data such as voltage, currents, and temperature.
 * @param hk_read Pointer to store HK data.
 * @return 1 Ok, -1 otherwise
 */
int sim_eps_get_hk(eps_hk_read_t *hk_read);

/**
 *
 * @param channel
 * @param mode
 * @return
 */
int sim_eps_set_output(uint8_t channel, uint8_t mode);

#endif //SUCHAI_FLIGHT_SOFTWARE_EPS_H
