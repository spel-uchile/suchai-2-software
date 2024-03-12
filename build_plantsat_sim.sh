#!/bin/bash
PROGRAM=${1}
OPTIONS="-DAPP=plantsat
-DSCH_OS=LINUX
-DSCH_ARCH=SIM
-DSCH_SEN_ENABLED=1
-DSCH_NAME=\"PLANTSAT\"
-DSCH_LOG=INFO
-DSCH_ST_MODE=SQLITE
-DSCH_COMM_NODE=1
-DSCH_DEVICE_ID=2
-DSCH_HOOK_COMM=1
-DSCH_CSP_BUFFERS=100
-DSCH_COM_TX_DELAY_MS=3000
-DSCH_MAX_WDT_TIMER=60
-DSCH_MAX_GND_WDT_TIMER=5400
-DSCH_TX_FREQ=437230000
-DSCH_TRX_PORT_APP=16"

cd apps/plantsat/src/drivers
[ ! -d "suchai-drivers-bus" ] && sh install.sh
cd -
rm -rf build-suchai-sim
cmake -B build-suchai-sim -G Ninja $OPTIONS && cmake --build build-suchai-sim -j4
