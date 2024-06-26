#!/bin/sh
echo "Checking os..."
uname -a

echo "Checking locale..."
locale

echo "Checking avr32-gcc Version..."
avr32-gcc --version
avr32-as --version

if  [ -n "$1" ] && [ $1 = "--ssh" ]; then
    SSH_ACTIVE="TRUE"
fi
echo SSH_ACTIVE=$SSH_ACTIVE

echo "Downloading OBC drivers SDK..."
if [ ! -d "suchai-drivers-obc" ]; then
    if [ -z "$SSH_ACTIVE" ]; then
         git clone -b framework https://gitlab.com/spel-uchile/suchai-drivers-obc
    else
         git clone -b framework git@gitlab.com:spel-uchile/suchai-drivers-obc.git
    fi
else
    cd suchai-drivers-obc
    git pull
    cd -
fi

echo "Linking source code into SDK..."
cd suchai-drivers-obc
ln -s -f ../../../../../suchai-flight-software
ln -s -f ../../../ app
cd -

echo "Downloading BUS drivers..."
if [ ! -d suchai-drivers-bus ]; then
    if [ -z "$SSH_ACTIVE" ]; then
        git clone https://gitlab.com/spel-uchile/suchai-drivers-bus suchai-drivers-bus
    else
        git clone git@gitlab.com:spel-uchile/suchai-drivers-bus.git suchai-drivers-bus
    fi
else
    cd suchai-drivers-bus
    git pull
    cd -
fi

echo "Linking BUS drivers into SDK"
cd suchai-drivers-obc/lib/
# Link all bus libraries
# for f in ../../suchai-drivers-bus/lib/*; do ln -s -f $f; done
# Link only used bus libraries
ln -s -f ../../suchai-drivers-bus/lib/libgssb_client
ln -s -f ../../suchai-drivers-bus/lib/libparam_client
ln -s -f ../../suchai-drivers-bus/lib/nanocom-ax_client
ln -s -f ../../suchai-drivers-bus/lib/nanopower-bpx_client
ln -s -f ../../suchai-drivers-bus/lib/nanopower_client
cd -

echo "Preparing build tools"
cd suchai-drivers-obc
python2 ./tools/buildtools/gsbuildtools_bootstrap.py

cd -
