//
// Created by spel on 15-07-21.
//

#include "suchai/taskCommunications.h"

static char *tag = "Communications*";

#define SCH_TRX_PORT_STT (SCH_TRX_PORT_APP+1)  ///< 16 STT app port
#define STT_TYPE_GYRO 30 ///< STT Gyro telemetry type

void parse_stt_data(csp_packet_t *packet);
void parse_mag_data(csp_packet_t *packet);

void taskCommunicationsHook(csp_conn_t *conn, csp_packet_t *packet)
{
    switch (csp_conn_dport(conn))
    {
        default:
            break;
    }

}
