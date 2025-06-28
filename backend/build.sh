#!/bin/bash

cd ~/dev/repos/github/NovaBank/backend/
mkdir build && cd build
cmake ..
make
./novabank

# cd ~/dev/repos/github/NovaBank/backend/build
# make
# ./novabank