#!/usr/bin/env bash
git submodule update --init --recursive

cd suchai-flight-software
build_cmd="LINUX X86 --hk 0 --drivers"
echo $build_cmd
python3 compile.py $build_cmd
cd -

if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake ..
make
cd -
