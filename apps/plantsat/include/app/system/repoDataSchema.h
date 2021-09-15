/**
 * @file  dataSchema.h
 * @author Camilo Rojas M - camrojas@uchile.cl
 * @date 2020
 * @copyright GNU GPL v3
 *
 * This header contains structs symbolizing the schema of data.
 */


#ifndef REPO_DATA_SCHEMA_H
#define REPO_DATA_SCHEMA_H

#include "suchai/log_utils.h"
#include "suchai/storage.h"
#include "app/system/config.h"

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
typedef enum dat_status_address_enum {
    /// OBC: On board computer related variables.
    dat_obc_opmode = 0,           ///< General operation mode
    dat_obc_last_reset,           ///< Last reset source
    dat_obc_hrs_alive,            ///< Hours since first boot
    dat_obc_hrs_wo_reset,         ///< Hours since last reset
    dat_obc_reset_counter,        ///< Number of reset since first boot
    dat_obc_sw_wdt,               ///< Software watchdog timer counter
    dat_obc_temp_1,               ///< Temperature value of the first sensor
    dat_obc_executed_cmds,        ///< Total number of executed commands
    dat_obc_failed_cmds,          ///< Total number of failed commands

    /// DEP: Deployment related variables.
    dat_dep_deployed,             ///< Was the satellite deployed?
    dat_dep_ant_deployed,         ///< Was the antenna deployed?
    dat_dep_date_time,            ///< Antenna deployment unix time

    /// RTC: Rtc related variables
    dat_rtc_date_time,            ///< RTC current unix time

    /// COM: Communications system variables.
    dat_com_count_tm,             ///< Number of Telemetries sent
    dat_com_count_tc,             ///< Number of received Telecommands
    dat_com_last_tc,              ///< Unix time of the last received Telecommand
    dat_com_freq,                 ///< Communications frequency [Hz]
    dat_com_tx_pwr,               ///< TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
    dat_com_baud,                 ///< Baudrate [bps]
    dat_com_mode,                 ///< Framing mode (1: RAW, 2: ASM, 3: HDLC, 4: Viterbi, 5: GOLAY, 6: AX25)
    dat_com_bcn_period,           ///< Number of seconds between trx beacon packets
    dat_obc_bcn_offset,           ///< Number of seconds between obc beacon packets

    /// FPL: Flight plan related variables
    dat_fpl_last,                 ///< Last executed flight plan (unix time)
    dat_fpl_queue,                ///< Flight plan queue length

    /// ADS: Altitude determination system
    dat_ads_omega_x,              ///< Gyroscope acceleration value along the x axis
    dat_ads_omega_y,              ///< Gyroscope acceleration value along the y axis
    dat_ads_omega_z,              ///< Gyroscope acceleration value along the z axis
    dat_tgt_omega_x,              ///< Target acceleration value along the x axis
    dat_tgt_omega_y,              ///< Target acceleration value along the y axis
    dat_tgt_omega_z,              ///< Target acceleration value along the z axis
    dat_ads_mag_x,                ///< Magnetometer value along the x axis
    dat_ads_mag_y,                ///< Magnetometer value along the y axis
    dat_ads_mag_z,                ///< Magnetometer value along the z axis
    dat_ads_pos_x,                ///< Satellite orbit position x (ECI)
    dat_ads_pos_y,                ///< Satellite orbit position y (ECI)
    dat_ads_pos_z,                ///< Satellite orbit position z (ECI)
    dat_ads_tle_epoch,            ///< Current TLE epoch, 0 if TLE is invalid
    dat_ads_tle_last,             ///< Las time position was propagated
    dat_ads_q0,                   ///< Attitude quaternion (Inertial to body)
    dat_ads_q1,                   ///< Attitude quaternion (Inertial to body)
    dat_ads_q2,                   ///< Attitude quaternion (Inertial to body)
    dat_ads_q3,                   ///< Attitude quaternion (Inertial to body)
    dat_tgt_q0,                   ///< Target quaternion (Inertial to body)
    dat_tgt_q1,                   ///< Target quaternion (Inertial to body)
    dat_tgt_q2,                   ///< Target quaternion (Inertial to body)
    dat_tgt_q3,                   ///< Target quaternion (Inertial to body)

    /// EPS: Energy power system
    dat_eps_vbatt,                ///< Voltage of the battery [mV]
    dat_eps_cur_sun,              ///< Current from boost converters [mA]
    dat_eps_cur_sys,              ///< Current from the battery [mA]
    dat_eps_temp_bat0,            ///< Battery temperature sensor

    /// Memory: Current payload memory addresses
    dat_drp_idx_temp,             ///< Temperature data index
    dat_drp_idx_ads,              ///< ADS data index
    dat_drp_idx_eps,              ///< EPS data index
    dat_drp_idx_sta,              ///< Status data index
    dat_drp_idx_stt,              ///< Data from STT index
    dat_drp_idx_rw,               ///< Temperature data index

    /// Memory: Current send acknowledge data
    dat_drp_ack_temp,             ///< Temperature data acknowledge
    dat_drp_ack_ads,              ///< ADS data index acknowledge
    dat_drp_ack_eps,              ///< EPS data index acknowledge
    dat_drp_ack_sta,              ///< Status data index acknowledge
    dat_drp_ack_stt,              ///< Data from STT index acknowledge
    dat_drp_ack_rw,               ///< Temperature data acknowledge

    /// Sample Machine: Current state of sample status_machine
    dat_drp_mach_action,          ///< Current action of sampling state machine
    dat_drp_mach_state,           ///< Current state of sampling state machine
    dat_drp_mach_step,            ///< Step in seconds of sampling state machine
    dat_drp_mach_payloads,        ///< Binary data storing active payload being sampled
    dat_drp_mach_left,            ///< Samples left for sampling state machine

    /// Add a new status variables address here
    //dat_custom,                 ///< Variable description

    /// LAST ELEMENT: DO NOT EDIT
    dat_status_last_address           ///< Dummy element, the amount of status variables
} dat_status_address_t;

