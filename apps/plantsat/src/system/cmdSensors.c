/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2018, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
 *      Copyright 2018, Tomas Opazo Toro, tomas.opazo.t@gmail.com
 *      Copyright 2018, Matias Ramirez Martinez, nicoram.mt@gmail.com
 *      Copyright 2018, Camilo Rojas, camrojas@uchile.cl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "app/system/cmdSensors.h"

static const char* tag = "cmdSens";

void cmd_sensors_init(void)
{
    cmd_add("sen_set_state", sensors_set_state, "%u %u %d", 3);
    cmd_add("sen_set_active", sensors_set_active, "%d %d", 2);
    cmd_add("sen_take_sample", sensors_take_sample, "%u", 1);
    cmd_add("sen_init_dummy", sensors_init, "", 0);
    cmd_add("sen_get_temp", sensors_get_temperatures, "", 0);
    cmd_add("sen_get_adcs", sensors_get_adcs_basic, "", 0);
    cmd_add("sen_get_eps", sensors_get_eps, "", 0);
    cmd_add("sen_get_status", sensors_get_status_basic, "", 0);
}

int sensors_set_state(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    unsigned int action;
    unsigned int step;
    int nsamples;
    if(nparams == sscanf(params, fmt, &action, &step, &nsamples)){
        int rc = dat_set_stmachine_state(action, step, nsamples);
        return rc ? CMD_OK : CMD_ERROR;
    }
    return CMD_SYNTAX_ERROR;
}

int sensors_set_active(char *fmt, char *params, int nparams)
{
    if(params == NULL)
        return CMD_SYNTAX_ERROR;

    int payload;
    int activate;
    if(nparams == sscanf(params, fmt, &payload, &activate))
    {
        if(payload < 0 ) {
            if (activate == 0) {
                osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
                status_machine.active_payloads = 0;
                osSemaphoreGiven(&repo_machine_sem);

            } else  {
                osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
                status_machine.active_payloads = 0;
                int i;
                for (i=0; i < status_machine.total_sensors; i++) {
                    status_machine.active_payloads +=  1 << i ;
                }
                osSemaphoreGiven(&repo_machine_sem);
            }
            return CMD_OK;
        }

        if (payload >= status_machine.total_sensors ) {
            return CMD_SYNTAX_ERROR;
        }

        if( activate == 1  && !dat_stmachine_is_sensor_active(payload,
                                                              status_machine.active_payloads,
                                                              status_machine.total_sensors) ) {
            osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
            status_machine.active_payloads = ((unsigned  int)1 << payload) | status_machine.active_payloads;
            osSemaphoreGiven(&repo_machine_sem);
            return CMD_OK;
        } else if ( activate == 0 && dat_stmachine_is_sensor_active(payload,
                                                                    status_machine.active_payloads,
                                                                    status_machine.total_sensors) ) {
            osSemaphoreTake(&repo_machine_sem, portMAX_DELAY);
            status_machine.active_payloads = ((unsigned  int)1 << payload) ^ status_machine.active_payloads;
            osSemaphoreGiven(&repo_machine_sem);
            return CMD_OK;
        }
    }
    return CMD_SYNTAX_ERROR;
}

int sensors_take_sample(char *fmt, char *params, int nparams)
{
    LOGW(tag, "DEPRECATED!");
    return CMD_ERROR;
}

int sensors_init(char *fmt, char *params, int nparams)
{
    LOGD(tag, "Initializing dummy sensor");
    return CMD_OK;
}

int sensors_get_adcs_basic(char *fmt, char *params, int nparams)
{
    gs_mpu3300_gyro_t gyro_reading;
    gs_hmc5843_data_t hmc_reading;
    int rc1 = gs_mpu3300_read_gyro(&gyro_reading);
    int rc2 = gs_hmc5843_read_single(&hmc_reading);
    int curr_time = dat_get_time();
    if(rc1 != 0 || rc2 != 0)
    {
        LOGE(tag, "Error reading adcs sensors (%d, %d)", rc1, rc2);
        return CMD_ERROR;
    }

    uint16_t sun1, sun2, sun3, sun4 = 0;
    int rc3 = gs_gssb_istage_get_sun_voltage(0x10, 500, &sun1);
    int rc4 = gs_gssb_istage_get_sun_voltage(0x11, 500, &sun2);
    int rc5 = gs_gssb_istage_get_sun_voltage(0x12, 500, &sun3);
    int rc6 = gs_gssb_istage_get_sun_voltage(0x13, 500, &sun4);
    if(rc3 != 0 || rc4 != 0 || rc5 != 0 || rc6 != 0)
    {
        LOGE(tag, "Error reading coarse sun sensors (%d, %d, %d, %d)", rc3, rc4, rc5, rc6);
        return CMD_ERROR;
    }

    /* Save ADCS data */
    int index_ads = dat_get_system_var(data_map[ads_sensors].sys_index);
    ads_data_t data_ads = {index_ads, curr_time,
                           gyro_reading.gyro_x, gyro_reading.gyro_y, gyro_reading.gyro_z,
                           hmc_reading.x, hmc_reading.y, hmc_reading.z,
                           sun1, sun2, sun3, sun4};
    int ret = dat_add_payload_sample(&data_ads, ads_sensors);

    LOGI(tag, "Saving payload %d: ADS (%d). Index: %d, time %d, gyro_x: %.04f, gyro_y: %.04f, gyro_z: %.04f, mag_x: %.04f, mag_y: %.04f, mag_z: %.04f, sun1: %d, sun2, %d, sun3: %d, sun4, %d",
         ads_sensors, ret, index_ads, curr_time, gyro_reading.gyro_x, gyro_reading.gyro_y, gyro_reading.gyro_z,
         hmc_reading.x, hmc_reading.y, hmc_reading.z,
         sun1, sun2, sun3, sun4);

    return ret == -1 ? CMD_ERROR : CMD_OK;
}

