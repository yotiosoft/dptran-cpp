#!/bin/bash

DEST="${HOME}/dptran_bin"
PROGRAM="dptran"
SRC="${HOME}/.bash_profile"

rm -rf ${DEST}
if [ "$(uname)" == 'Darwin' ]; then
    sed -i '' '/dptran/d' ${SRC}
elif [ "$(expr substr $(uname -s) 1 5)" == 'Linux' ]; then
    sed -i '/dptran/d' ${SRC}
fi
source ${SRC}