///< Define opeartion modes
#define DAT_OBC_OPMODE_DEPLOYING     (0) ///< Normal operation
#define DAT_OBC_OPMODE_NORMAL        (1) ///< Normal operation
#define DAT_OBC_OPMODE_WARN          (2) ///< Fail safe operation
#define DAT_OBC_OPMODE_FAIL          (3) ///< Generalized fail operation
#define DAT_OBC_OPMODE_REF_POINT     (4) ///< Point to vector
#define DAT_OBC_OPMODE_NAD_POINT     (5) ///< Point to nadir
#define DAT_OBC_OPMODE_DETUMB_MAG    (6) ///< Detumbling

///< Define is a variable is config or status
#define DAT_IS_CONFIG 0
#define DAT_IS_STATUS 1

/**
 * A system variable (status or config) with an address, name, type and value
 */
#define MAX_VAR_NAME 24

typedef struct __attribute__((packed)) dat_sys_var {
    uint16_t address;   ///< Variable address or index (in the data storage)
    char name[MAX_VAR_NAME];      ///< Variable name (max 24 chars)
    char type;          ///< Variable type (u: uint, i: int, f: float)
    int8_t status;      ///< Variable is status (1), is config (0), or uninitialized (-1)
    value32_t value;    ///< Variable default value
} dat_sys_var_t;

/**
 * A system variable (status or config) with an address, and value
 * A short version to be sent as telemetry
 */
typedef struct __attribute__((packed)) dat_sys_var_short {
    uint16_t address;   ///< Variable address or index (in the data storage)
    value32_t value;    ///< Variable default value
} dat_sys_var_short_t;

/**
 * List of status variables with address, name, type and default values
 * This list is useful to decide how to store and send the status variables
 */
