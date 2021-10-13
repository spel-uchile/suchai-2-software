/*                                 SUCHAI
 *                      NANOSATELLITE FLIGHT SOFTWARE
 *
 *      Copyright 2021, Carlos Gonzalez Cortes, carlgonz@ug.uchile.cl
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

#include "app/system/cmdCDH.h"

static const char* tag = "cmd_cdh";

void cmd_cdh_init(void)
{
    /** OBC COMMANDS **/
    cmd_add("obc_set_mode", obc_set_mode, "%s", 1);
    cmd_add("obc_cancel_deploy", obc_cancel_deploy, "", 0);
}

int obc_set_mode(char *fmt, char *params, int nparams)
{
    int opmode;
    if(params == NULL || sscanf(params, fmt, &opmode) != nparams) {
        return CMD_SYNTAX_ERROR;
    }

    if(strncmp(params, "normal", 6) == 0)
        opmode = DAT_OBC_OPMODE_NORMAL;
    else if(strncmp(params, "deploy", 6) == 0)
        opmode = DAT_OBC_OPMODE_DEPLOYING;
    else if(strncmp(params, "safe", 4) == 0)
        opmode = DAT_OBC_OPMODE_WARN;
    else if(strncmp(params, "point", 5) == 0)
        opmode = DAT_OBC_OPMODE_REF_POINT;
    else if(strncmp(params, "nadir", 5) == 0)
        opmode = DAT_OBC_OPMODE_NAD_POINT;
    else if(strncmp(params, "detumb", 6) == 0)
        opmode = DAT_OBC_OPMODE_DETUMB_MAG;
    else
    {
        LOGE(tag, "Invalid mode %d. Select from normal, deploy, safe, point, nadir, detumb", params)
        return CMD_SYNTAX_ERROR;
    }

    LOGR(tag, "Opmode %s (%d) selected!", params, opmode);
    dat_set_system_var(dat_obc_opmode, opmode);

    return CMD_OK;
}

int obc_cancel_deploy(char *fmt, char *params, int nparams)
{
    int current_opmode = dat_get_system_var(dat_obc_opmode);
    if( current_opmode == DAT_OBC_OPMODE_DEPLOYING)
    {
        LOGR(tag, "Set opmode from deploying (%d) to normal (%d)", DAT_OBC_OPMODE_DEPLOYING, DAT_OBC_OPMODE_NORMAL);
        dat_set_system_var(dat_obc_opmode, DAT_OBC_OPMODE_NORMAL);
        return CMD_OK;
    }
    else
    {
        LOGW(tag, "Opmode not changed because was not deploying (%d)", current_opmode)
        return CMD_ERROR;
    }
}