#!/bin/bash
PROGRAM=${1}
OPTIONS="-DAPP=plantsat -DSCH_OS=FREERTOS -DSCH_ARCH=NANOMIND -DSCH_SEN_ENABLED=0 -DSCH_NAME=\"PLANTSAT\" -DSCH_LOG=DEBUG -DSCH_ST_MODE=FLASH -DSCH_COMM_NODE=3 -DSCH_DEVICE_ID=3 -DSCH_HOOK_COMM=0 -DSCH_CSP_BUFFERS=100 -DSCH_COM_TX_DELAY_MS=1000"
cmake -B build $OPTIONS #&& cmake --build build

source ~/.profile
export LC_ALL=C
cd apps/plantsat/src/drivers
sh build.sh $PROGRAM
cd -