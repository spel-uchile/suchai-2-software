cmake_minimum_required(VERSION 3.5)
project(Payload-Software)

set(CMAKE_C_STANDARD 99)

set(PAYLOAD_SOURCE_FILES
        src/system/repoDataSchema.c
        src/system/taskInit.c
        src/system/cmdPayload.c
        src/system/taskPayload.c
)

set(PAYLOAD_DRIVERS_FILES
        suchai-flight-software/src/drivers/x86/init.c
        suchai-flight-software/src/drivers/x86/data_storage.c
        suchai-flight-software/src/drivers/x86/sgp4/src/c/SGP4.c
        suchai-flight-software/src/drivers/x86/sgp4/src/c/TLE.c
        suchai-flight-software/src/drivers/x86/linenoise/linenoise.c
)

set(SUCHAI_SOURCE_FILES
        suchai-flight-software/src/os/Linux/osDelay.c
        suchai-flight-software/src/os/Linux/osQueue.c
        suchai-flight-software/src/os/Linux/osScheduler.c
        suchai-flight-software/src/os/Linux/osSemphr.c
        suchai-flight-software/src/os/Linux/osThread.c
        suchai-flight-software/src/os/Linux/pthread_queue.c
        suchai-flight-software/src/lib/math_utils.c
        suchai-flight-software/src/lib/log_utils.c
        suchai-flight-software/src/system/globals.c
        suchai-flight-software/src/system/repoCommand.c
        suchai-flight-software/src/system/repoData.c
        suchai-flight-software/src/system/taskDispatcher.c
        suchai-flight-software/src/system/taskExecuter.c
        suchai-flight-software/src/system/taskCommunications.c
        suchai-flight-software/src/system/taskFlightPlan.c
        suchai-flight-software/src/system/taskConsole.c
        suchai-flight-software/src/system/taskWatchdog.c
        suchai-flight-software/src/system/cmdDRP.c
        suchai-flight-software/src/system/cmdOBC.c
        suchai-flight-software/src/system/cmdCOM.c
        suchai-flight-software/src/system/cmdFP.c
        suchai-flight-software/src/system/cmdTM.c
        suchai-flight-software/src/system/cmdConsole.c
        suchai-flight-software/src/system/main.c
)

include_directories(
        suchai-flight-software/src/drivers/x86/include
        suchai-flight-software/src/drivers/x86/libcsp/include
        suchai-flight-software/src/drivers/x86/linenoise
        suchai-flight-software/src/drivers/x86/sgp4/src/c
        suchai-flight-software/src/lib/include
        suchai-flight-software/src/os/include
        suchai-flight-software/src/system/include
        src/drivers/include
        src/system/include
        /usr/include/postgresql
)

link_directories(
    suchai-flight-software/src/drivers/x86/libcsp/lib
)

link_libraries(-lcsp -lzmq -lsqlite3 -lpq -lpthread -lm)

# Use pthread_setname_np included in <features.h>
add_definitions(-D_GNU_SOURCE)

add_executable(payload-software ${SUCHAI_SOURCE_FILES} ${PAYLOAD_DRIVERS_FILES} ${PAYLOAD_SOURCE_FILES})
