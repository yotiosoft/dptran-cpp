#!/bin/bash

DEST="${HOME}/dptran_bin"
PROGRAM="dptran"
SRC="${HOME}/.bash_profile"

mkdir -p ${DEST}
install -s ${PROGRAM} ${DEST}
echo "export PATH=\"\$PATH:${DEST}\"" >> ${SRC}
source ${SRC}
