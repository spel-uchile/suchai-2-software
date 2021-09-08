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

    /* Save ADCS data */
    int index_ads = dat_get_system_var(data_map[ads_sensors].sys_index);
    ads_data_t data_ads = {index_ads, curr_time,
                           gyro_reading.gyro_x, gyro_reading.gyro_y, gyro_reading.gyro_z,
                           hmc_reading.x, hmc_reading.y, hmc_reading.z};
    int ret = dat_add_payload_sample(&data_ads, ads_sensors);

    LOGI(tag, "Saving payload %d: ADS (%d). Index: %d, time %d, gyro_x: %.04f, gyro_y: %.04f, gyro_z: %.04f, mag_x: %.04f, mag_y: %.04f, mag_z: %.04f",
         ads_sensors, ret, index_ads, curr_time, gyro_reading.gyro_x, gyro_reading.gyro_y, gyro_reading.gyro_z,
         hmc_reading.x, hmc_reading.y, hmc_reading.z);

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
    int16_t tobc1, tobc2 = 0;
    float tgyro = 1;
    int curr_time = dat_get_time();
    int rc = 0;

    /* Read board temperature sensors */
    rc += gs_lm71_read_temp(GS_A3200_SPI_SLAVE_LM71_0, 100, &tobc1); //sensor1 = lm70_read_temp(1);
    rc += gs_lm71_read_temp(GS_A3200_SPI_SLAVE_LM71_1, 100, &tobc2); //sensor2 = lm70_read_temp(2);
    rc += gs_mpu3300_read_temp(&tgyro);
    if(rc != 0)
        return CMD_ERROR;

    /* Save temperature data */
    int index_temp = dat_get_system_var(data_map[temp_sensors].sys_index);
    struct temp_data data_temp = {index_temp, curr_time, (float)(tobc1 / 10.0), (float)(tobc2 / 10.0), tgyro};
    rc = dat_add_payload_sample(&data_temp, temp_sensors);

    LOGI(tag, "Saving payload %d: TEMP (%d). Index: %d, time %d, tobc1: %d, tobc2: %d, tgyro: %.02f.",
         temp_sensors, rc, index_temp, curr_time, tobc1, tobc2, tgyro)
    return rc != 0 ? CMD_ERROR : CMD_OK;
}

int sensors_get_status_basic(char *fmt, char *params, int nparams)
{
    return CMD_ERROR;
}