int sensors_get_adcs_full(char *fmt, char *params, int nparams)
{
    return CMD_ERROR;
}

int sensors_get_eps(char *fmt, char *params, int nparams)
{
    eps_hk_t hk = {};
    int rc = eps_hk_get(&hk);
    if(rc == 0) return CMD_ERROR;

    int32_t curr_time = dat_get_time();
    uint32_t cursun = hk.cursun;
    uint32_t cursys = hk.cursys;
    uint32_t vbatt = hk.vbatt;
    int32_t teps =  (hk.temp[0]+hk.temp[1]+hk.temp[2]+hk.temp[3])*10/4;
    int32_t tbat = (hk.temp[4]+hk.temp[5])*10/2;

    int index_eps = dat_get_system_var(data_map[eps_sensors].sys_index);
    eps_data_t data_eps = {index_eps, curr_time, cursun, cursys, vbatt, teps, tbat};
    rc = dat_add_payload_sample(&data_eps, eps_sensors);

    LOGI(tag, "Saving payload %d: EPS (%d). Index: %d, time %d, cursun: %d, cursys: %d, vbatt: %d, teps: %d, tbat: %d ",
         eps_sensors, rc, index_eps, curr_time, cursun, cursys, vbatt, teps, tbat);

    return rc == -1 ? CMD_ERROR : CMD_OK;
}

int sensors_get_temperatures(char *fmt, char *params, int nparams)
{
    /** OBC TEMPERATURES */
    int16_t tobc1, tobc2, tobc3 = 0;
    float tgyro = 1;
    int curr_time = dat_get_time();
    int rc = 0;

    /* Read board temperature sensors */
    LOGD(tag, "OBC Temperatures");
    rc += gs_lm71_read_temp(GS_A3200_SPI_SLAVE_LM71_0, 100, &tobc1); //sensor1 = lm70_read_temp(1);
    rc += gs_lm71_read_temp(GS_A3200_SPI_SLAVE_LM71_1, 100, &tobc2); //sensor2 = lm70_read_temp(2);
    rc += gs_mpu3300_read_temp(&tgyro);
    tobc3 = (uint16_t)tgyro;
    if(rc != 0)
    {
        LOGE(tag, "Error reading OBC temperatures");
        return CMD_ERROR;
    }

    /** EPS TEMPERATURES */
    LOGD(tag, "EPS Temperatures");
    eps_hk_t hk = {0};
    rc = eps_hk_get(&hk);
    if(rc == 0)
    {
        LOGE(tag, "Error reading eps temperatures");
        return CMD_ERROR;
    }

    gs_a3200_pwr_switch_enable(GS_A3200_PWR_GSSB);
    gs_a3200_pwr_switch_enable(GS_A3200_PWR_GSSB2);

    /** GSSB TEMPERATURES */
    LOGD(tag, "GSSB Temperatures");
    int16_t gtemp1, gtemp2, gtemp3, gtemp4 = 0;
    rc = gs_gssb_istage_get_internal_temp(0x10, 500, &gtemp1);
    rc += gs_gssb_istage_get_internal_temp(0x11, 500, &gtemp2);
    rc += gs_gssb_istage_get_internal_temp(0x12, 500, &gtemp3);
    rc += gs_gssb_istage_get_internal_temp(0x13, 500, &gtemp4);
    if(rc != 0)
    {
        LOGW(tag, "Error reading GSSB temperatures (%d)", rc);
        //return CMD_ERROR;
    }

    /** SOLAR PANELS TEMPERATURES */
    LOGD(tag, "SP Temperatures");
    float stempf1, stempf2, stempf3, stempf4 = 0;
    rc = gs_gssb_istage_get_temp(0x10, 500, &stempf1);
    rc += gs_gssb_istage_get_temp(0x11, 500, &stempf2);
    rc += gs_gssb_istage_get_temp(0x12, 500, &stempf3);
    rc += gs_gssb_istage_get_temp(0x13, 500, &stempf4);
    if(rc != 0)
    {
        LOGW(tag, "Error reading SOLAR PANELS temperatures (%d)", rc);
        //return CMD_ERROR;
    }
    int16_t stemp1 = (int16_t)stempf1*10;
    int16_t stemp2 = (int16_t)stempf2*10;
    int16_t stemp3 = (int16_t)stempf3*10;
    int16_t stemp4 = (int16_t)stempf4*10;

    /** UPPER INTER STAGE TEMPERATURES */
    LOGD(tag, "IS2 Temperatures");
    int16_t is2_int_temp1, is2_int_temp2, is2_int_temp3, is2_int_temp4, is2_ext_temp1, is2_ext_temp2, is2_ext_temp3, is2_ext_temp4 = 0;
    //TODO: READ UPPER INTER STAGE PANELS TEMPERATURES

    /* Save temperature data */
    LOGD(tag, "Fill Temperatures");
    int index_temp = dat_get_system_var(data_map[temp_sensors].sys_index);
    temp_data_t data_temp = {
            index_temp, curr_time,
            tobc1, tobc3, tobc3,
            hk.temp[0], hk.temp[1], hk.temp[2], hk.temp[3], hk.temp[4], hk.temp[5],
            gtemp1, gtemp2, gtemp3, gtemp4, stemp1, stemp2, stemp3, stemp4,
            is2_int_temp1, is2_int_temp2, is2_int_temp3, is2_int_temp4, is2_ext_temp1, is2_ext_temp2, is2_ext_temp3, is2_ext_temp4
    };
    LOGD(tag, "Save Temperatures");
    rc = dat_add_payload_sample(&data_temp, temp_sensors);

    LOGI(tag, "Saving payload %d: TEMP (%d). Index: %d, time %d, tobc1: %d, teps1: %d, istage1: %d, panel1: %d, istage1: %d, istageext1: %d",
         temp_sensors, rc, index_temp, curr_time, tobc1, hk.temp[0], gtemp1, stemp1, is2_int_temp1, is2_ext_temp1);
    return rc != 0 ? CMD_ERROR : CMD_OK;
}

