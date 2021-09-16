#!/bin/bash
OPTIONS="-DAPP=groundstation -DSCH_OS=LINUX -DSCH_ARCH=X86 -DSCH_NAME=\"GROUNDSTATION\" -DSCH_ST_MODE=SQLITE -DSCH_COMM_NODE=10 -DSCH_DEVICE_ID=10 -DSCH_HOOK_COMM=1 -DSCH_CSP_BUFFERS=1000 -DSCH_COM_TX_DELAY_MS=100 -DSCH_MAX_WDT_TIMER=99999"
cmake -B build -G Ninja $OPTIONS && cmake --build build -j4
