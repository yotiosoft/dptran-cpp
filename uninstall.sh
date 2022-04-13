#!/bin/bash

DEST="${HOME}/dptran_bin"
PROGRAM="dptran"
SRC="${HOME}/.bash_profile"

rm -rf ${DEST}
sed -i '' '/dptran/d' ${SRC}
source ${SRC}