int sensors_get_status_basic(char *fmt, char *params, int nparams)
{
    status_data_t status;
    status.timestamp = dat_get_time();
    status.index = dat_get_system_var(data_map[temp_sensors].sys_index);
    status.dat_obc_opmode = dat_get_system_var(dat_obc_opmode);
    status.dat_rtc_date_time = dat_get_system_var(dat_rtc_date_time);
    status.dat_obc_last_reset = dat_get_system_var(dat_obc_last_reset);
    status.dat_obc_hrs_alive = dat_get_system_var(dat_obc_hrs_alive);
    status.dat_obc_hrs_wo_reset = dat_get_system_var(dat_obc_hrs_wo_reset);
    status.dat_obc_reset_counter = dat_get_system_var(dat_obc_reset_counter);
    status.dat_obc_executed_cmds = dat_get_system_var(dat_obc_executed_cmds);
    status.dat_obc_failed_cmds = dat_get_system_var(dat_obc_failed_cmds);
    status.dat_com_count_tm = dat_get_system_var(dat_com_count_tm);
    status.dat_com_count_tc = dat_get_system_var(dat_com_count_tc);
    status.dat_com_last_tc = dat_get_system_var(dat_com_last_tc);
    status.dat_fpl_last = dat_get_system_var(dat_fpl_last);
    status.dat_fpl_queue = dat_get_system_var(dat_fpl_queue);
    status.dat_ads_tle_epoch = dat_get_system_var(dat_ads_tle_epoch);
    status.dat_eps_vbatt = dat_get_system_var(dat_eps_vbatt);
    status.dat_eps_cur_sun = dat_get_system_var(dat_eps_cur_sun);
    status.dat_eps_cur_sys = dat_get_system_var(dat_eps_cur_sys);
    status.dat_obc_temp_1 = dat_get_system_var(dat_obc_temp_1);
    status.dat_eps_temp_bat0 = dat_get_system_var(dat_eps_temp_bat0);
    status.dat_drp_mach_action = dat_get_system_var(dat_drp_mach_action);
    status.dat_drp_mach_state = dat_get_system_var(dat_drp_mach_state);
    status.dat_drp_mach_payloads = dat_get_system_var(dat_drp_mach_payloads);
    status.dat_drp_mach_step = dat_get_system_var(dat_drp_mach_step);

    int rc = dat_add_payload_sample(&status, status_sensors);

    LOGI(tag, "Saving payload %d: STATUS (%d). Index: %d, time %d", status_sensors, rc, index, status.timestamp);
    return rc != 0 ? CMD_ERROR : CMD_OK;
}