static const dat_sys_var_t dat_status_list[] = {
        {dat_rtc_date_time,     "rtc_date_time",     'd', DAT_IS_CONFIG, 0},          ///< RTC current unix time
        {dat_obc_last_reset,    "obc_last_reset",    'u', DAT_IS_STATUS, 0},          ///< Last reset source
        {dat_obc_opmode,        "obc_opmode",        'd', DAT_IS_CONFIG, DAT_OBC_OPMODE_DEPLOYING}, ///< General operation mode
        {dat_obc_hrs_alive,     "obc_hrs_alive",     'u', DAT_IS_STATUS, 0},          ///< Hours since first boot
        {dat_obc_hrs_wo_reset,  "obc_hrs_wo_reset",  'u', DAT_IS_STATUS, 0},          ///< Hours since last reset
        {dat_obc_reset_counter, "obc_reset_counter", 'u', DAT_IS_STATUS, 0},          ///< Number of reset since first boot
        {dat_obc_sw_wdt,        "obc_sw_wdt",        'u', DAT_IS_STATUS, 0},          ///< Software watchdog timer counter
        {dat_obc_temp_1,        "obc_temp_1",        'f', DAT_IS_STATUS, -1},         ///< Temperature value of the first sensor
        {dat_obc_executed_cmds, "obc_executed_cmds", 'u', DAT_IS_STATUS, 0},          ///< Execute commands counter
        {dat_obc_failed_cmds,   "obc_failed_cmds",   'u', DAT_IS_STATUS, 0},          ///< Commands execute with errors counter
        {dat_dep_deployed,      "dep_deployed",      'u', DAT_IS_STATUS, 2},          ///< Was the satellite deployed?
        {dat_dep_ant_deployed,  "dep_ant_deployed",  'u', DAT_IS_STATUS, 1},          ///< Was the antenna deployed?
        {dat_dep_date_time,     "dep_date_time",     'u', DAT_IS_STATUS, 0},          ///< Antenna deployment unix time
        {dat_com_count_tm,      "com_count_tm",      'u', DAT_IS_STATUS, 0},          ///< Number of Telemetries sent
        {dat_com_count_tc,      "com_count_tc",      'u', DAT_IS_STATUS, 0},          ///< Number of received Telecommands
        {dat_com_last_tc,       "com_last_tc",       'd', DAT_IS_STATUS, 0},          ///< Unix time of the last received Telecommand
        {dat_com_freq,          "com_freq",          'u', DAT_IS_CONFIG, SCH_TX_FREQ},       ///< Communications frequency [Hz]
        {dat_com_tx_pwr,        "com_tx_pwr",        'u', DAT_IS_CONFIG, SCH_TX_PWR},        ///< TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
        {dat_com_baud,          "com_baud",          'u', DAT_IS_CONFIG, SCH_TX_BAUD},       ///< Baudrate [bps]
        {dat_com_mode,          "com_mode",          'u', DAT_IS_CONFIG, 0},          ///< Framing mode (1: RAW, 2: ASM, 3: HDLC, 4: Viterbi, 5: GOLAY, 6: AX25)
        {dat_com_bcn_period,    "com_bcn_period",    'u', DAT_IS_CONFIG, SCH_TX_BCN_PERIOD},  ///< Number of seconds between trx beacon packets
        {dat_obc_bcn_offset,    "obc_bcn_offset",    'u', DAT_IS_CONFIG, SCH_OBC_BCN_OFFSET}, ///< Number of seconds between obc beacon packets
        {dat_fpl_last,          "fpl_last",          'd', DAT_IS_STATUS, 0},          ///< Last executed flight plan (unix time)
        {dat_fpl_queue,         "fpl_queue",         'u', DAT_IS_STATUS, 0},          ///< Flight plan queue length
        {dat_ads_omega_x,       "ads_omega_x",       'f', DAT_IS_STATUS, -1},         ///< Gyroscope acceleration value along the x axis
        {dat_ads_omega_y,       "ads_omega_y",       'f', DAT_IS_STATUS, -1},         ///< Gyroscope acceleration value along the y axis
        {dat_ads_omega_z,       "ads_omega_z",       'f', DAT_IS_STATUS, -1},         ///< Gyroscope acceleration value along the z axis
        {dat_ads_mag_x,         "ads_mag_x",         'f', DAT_IS_STATUS, -1},         ///< Magnetometer value along the x axis
        {dat_ads_mag_y,         "ads_mag_y",         'f', DAT_IS_STATUS, -1},         ///< Magnetometer value along the y axis
        {dat_ads_mag_z,         "ads_mag_z",         'f', DAT_IS_STATUS, -1},         ///< Magnetometer value along the z axis
        {dat_ads_pos_x,         "ads_pos_x",         'f', DAT_IS_STATUS, -1},         ///< Satellite orbit position x (ECI)
        {dat_ads_pos_y,         "ads_pos_y",         'f', DAT_IS_STATUS, -1},         ///< Satellite orbit position y (ECI)
        {dat_ads_pos_z,         "ads_pos_z",         'f', DAT_IS_STATUS, -1},         ///< Satellite orbit position z (ECI)
        {dat_ads_tle_epoch,     "ads_tle_epoch",     'd', DAT_IS_STATUS, 0},          ///< Current TLE epoch, 0 if TLE is invalid
        {dat_ads_tle_last,      "ads_tle_last",      'u', DAT_IS_STATUS, 0},          ///< Last time position was propagated
        {dat_ads_q0,            "ads_q0",            'f', DAT_IS_STATUS, 0},          ///< Attitude quaternion (Inertial to body)
        {dat_ads_q1,            "ads_q1",            'f', DAT_IS_STATUS, 0},          ///< Attitude quaternion (Inertial to body)
        {dat_ads_q2,            "ads_q2",            'f', DAT_IS_STATUS, 0},          ///< Attitude quaternion (Inertial to body)
        {dat_ads_q3,            "ads_q3",            'f', DAT_IS_STATUS, 0},          ///< Attitude quaternion (Inertial to body)
        {dat_tgt_omega_x,       "tgt_omega_x",       'f', DAT_IS_CONFIG, 0},          ///< Target acceleration value along the x axis
        {dat_tgt_omega_y,       "tgt_omega_y",       'f', DAT_IS_CONFIG, 0},          ///< Target acceleration value along the y axis
        {dat_tgt_omega_z,       "tgt_omega_z",       'f', DAT_IS_CONFIG, 0},          ///< Target acceleration value along the z axis
        {dat_tgt_q0,            "tgt_q0",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_tgt_q1,            "tgt_q1",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_tgt_q2,            "tgt_q2",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_tgt_q3,            "tgt_q3",            'f', DAT_IS_CONFIG, 0},          ///< Target quaternion (Inertial to body)
        {dat_eps_vbatt,         "eps_vbatt",         'u', DAT_IS_STATUS, 0},          ///< Voltage of the battery [mV]
        {dat_eps_cur_sun,       "eps_cur_sun",       'u', DAT_IS_STATUS, 0},          ///< Current from boost converters [mA]
        {dat_eps_cur_sys,       "eps_cur_sys",       'u', DAT_IS_STATUS, 0},          ///< Current from the battery [mA]
        {dat_eps_temp_bat0,     "eps_temp_bat0",     'd', DAT_IS_STATUS, 0},          ///< Battery temperature sensor
        {dat_drp_idx_temp,      "drp_temp",          'u', DAT_IS_STATUS, 0},          ///< Temperature data index
        {dat_drp_idx_ads,       "drp_ads",           'u', DAT_IS_STATUS, 0},          ///< ADS data index
        {dat_drp_idx_eps,       "drp_eps",           'u', DAT_IS_STATUS, 0},          ///< EPS data index
        {dat_drp_idx_sta,       "drp_sta",           'u', DAT_IS_STATUS, 0},          ///< Status data index
        {dat_drp_idx_stt,       "drp_stt",           'u', DAT_IS_STATUS, 0},          ///< STT data index
        {dat_drp_idx_rw,        "drp_idx_rw",        'u', DAT_IS_STATUS, 0},          ///< RW data index
        {dat_drp_ack_temp,      "drp_ack_temp",      'u', DAT_IS_CONFIG, 0},          ///< Temperature data acknowledge
        {dat_drp_ack_ads,       "drp_ack_ads",       'u', DAT_IS_CONFIG, 0},          ///< ADS data index acknowledge
        {dat_drp_ack_eps,       "drp_ack_eps",       'u', DAT_IS_CONFIG, 0},          ///< EPS data index acknowledge
        {dat_drp_ack_sta,       "drp_ack_sta",       'u', DAT_IS_CONFIG, 0},          ///< Status data index acknowledge
        {dat_drp_ack_stt,       "drp_ack_stt",       'u', DAT_IS_CONFIG, 0},          ///< Stt data index acknowledge
        {dat_drp_ack_rw,        "drp_ack_rw",        'u', DAT_IS_CONFIG, 0},          ///< RW data acknowledge
        {dat_drp_mach_action,   "drp_mach_action",   'u', DAT_IS_STATUS, 0},          ///<
        {dat_drp_mach_state,    "drp_mach_state",    'u', DAT_IS_STATUS, 0},          ///<
        {dat_drp_mach_left,     "drp_mach_left",     'u', DAT_IS_STATUS, 0},          ///<
        {dat_drp_mach_step,     "drp_mach_step",     'd', DAT_IS_CONFIG, 0},          ///<
        {dat_drp_mach_payloads, "drp_mach_payloads", 'u', DAT_IS_CONFIG, 0}           ///<
};
///< The dat_status_last_var constant serves for looping through all status variables
static const int dat_status_last_var = sizeof(dat_status_list) / sizeof(dat_status_list[0]);

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

