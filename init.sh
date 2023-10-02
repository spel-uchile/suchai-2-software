#!/usr/bin/env bash
echo "Downloading SUCHAI Flight Software"
[ ! -d suchai-flight-software ] && git clone -b feature/framework https://gitlab.com/spel-uchile/suchai-flight-software.git

#echo "Downloading BUS drivers..."
#cd apps/groundstation/src/drivers || exit
#[ ! -d suchai-drivers-bus ] && git clone https://gitlab.com/spel-uchile/suchai-drivers-bus suchai-drivers-bus
#cd - || exit
