/**
 * @file  cmdCDH.h
 * @author Carlos Gonzalez - carlgonz@uchile.cl
 * @date 2021
 * @copyright GNU GPL v3
 *
 * This header have definitions of commands related to C&DH system
 */

#ifndef _CMDCDH_H
#define _CMDCDH_H

#define TM_TYPE_STRING 104

#define SCH_TRX_PORT_CDH (SCH_TRX_PORT_APP+0)

#include "app/system/config.h"
#include "config.h"
#include "suchai/globals.h"
#include "suchai/log_utils.h"

#include "suchai/repoCommand.h"

/**
 * Register command and data handling (C&DH) commands
 */
void cmd_cdh_init(void);

/**
 * Set operation mode status variable
 * @param fmt "%s"
 * @param params <opmode={"normal", "deploy", "safe"}>
 * @param nparams 1
 * @return CMD_OK if executed correctly
 */
int obc_set_mode(char *fmt, char *params, int nparams);

/**
 * Cancel deployment by changing dat_obc_opmode (operation mode) from DEPLOYIG to NORMAL.
 * If current opmode is not DEPLOYING the value was not changed.
 * @param fmt ""
 * @param params ""
 * @param nparams 0
 * @return CMD_OK, CMD_ERROR
 */
int obc_cancel_deploy(char *fmt, char *params, int nparams);

int obc_read_status_basic(status_data_t *status_data);

#endif //_CMDCDH_H