typedef enum payload_id {
    temp_sensors=0,         ///< Temperature sensors
    ads_sensors,            ///< Ads sensors
    eps_sensors,            ///< Eps sensors
    status_sensors,         ///< Status Variables
    stt_sensors,            ///<
    rw_sensors,             ///< RW Speed and current sensor
    //custom_sensor,           ///< Add custom sensors here
    last_sensor             ///< Dummy element, the amount of payload variables
} payload_id_t;

/**
 * Struct for storing temperature data.
 */
typedef struct __attribute__((__packed__)) temp_data {
    uint32_t index;
    uint32_t timestamp;
    int16_t obc_temp_1;
    int16_t obc_temp_2;
    int16_t obc_temp_3;
    int16_t eps_temp1;
    int16_t eps_temp2;
    int16_t eps_temp3;
    int16_t eps_temp4;
    int16_t bat_temp1;
    int16_t bat_temp2;
    int16_t istage_temp1;
    int16_t istage_temp2;
    int16_t istage_temp3;
    int16_t istage_temp4;
    int16_t spanel_temp1;
    int16_t spanel_temp2;
    int16_t spanel_temp3;
    int16_t spanel_temp4;
    int16_t is2_int_temp1;
    int16_t is2_int_temp2;
    int16_t is2_int_temp3;
    int16_t is2_int_temp4;
    int16_t is2_ext_temp1;
    int16_t is2_ext_temp2;
    int16_t is2_ext_temp3;
    int16_t is2_ext_temp4;
} temp_data_t; //2*4+24*2 bytes = 56

