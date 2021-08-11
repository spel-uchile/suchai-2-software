#!/bin/bash
OPTIONS="-DAPP=groundstation -DSCH_COMM_NODE=10 -DSCH_HOOK_COMM=1 -DSCH_CSP_BUFFERS=1000 -DSCH_COM_TX_DELAY_MS=100"
cmake -B build $OPTIONS && cmake --build build