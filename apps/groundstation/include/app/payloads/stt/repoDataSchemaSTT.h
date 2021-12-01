/**
 * @file  dataSchema.h
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header contains structs symbolizing the schema of data.
 */


#ifndef REPO_DATA_SCHEMA_STT_H
#define REPO_DATA_SCHEMA_STT_H

#include "suchai/log_utils.h"
#include "suchai/storage.h"

/**
 * Enum constants for dynamically identifying system status fields at execution time.
 *
 * Also permits adding new status variables cheaply, by generalizing both the
 * dat_set_system_var and dat_get_system_var functions.
 *
 * The dat_status_last_address constant serves only for comparison when looping through all
 * system status values. For example:
 *
 * @code
 * for (dat_status_address_t i = 0; i < dat_status_last_address; i++)
 * {
 * // some code using i.
 * }
 * @endcode
 *
 * @see dat_status_t
 * @seealso dat_set_system_var
 * @seealso dat_get_system_var
 */
typedef enum stt_dat_status_address_enum {
    /// OBC: On board computer related variables.
    stt_dat_obc_opmode = 0,           ///< General operation mode
    stt_dat_obc_last_reset,           ///< Last reset source
    stt_dat_obc_hrs_alive,            ///< Hours since first boot
    stt_dat_obc_hrs_wo_reset,         ///< Hours since last reset
    stt_dat_obc_reset_counter,        ///< Number of reset since first boot
    stt_dat_obc_sw_wdt,               ///< Software watchdog timer counter
    stt_dat_obc_temp_1,               ///< Temperature value of the first sensor
    stt_dat_obc_executed_cmds,        ///< Total number of executed commands
    stt_dat_obc_failed_cmds,          ///< Total number of failed commands

    /// RTC: Rtc related variables
    stt_dat_rtc_date_time,            ///< RTC current unix time

    /// COM: Communications system variables.
    stt_dat_com_count_tm,             ///< Number of Telemetries sent
    stt_dat_com_count_tc,             ///< Number of received Telecommands
    stt_dat_com_last_tc,              ///< Unix time of the last received Telecommand

    /// FPL: Flight plan related variables
    stt_dat_fpl_last,                 ///< Last executed flight plan (unix time)
    stt_dat_fpl_queue,                ///< Flight plan queue length

    /// Memory: Current payload memory addresses
    stt_dat_drp_idx_temp,                 ///< Temperature data index
    stt_dat_drp_stt,                      /// TODO: cambiar por stt_dat_drp_idx_stt y asiii...
    stt_dat_drp_stt_exp_time,
    stt_dat_drp_stt_gyro,

    /// Memory: Current send acknowledge data
    stt_dat_drp_ack_temp,             ///< Temperature data acknowledge
    stt_dat_drp_ack_stt,
    stt_dat_drp_ack_stt_exp_time,
    stt_dat_drp_ack_stt_gyro,

    /// Add a new status variables address here
    //stt_dat_custom,                 ///< Variable description
    stt_dat_cam_rawimg_count,         ///< Raw images stored


    /// LAST ELEMENT: DO NOT EDIT
    stt_dat_status_last_address           ///< Dummy element, the amount of status variables
} stt_dat_status_address_t;

/**
 * STT structs
 */

typedef struct __attribute__((__packed__)) stt_stt_data {
    uint32_t index;
    uint32_t timestamp;
    float ra;
    float dec;
    float roll;
    int time;
    float exec_time;
} stt_stt_data_t;

typedef struct __attribute__((__packed__)) stt_exp_time_data{
    uint32_t index;
    uint32_t timestamp;
    int exp_time;
    int n_stars;
}stt_exp_time_data_t;

typedef struct __attribute__((__packed__)) stt_gyro_data{
    uint32_t index;
    uint32_t timestamp;
    float gx;
    float gy;
    float gz;
}stt_gyro_data_t;

/**
 * List of status variables with address, name, type and default values
 * This list is useful to decide how to store and send the status variables
 */