static char temp_var_string[] = "sat_index timestamp obc_temp_1 obc_temp_2 obc_temp_3 eps_temp1 eps_temp2 eps_temp3 "
                                  "eps_temp4 bat_temp1 bat_temp2 istage_temp1 istage_temp2 istage_temp3 istage_temp4 "
                                  "spanel_temp1 spanel_temp2 spanel_temp3 spanel_temp4 is2_int_temp1 is2_int_temp2 "
                                  "is2_int_temp3 is2_int_temp4 is2_ext_temp1 is2_ext_temp2 is2_ext_temp3 is2_ext_temp4";

static char temp_var_types[] = "%u %u %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h %h";

/**
 * Struct for storing data collected by ads sensors.
 */
typedef struct __attribute__((__packed__)) ads_data {
    uint32_t index;
    uint32_t timestamp;
    float acc_x;            ///< Gyroscope acceleration value along the x axis
    float acc_y;            ///< Gyroscope acceleration value along the y axis
    float acc_z;            ///< Gyroscope acceleration value along the z axis
    float mag_x;            ///< Magnetometer x axis
    float mag_y;            ///< Magnetometer y axis
    float mag_z;            ///< Magnetometer z axis
    int32_t sun1;           ///< Coarse sun sensor 1
    int32_t sun2;           ///< Coarse sun sensor 2
    int32_t sun3;           ///< Coarse sun sensor 3
    int32_t sun4;           ///< Coarse sun sensor 4
    int32_t sun5;           ///< Coarse sun sensor 5
    int32_t sun6;           ///< Coarse sun sensor 6
} ads_data_t;

/**
 * Struct for storing data collected by eps housekeeping.
 */
typedef struct __attribute__((__packed__)) eps_data {
    uint32_t index;
    uint32_t timestamp;
    uint32_t cursun; ///< Current from boost converters [mA]
    uint32_t cursys; ///< Current out of battery [mA]
    uint32_t vbatt; ///< Voltage of battery [mV]
    int32_t temp1; ///< EPS temperature = 10*(TEMP1+TEMP2+TEMP3+TEMP4)/4
    int32_t temp2; ///< BAT temperature = 10*(BATT0 + BATT1)/2
} eps_data_t;

/**
 * Struct for storing data collected by stt.
 */
typedef struct __attribute__((__packed__)) stt_data {
    uint32_t index;
    uint32_t timestamp;
    float ra;
    float dec;
    float roll;
    int time;
    float exec_time;
} stt_data_t;

/**
 * Struct for storing data collected by status variables.
 */
