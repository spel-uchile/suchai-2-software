//
// Created by javier on 09-07-20.
//

#ifndef T_ADCS_H
#define T_ADCS_H

#include <stdlib.h>
#include <stdint.h>

#include "suchai/config.h"
#include "suchai/globals.h"

#include "suchai/osQueue.h"
#include "suchai/osDelay.h"

#include "suchai/math_utils.h"
#include "suchai/log_utils.h"

#include "suchai/repoCommand.h"
#include "igrf/igrf13.h"
#include "SGP4.h"

void taskADCS(void *param);

void eskf_predict_state(double* P, double dt);

void calc_sun_pos_i(double jd, vector3_t * sun_dir);

double unixt_to_jd(uint32_t unix_time);

void calc_magnetic_model(double decyear, double latrad, double lonrad, double altm, double* mag);

double jd_to_dec(double jd);

#endif //T_ADCS_H
