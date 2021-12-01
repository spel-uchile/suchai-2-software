//
// Created by spel on 15-07-21.
//

#include "suchai/taskCommunications.h"
#include "app/system/cmdCDH.h"
#include "app/payloads/stt/repoDataSchemaSTT.h"

static char *tag = "Communications*";

#define SCH_TRX_PORT_CDH (SCH_TRX_PORT_APP+1)  ///< 16 CDH app port
#define SCH_TRX_PORT_STT (SCH_TRX_PORT_APP+2)  ///< 17 STT app port
#define SCH_TRX_PORT_GPS (SCH_TRX_PORT_APP+3)  ///< 18 GPS app port
#define SCH_TRX_PORT_PHY (SCH_TRX_PORT_APP+4)  ///< 19 PHY-LANG app port

int pay_parse_tm(char *data, int n_structs, int payload, data_map_t *pay_data_map);
void com_receive_cmdh_tm(csp_packet_t *packet);
void parse_stt_data(csp_packet_t *packet);

void taskCommunicationsHook(csp_conn_t *conn, csp_packet_t *packet)
{
    switch (csp_conn_dport(conn))
    {
        case SCH_TRX_PORT_CDH:
            // Process TM packet
            com_receive_cmdh_tm(packet);
            break;
        case SCH_TRX_PORT_STT:
            parse_stt_data(packet);
            break;
        default:
            break;
    }

}

/**
 * Save payload telemetry to database. Receive the payload specific data_map struct to parse
 * telemetry definitions.
 * @param data Buffer with one or more payload telemetry structs
 * @param n_structs Number of structs in data buffer
 * @param payload Payload telemetry id
 * @param pay_data_map Payload data_map struct
 * @return CMD_OK or CMD_ERROR
 */
int pay_parse_tm(char *data, int n_structs, int payload, data_map_t *pay_data_map)
{
    int j, offset, errors = 0;

    for(j=0; j < n_structs; j++)
    {
        offset = j * pay_data_map[payload].size; // Select next struct
        int rc = dat_add_payload_sample(data + offset, payload); //Save next struct
        if(rc == -1)
            errors ++;
    }

    return errors > 0 ? CMD_ERROR : CMD_OK;
}

/**
 * Process a TM frame, determine TM type and call corresponding parsing command
 * @param packet a csp buffer containing a com_frame_t structure.
 */
void com_receive_cmdh_tm(csp_packet_t *packet) {
    cmd_t *cmd_parse_tm;
    com_frame_t *frame = (com_frame_t *) packet->data;

    frame->nframe = csp_ntoh16(frame->nframe);
    frame->ndata = csp_ntoh32(frame->ndata);

    LOGI(tag, "Received: %d bytes", packet->length);
    LOGI(tag, "Node    : %d", frame->node);
    LOGI(tag, "Frame   : %d", frame->nframe);
    LOGI(tag, "Type    : %d", frame->type);
    LOGI(tag, "Samples : %d", frame->ndata);

    if(frame->type == TM_TYPE_STRING)
    {
        cmd_parse_tm = cmd_get_str("tm_parse_msg");
        cmd_add_params_raw(cmd_parse_tm, frame, sizeof(com_frame_t));
        cmd_send(cmd_parse_tm);
    }
    if(frame->type == TM_TYPE_PAYLOAD_STA)
    {
        cmd_parse_tm = cmd_get_str("tm_parse_beacon");
        cmd_add_params_raw(cmd_parse_tm, frame, sizeof(com_frame_t));
        cmd_send(cmd_parse_tm);
    }
}

/**
 * Receives STT telemetry data
 * @param packet CSP packet with telemetry frame
 */
void parse_stt_data(csp_packet_t *packet)
{
    com_frame_t *frame = (com_frame_t *)packet->data;
    frame->nframe = csp_ntoh16(frame->nframe);
    frame->ndata = csp_ntoh32(frame->ndata);

    LOGI(tag, "Node    : %d", frame->node);
    LOGI(tag, "Type    : %d", frame->type);
    LOGI(tag, "Frame   : %d", frame->nframe);
    LOGI(tag, "Samples : %d", frame->ndata);

    _ntoh32_buff(frame->data.data32, sizeof(frame->data.data32)/ sizeof(uint32_t));
    pay_parse_tm((char *)(frame->data.data8), frame->ndata, frame->type-TM_TYPE_PAYLOAD, stt_data_map);
}