typedef struct __attribute__((__packed__)) sta_data {
    uint32_t index;
    uint32_t timestamp;
    uint32_t dat_obc_opmode;
    uint32_t dat_rtc_date_time;
    uint32_t dat_obc_last_reset;
    uint32_t dat_obc_hrs_alive;
    uint32_t dat_obc_hrs_wo_reset;
    uint32_t dat_obc_reset_counter;
    uint32_t dat_obc_executed_cmds;
    uint32_t dat_obc_failed_cmds;
    uint32_t dat_com_count_tm;
    uint32_t dat_com_count_tc;
    uint32_t dat_com_last_tc;
    uint32_t dat_fpl_last;
    uint32_t dat_fpl_queue;
    uint32_t dat_ads_tle_epoch;
    uint32_t dat_eps_vbatt;
    uint32_t dat_eps_cur_sun;
    uint32_t dat_eps_cur_sys;
    uint32_t dat_obc_temp_1;
    uint32_t dat_eps_temp_bat0;
    uint32_t dat_drp_mach_action;
    uint32_t dat_drp_mach_state;
    uint32_t dat_drp_mach_payloads;
    uint32_t dat_drp_mach_step;
} status_data_t;

static char status_var_string[] = "sat_index timestamp dat_obc_opmode rtc_date_time obc_last_reset obc_hrs_alive "
                                  "obc_hrs_wo_reset obc_reset_counter obc_executed_cmds obc_failed_cmds com_count_tm "
                                  "com_count_tc com_last_tc fpl_last fpl_queue ads_tle_epoch eps_vbatt eps_cur_sun "
                                  "eps_cur_sys obc_temp_1 eps_temp_bat0 drp_mach_action drp_mach_state drp_mach_payloads "
                                  "drp_mach_step";

static char status_var_types[] = "%u %u %u %d %u %u %u %u %u %u %u %u %d %d %u %d %d %u %u %f %d %u %u %u %u";

/**
 * Struct for storing rw data.
 */
typedef struct __attribute__((__packed__)) rw_data {
    uint32_t index;
    uint32_t timestamp;
    float current1;
    float current2;
    float current3;
    int32_t speed1;
    int32_t speed2;
    int32_t speed3;
} rw_data_t;

static data_map_t data_map[] = {
        {"dat_temp_data",    (uint16_t) (sizeof(temp_data_t)),   dat_drp_idx_temp, dat_drp_ack_temp, temp_var_types, temp_var_string},
        {"dat_ads_data",     (uint16_t) (sizeof(ads_data_t)),    dat_drp_idx_ads,  dat_drp_ack_ads,  "%u %u %f %f %f %f %f %f %d %d %d %d", "sat_index timestamp acc_x acc_y acc_z mag_x mag_y mag_z sun1 sun2 sun3 sun4"},
        {"dat_eps_data",     (uint16_t) (sizeof(eps_data_t)),    dat_drp_idx_eps,  dat_drp_ack_eps,  "%u %u %u %u %u %d %d",                "sat_index timestamp cursun cursys vbatt temp_eps temp_bat"},
        {"dat_sta_data",     (uint16_t) (sizeof(status_data_t)), dat_drp_idx_sta,  dat_drp_ack_sta,  status_var_types, status_var_string},
        {"dat_stt_data",     (uint16_t) (sizeof(stt_data_t)),    dat_drp_idx_stt,  dat_drp_ack_stt,  "%u %u %f %f %f %d %f",    "sat_index timestamp ra dec roll time exec_time"},
        {"dat_rw_data",      (uint16_t) (sizeof(temp_data_t)),   dat_drp_idx_rw,   dat_drp_ack_rw,   "%u %u %f %f %f %d %d %d", "sat_index timestamp current1 current2 current3 speed1 speed2 speed3"}
};

/** The repository's name */
#define DAT_TABLE_STATUS "dat_status"      ///< Status variables table name
#define DAT_TABLE_DATA   "dat_data"        ///< Data storage table name
#define DAT_TABLE_FP     "dat_flightplan"  ///< Flight plan table name

/**
 * Search and return a status variable definition from dat_status_list by index or by name
 * @param address Variable index
 * @param name Variable name
 * @return dat_sys_var_t or 0 if not found.
 */
dat_sys_var_t dat_get_status_var_def(dat_status_address_t address);
dat_sys_var_t dat_get_status_var_def_name(char *name);

/**
 * Print the names and values of a system status variable list.
 * @param status Pointer to a status variables list
 */
void dat_print_system_var(dat_sys_var_t *status);

#endif //REPO_DATA_SCHEMA_H