static const dat_sys_var_t stt_dat_status_list[] = {
        {stt_dat_obc_last_reset,    "obc_last_reset",    'u', DAT_IS_STATUS, 0},         ///< Last reset source
        {stt_dat_obc_hrs_alive,     "obc_hrs_alive",     'u', DAT_IS_STATUS, 0},          ///< Hours since first boot
        {stt_dat_obc_hrs_wo_reset,  "obc_hrs_wo_reset",  'u', DAT_IS_STATUS, 0},          ///< Hours since last reset
        {stt_dat_obc_reset_counter, "obc_reset_counter", 'u', DAT_IS_STATUS, 0},          ///< Number of reset since first boot
        {stt_dat_obc_sw_wdt,        "obc_sw_wdt",        'u', DAT_IS_STATUS, 0},          ///< Software watchdog timer counter
        {stt_dat_obc_temp_1,        "obc_temp_1",        'f', DAT_IS_STATUS, -1},         ///< Temperature value of the first sensor
        {stt_dat_obc_executed_cmds, "obc_executed_cmds", 'u', DAT_IS_STATUS, 0},
        {stt_dat_obc_failed_cmds,   "obc_failed_cmds",   'u', DAT_IS_STATUS, 0},
        {stt_dat_com_count_tm,      "com_count_tm",      'u', DAT_IS_STATUS, 0},          ///< Number of Telemetries sent
        {stt_dat_com_count_tc,      "com_count_tc",      'u', DAT_IS_STATUS, 0},          ///< Number of received Telecommands
        {stt_dat_com_last_tc,       "com_last_tc",       'u', DAT_IS_STATUS, 0},         ///< Unix time of the last received Telecommand
        {stt_dat_fpl_last,          "fpl_last",          'u', DAT_IS_STATUS, 0},          ///< Last executed flight plan (unix time)
        {stt_dat_fpl_queue,         "fpl_queue",         'u', DAT_IS_STATUS, 0},          ///< Flight plan queue length
        {stt_dat_obc_opmode,        "obc_opmode",        'd', DAT_IS_CONFIG, -1},          ///< General operation mode
        {stt_dat_rtc_date_time,     "rtc_date_time",     'd', DAT_IS_CONFIG, -1},          ///< RTC current unix time
        {stt_dat_drp_idx_temp,      "drp_idx_temp",      'u', DAT_IS_STATUS, 0},          ///< Temperature data index
        {stt_dat_drp_ack_temp,      "drp_ack_temp",      'u', DAT_IS_CONFIG, 0},          ///< Temperature data acknowledge
        {stt_dat_drp_stt,           "drp_stt",           'u', DAT_IS_STATUS, 0},          ///< STT data index
        {stt_dat_drp_stt_exp_time,  "drp_stt_exp_time",  'u', DAT_IS_STATUS, 0},          ///< STT data exposure time index
        {stt_dat_drp_stt_gyro,      "drp_stt_gyro",      'u', DAT_IS_STATUS, 0},          ///< STT data gyro index
        {stt_dat_drp_ack_stt,       "drp_ack_stt",       'u', DAT_IS_CONFIG, 0},          ///< Stt data index acknowledge
        {stt_dat_drp_ack_stt_exp_time, "drp_ack_stt_exp_time",'u', DAT_IS_CONFIG, 0},     ///< Stt data exp time index acknowledge
        {stt_dat_drp_ack_stt_gyro,  "drp_ack_stt_gyro",  'u', DAT_IS_CONFIG, 0},          ///< Stt data gyro index acknowledge
        {stt_dat_cam_rawimg_count,  "cam_rawimg_count",  'd', DAT_IS_STATUS, 0},          ///< Raw images stored
};
///< The dat_status_last_var constant serves for looping through all status variables
static const int stt_dat_status_last_var = sizeof(stt_dat_status_list) / sizeof(stt_dat_status_list[0]);


/**
 * PAYLOAD DATA DEFINITIONS
 */

/**
 * Struct for storing temperature data.
 */
typedef struct __attribute__((__packed__)) sst_temp_data {
    uint32_t index;
    uint32_t timestamp;
    float obc_temp_1;
} stt_temp_data_t;

/**
 * Enum constants for dynamically identifying payload fields at execution time.
 *
 * Also permits adding payload fields cheaply.
 *
 * The last_sensor constant serves only for comparison when looping through all
 * payload values. For example:
 *
 * @code
 * for (payload_id_t i = 0; i < last_sensor; i++)
 * {
 * // some code using i.
 * }
 * @endcode
 */
typedef enum stt_payload_id {
    stt_temp_sensors=0,         ///< Temperature sensors
    stt_stt_sensors,
    stt_exp_time_sensors,
    stt_gyro_sensors,
    stt_last_sensor             ///< Dummy element, the amount of payload variables
} stt_payload_id_t;

/**
 * Struct for storing data collected by status variables.
 */
typedef struct __attribute__((__packed__)) stt_sta_data {
    uint32_t index;
    uint32_t timestamp;
    uint32_t sta_buff[sizeof(stt_dat_status_list) / sizeof(stt_dat_status_list[0])];
} stt_sta_data_t;

static data_map_t stt_data_map[] = {
    {"temp_data",      (uint16_t) (sizeof(stt_temp_data_t)),     stt_dat_drp_idx_temp,     stt_dat_drp_ack_temp,         "%u %u %f",             "sat_index timestamp obc_temp_1"},
    {"stt_data",       (uint16_t) (sizeof(stt_stt_data_t)),      stt_dat_drp_stt,          stt_dat_drp_ack_stt,          "%u %u %f %f %f %d %f", "sat_index timestamp ra dec roll time exec_time"},
    {"stt_exp_time",   (uint16_t) (sizeof(stt_exp_time_data_t)), stt_dat_drp_stt_exp_time, stt_dat_drp_ack_stt_exp_time, "%u %u %d %d",          "sat_index timestamp exp_time n_stars"},
    {"stt_gyro_data", (uint16_t) (sizeof(stt_gyro_data_t)),      stt_dat_drp_stt_gyro,     stt_dat_drp_ack_stt_gyro,     "%u %u %f %f %f",       "sat_index, timestamp gx gy gz"}
};

#endif //REPO_DATA_SCHEMA_STT_H
