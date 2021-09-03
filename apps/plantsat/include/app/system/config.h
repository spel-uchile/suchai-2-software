#ifndef SUCHAI_APP_CONFIG_H
#define	SUCHAI_APP_CONFIG_H

#define SCH_TNC_ADDRESS        9  ///< Nanocom GS100 address
#define SCH_TRX_ADDRESS        5  ///< Nanocom GS100 address
#define SCH_TX_FREQ            425000000  ///< Nanocom TX freq in Hz
#define SCH_TX_PWR             0  ///< Nanocom TX power (0: 25dBm, 1: 27dBm, 2: 28dBm, 3: 30dBm)
#define SCH_TX_BAUD            4800  ///< Nanocom TX Baudrate bps (4800, 9600, 19200)
#define SCH_TX_BCN_PERIOD      600  ///< Number of seconds between trx beacon packets
#define SCH_OBC_BCN_OFFSET     600  ///< Number of seconds between obc beacon packets
#define SCH_WDT_PERIOD          120

#define SCH_HK_ENABLED 1
#define SCH_SEN_ENABLED 1
#define SCH_ADCS_ENABLED 0

#endif //SUCHAI_APP_CONFIG_H
