//
// Created by spel on 15-07-21.
//

#include "suchai/taskCommunications.h"
#include "app/system/cmdCDH.h"

static char *tag = "Communications*";

#define SCH_TRX_PORT_CDH (SCH_TRX_PORT_APP+0)
#define SCH_TRX_PORT_STT (SCH_TRX_PORT_APP+1)  ///< 16 STT app port
#define STT_TYPE_GYRO 30 ///< STT Gyro telemetry type
#define SCH_TRX_PORT_MAG (SCH_TRX_PORT_APP+2)
#define MAG_TYPE_LIST_FILE 25

void parse_stt_data(csp_packet_t *packet);
void parse_mag_data(csp_packet_t *packet);
void com_receive_cmdh_tm(csp_packet_t *packet);

void taskCommunicationsHook(csp_conn_t *conn, csp_packet_t *packet)
{
    switch (csp_conn_dport(conn))
    {
        case SCH_TRX_PORT_CDH:
            // Process TM packet
            com_receive_cmdh_tm(packet);
            csp_buffer_free(packet);
        case SCH_TRX_PORT_STT:
            parse_stt_data(packet);
            break;
        case SCH_TRX_PORT_MAG:
            parse_mag_data(packet);
            break;
        default:
            break;
    }

}

void parse_stt_data(csp_packet_t *packet)
{
    com_frame_t *frame = (com_frame_t *)packet->data;
    frame->nframe = csp_ntoh16(frame->nframe);
    frame->ndata = csp_ntoh32(frame->ndata);
    int tm_type = frame->type;

    LOGI(tag, "Node    : %d", frame->node);
    LOGI(tag, "Type    : %d", frame->type);
    LOGI(tag, "Frame   : %d", frame->nframe);
    LOGI(tag, "Samples : %d", frame->ndata);

    if(tm_type == STT_TYPE_GYRO)
    {
        for(int i=0; i<frame->ndata; i++)
        {
            stt_gyro_data_t gyro_data;
            memcpy(&gyro_data, frame->data.data8, sizeof(gyro_data));
            LOGR(tag, "Gyro x: %f, y: %f, z: %f", gyro_data.gx, gyro_data.gy, gyro_data.gz)
        }
    }
}

void parse_mag_data(csp_packet_t *packet)
{
    com_frame_t *frame = (com_frame_t *)packet->data;
    frame->nframe = csp_ntoh16(frame->nframe);
    frame->ndata = csp_ntoh32(frame->ndata);
    int tm_type = frame->type;

    LOGI(tag, "Node    : %d", frame->node);
    LOGI(tag, "Type    : %d", frame->type);
    LOGI(tag, "Frame   : %d", frame->nframe);
    LOGI(tag, "Samples : %d", frame->ndata);

    if(tm_type == MAG_TYPE_LIST_FILE)
    {
        cmd_t *cmd = cmd_get_str("mag_parse_names");
        cmd_add_params_raw(cmd, frame, sizeof(com_frame_t));
        cmd_send(cmd);
    }
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
}