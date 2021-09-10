/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@uchile.cl
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

#include "suchai/config.h"
#include "app/system/config.h"
#include "suchai/mainFS.h"
#include "suchai/taskInit.h"
#include "suchai/osThread.h"
#include "suchai/log_utils.h"
#include "app/system/cmdADCS.h"
#include "app/system/cmdAX100.h"
#include "app/system/cmdEPS.h"
#include "app/system/cmdGSSB.h"
#include "app/system/cmdRW.h"
#include "app/system/cmdSensors.h"
#include "app/system/taskHousekeeping.h"
#include "app/system/taskADCS.h"
#include "app/system/taskSensors.h"

static char *tag = "app_main";

int init_setup_libcsp_2(void)
{
    /* Init communications */
    LOGI(tag, "Add CSP interfaces and routes...");
    /**
     * Set interfaces and routes
     *  Platform dependent
     */
    /* Init csp i2c interface with address 1 and 400 kHz clock */
    LOGI(tag, "csp_i2c_init...");
    sch_a3200_init_twi0(GS_AVR_I2C_MULTIMASTER, SCH_COMM_NODE, 400000);
    int t_ok = csp_i2c_init(SCH_COMM_NODE, 0, 400000);
    if(t_ok != CSP_ERR_NONE) LOGE(tag, "\tcsp_i2c_init failed!");

    /**
     * Setting route table
     * Build with options: --enable-if-i2c --with-rtable cidr
     *  csp_rtable_load("8/2 I2C 5");
     *  csp_rtable_load("0/0 I2C");
     */
    csp_rtable_set(8, 2, &csp_if_i2c, SCH_TRX_ADDRESS); // Traffic to GND (8-15) via I2C node TRX
    csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_i2c, CSP_NODE_MAC); // All traffic to I2C using node as i2c address

    LOGI(tag, "Route table");
    csp_route_print_table();
    LOGI(tag, "Interfaces");
    csp_route_print_interfaces();

    return 0;
}

int init_setup_trx(void) {
    LOGD(tag, "\t Init TRX...");
    cmd_t *trx_cmd;
    // Set TX_INHIBIT to implement silent time
    trx_cmd = cmd_get_str("com_set_config");
    cmd_add_params_var(trx_cmd, 0, "tx_inhibit", TOSTRING(SCH_TX_INHIBIT));
    cmd_send(trx_cmd);
    if(log_lvl >= LOG_LVL_DEBUG)
    {
        trx_cmd = cmd_build_from_str("com_get_config 0 tx_inhibit");
        cmd_send(trx_cmd);
    }
    trx_cmd = cmd_build_from_str("com_set_config 0 bcn_holdoff 60)");
    cmd_send(trx_cmd);
    if(log_lvl >= LOG_LVL_DEBUG)
    {
        trx_cmd = cmd_build_from_str("com_get_config 0 bcn_holdoff");
        cmd_send(trx_cmd);
    }
    // Set TX_PWR
    trx_cmd = cmd_get_str("com_set_config");
    cmd_add_params_var(trx_cmd, 0, "tx_pwr", dat_get_status_var(dat_com_tx_pwr).i);
    cmd_send(trx_cmd);
    if(log_lvl >= LOG_LVL_DEBUG)
    {
        trx_cmd = cmd_build_from_str("com_get_config 0 tx_pwr");
        cmd_send(trx_cmd);
    }
//    // Set TX_BEACON_PERIOD
//    trx_cmd = cmd_get_str("com_set_config");
//    cmd_add_params_var(trx_cmd, 0, "bcn_interval", dat_get_status_var(dat_com_bcn_period).i);
//    cmd_send(trx_cmd);
//    if(log_lvl >= LOG_LVL_DEBUG)
//    {
//        trx_cmd = cmd_build_from_str("com_get_config 0 bcn_interval");
//        cmd_send(trx_cmd);
//    }
//    // Set TRX Freq
//    trx_cmd = cmd_get_str("com_set_config");
//    cmd_add_params_var(trx_cmd, 1, "freq", dat_get_status_var(dat_com_freq).i);
//    cmd_send(trx_cmd);
//    trx_cmd = cmd_get_str("com_set_config");
//    cmd_add_params_var(trx_cmd, 5, "freq", dat_get_status_var(dat_com_freq).i);
//    cmd_send(trx_cmd);
//    if(log_lvl >= LOG_LVL_DEBUG)
//    {
//        trx_cmd = cmd_build_from_str("com_get_config 1 freq");
//        cmd_send(trx_cmd);
//        trx_cmd = cmd_build_from_str("com_get_config 5 freq");
//        cmd_send(trx_cmd);
//    }
//    // Set TRX Baud
//    trx_cmd = cmd_get_str("com_set_config");
//    cmd_add_params_var(trx_cmd, 1, "baud", dat_get_status_var(dat_com_baud).i);
//    cmd_send(trx_cmd);
//    trx_cmd = cmd_get_str("com_set_config");
//    cmd_add_params_var(trx_cmd, 5, "baud", dat_get_status_var(dat_com_baud).i);
//    cmd_send(trx_cmd);
//    if(log_lvl >= LOG_LVL_DEBUG)
//    {
//        trx_cmd = cmd_build_from_str("com_get_config 1 baud");
//        cmd_send(trx_cmd);
//        trx_cmd = cmd_build_from_str("com_get_config 5 baud");
//        cmd_send(trx_cmd);
//    }
}

