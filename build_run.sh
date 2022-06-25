#!/bin/bash
# Will clean build if no directory called "build" exists or -
# a parameter "-clean" was passed
export LDFLAGS="-Wl,--copy-dt-needed-entries"

clean=$1

if [ ! -d "build" ] || [ "$clean" == "-clean" ]; then
    rm -rf "build";
    mkdir "build";
    cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .. && cd ..
fi

cp -r ./resources/*/** ./build/ 
cd build && make && ./GOF
