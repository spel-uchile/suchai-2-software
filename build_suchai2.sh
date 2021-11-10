#!/bin/bash
PROGRAM=${1}
OPTIONS="-DAPP=plantsat -DSCH_OS=FREERTOS -DSCH_ARCH=NANOMIND -DSCH_SEN_ENABLED=1 -DSCH_NAME=\"SUCHAI2\" -DSCH_LOG=INFO -DSCH_ST_MODE=FLASH -DSCH_COMM_NODE=1 -DSCH_DEVICE_ID=2 -DSCH_HOOK_COMM=1 -DSCH_CSP_BUFFERS=100 -DSCH_COM_TX_DELAY_MS=3000 -DSCH_MAX_WDT_TIMER=120"
cmake -B build-suchai $OPTIONS #&& cmake --build build

source ~/.profile
export LC_ALL=C
cd apps/plantsat/src/drivers
sh build.sh $PROGRAM
status=$?
cd -
exit $status