int init_antenna_deploy(void)
{
    LOGD(tag, "\tAntenna deployment...")
    //Turn on gssb and update antenna deployment status
    cmd_t *cmd_dep;
    cmd_dep = cmd_get_str("gssb_pwr");
    cmd_add_params_str(cmd_dep, "1 1");
    cmd_send(cmd_dep);

    cmd_dep = cmd_get_str("gssb_update_status");
    cmd_send(cmd_dep);

    //Try to deploy antennas if necessary
    //      istage 1. On: 2s, off: 1s, rep: 5
    cmd_dep = cmd_get_str("gssb_antenna_release");
    cmd_add_params_var(cmd_dep, 16, 2, 1, 5);
    cmd_send(cmd_dep);
    //      istage 2. On: 2s, off: 1s, rep: 5
    cmd_dep = cmd_get_str("gssb_antenna_release");
    cmd_add_params_var(cmd_dep, 17, 2, 1, 5);
    cmd_send(cmd_dep);
    //      istage 3. On: 2s, off: 1s, rep: 5
    cmd_dep = cmd_get_str("gssb_antenna_release");
    cmd_add_params_var(cmd_dep, 18, 2, 1, 5);
    cmd_send(cmd_dep);
    //      istage 4. On: 2s, off: 1s, rep: 5
    cmd_dep = cmd_get_str("gssb_antenna_release");
    cmd_add_params_var(cmd_dep, 19, 2, 1, 5);
    cmd_send(cmd_dep);

    return 0;
}

int init_deployment_routine(void)
{
    LOGI(tag, "DEPLOYMENT...");
    int deployed = dat_get_system_var(dat_dep_deployed);
    LOGI(tag, "dat_dep_deployed: %d...", deployed);
    if(deployed == 0) // First deploy
    {
        LOGI(tag, "FIRST DEPLOY");
        /* First deploy - 30min TRX Silence */
        LOGI(tag, "Setting TRX Inhibit to: %d seconds...", 1860);
        cmd_t *tx_silence = cmd_build_from_str("com_set_config tx_inhibit 1860");
        cmd_send(tx_silence);

        /* Wait 30 minutes before antenna deployment */
        int seconds = 0;
        portTick xLastWakeTime = osTaskGetTickCount();
        while(seconds < 1800)
        {
            LOGI(tag, "Deployment delay: %d/%d seconds...", seconds, 1800);
            osTaskDelayUntil(&xLastWakeTime, 1000); //Suspend task
            seconds ++;
            //TODO CANCEL
        }
        dat_set_system_var(dat_dep_deployed, 1);
    }

    deployed = dat_get_system_var(dat_dep_deployed);
    LOGI(tag, "dat_dep_deployed: %d...", deployed);
    if(deployed == 1) // Deployed not confirmed, but silence time
    {
        LOGI(tag, "ANTENNA DEPLOYMENT");
        cmd_t *eps_update_status_cmd = cmd_get_str("eps_update_status");
        cmd_send(eps_update_status_cmd);
        osDelay(500);
        int vbat_mV = dat_get_system_var(dat_eps_vbatt);

        // Deploy antenna
        LOGI(tag, "Deploying antennas: %d (Battery Voltage: %.04f)", vbat_mV>7000, vbat_mV/1000.0);
        if(vbat_mV > 7000)
            init_antenna_deploy();

        //Update antenna deployment status
        cmd_t *cmd_dep = cmd_get_str("gssb_update_status");
        cmd_send(cmd_dep);
    }

    LOGI(tag, "Restore TRX Inhibit to: %d seconds", 0);
    cmd_t *tx_silence = cmd_build_from_str("com_get_config tx_inhibit 0");
    cmd_send(tx_silence);
}

/**
 * App specific initialization routines
 * This function is called by taskInit
 *
 * @param params taskInit params
 */
void initAppHook(void *params)
{
    int rc;

    /** Finish CSP setup */
    init_setup_libcsp_2();

    /** Init TRX */
    LOGI(tag, "SETUP TRX...");
    rc = init_setup_trx();

    /** Include app commands */
    cmd_adcs_init();
    cmd_ax100_init();
    cmd_eps_init();
    cmd_gssb_init();
    cmd_rw_init();
    cmd_sensors_init();

    /** Init app tasks */
    int t_ok;
#if SCH_HK_ENABLED
    t_ok = osCreateTask(taskHousekeeping, "housekeeping", 2*SCH_TASK_DEF_STACK, NULL, 2, NULL);
    if(t_ok != 0) LOGE(tag, "Task housekeeping not created!");
#endif
#if SCH_SEN_ENABLED
    t_ok = osCreateTask(taskSensors,"sensors", 2*SCH_TASK_DEF_STACK, NULL, 2, NULL);
    if(t_ok != 0) LOGE(tag, "Task sensors not created!");
#endif
#if SCH_ADCS_ENABLED
    t_ok = osCreateTask(taskADCS,"adcs", 2*SCH_TASK_DEF_STACK, NULL, 2, NULL);
    if(t_ok != 0) LOGE(tag, "Task sensors not created!");
#endif

    /** DEPLOYMENT */
    //init_deployment_routine();
}

int main(void)
{
    /** Call framework main, shouldn't return */
    suchai_main();
}

void on_close(int signal)
{
    dat_repo_close();
    cmd_repo_close();

    LOGI(tag, "Exit system!");
    exit(signal);
}