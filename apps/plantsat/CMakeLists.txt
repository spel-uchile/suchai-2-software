###
# PLANTSAT APP CMAKE FILE
##
set(SCH_TX_INHIBIT 10 CACHE STRING "Default silent time in seconds [0, 1800 (30min)]")
set(SCH_TX_FREQ 437250000 CACHE STRING "Nanocom TX freq in Hz")
set(SCH_TX_PWR 0 CACHE STRING "Nanocom TX power")
set(SCH_TX_BAUD 4800 CACHE STRING "Nanocom TX Baudrate")
set(SCH_TX_BCN_PERIOD 600 CACHE STRING "Number of seconds between trx beacon packets")
set(SCH_OBC_BCN_OFFSET 600 CACHE STRING "Number of seconds between obc beacon packets")
set(SCH_HK_ENABLED 1 CACHE BOOL "Enable task housekeeping")
set(SCH_SEN_ENABLED 1 CACHE BOOL "Enable task sensors")
set(SCH_ADCS_ENABLED 0 CACHE BOOL "Enable task adcs")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/app/system/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/app/system/config.h)


set(SOURCE_FILES
        src/system/main.c
        src/system/cmdADCS.c
        src/system/cmdAX100.c
        src/system/cmdEPS.c
        src/system/cmdGSSB.c
        src/system/cmdRW.c
        src/system/cmdSensors.c
        src/system/cmdCDH.c
        src/system/hookCommunications.c
        src/system/taskHousekeeping.c
        src/system/taskSensors.c
        src/system/taskADCS.c
        src/drivers/rwdrv10987.c
)

set(GS_INCLUDE_PATH
        src/drivers/suchai-drivers-bus/lib/libparam_client/src
        src/drivers/suchai-drivers-bus/lib/libparam_client/include/deprecated
        src/drivers/suchai-drivers-bus/lib/libparam_client/include
        src/drivers/suchai-drivers-bus/lib/libparam_client/include/gs
        src/drivers/suchai-drivers-bus/lib/libgscsp/include
        src/drivers/suchai-drivers-bus/lib/nanocom-ax_client/include
        src/drivers/suchai-drivers-bus/lib/libutil/include/deprecated/gs/gosh
        src/drivers/suchai-drivers-bus/lib/libutil/include
        src/drivers/suchai-drivers-bus/lib/libutil/include/gs
        src/drivers/suchai-drivers-bus/lib/libutil
        src/drivers/suchai-drivers-bus/lib/nanopower_client
        src/drivers/suchai-drivers-bus/lib/nanopower-bpx_client/include
        src/drivers/suchai-drivers-bus/lib/libgssb_client/include

        src/drivers/suchai-drivers-obc/lib/liba3200/include
        src/drivers/suchai-drivers-obc/lib/liba3200/lib/liba3200_client/include
        src/drivers/suchai-drivers-obc/lib/libasf/avr32/drivers/wdt
        src/drivers/suchai-drivers-obc/lib/libasf/avr32/drivers/gpio
        src/drivers/suchai-drivers-obc/lib/libasf/avr32/drivers/spi
        src/drivers/suchai-drivers-obc/lib/libasf/avr32/drivers/scif
        src/drivers/suchai-drivers-obc/lib/libasf/common/boards
        src/drivers/suchai-drivers-obc/lib/libasf/common/services/clock
        src/drivers/suchai-drivers-obc/lib/libasf/avr32/drivers/ebi/sdramc
        src/drivers/suchai-drivers-obc/lib/libthirdparty/include
        src/drivers/suchai-drivers-obc/lib/libembed/include
)

include_directories(include/app/drivers ${GS_INCLUDE_PATH})
add_executable(suchai-app ${SOURCE_FILES})
target_include_directories(suchai-app PUBLIC ${GS_INCLUDE_PATH})
target_include_directories(suchai-app PUBLIC include)
target_link_libraries(suchai-app PUBLIC suchai-fs-core)
