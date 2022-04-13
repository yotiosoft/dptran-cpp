#!/bin/bash

DEST="${HOME}/dptran"
PROGRAM="dptran"

mkdir -p ${DEST}
install -s ${PROGRAM} ${DEST}
export PATH=${PATH}:${DEST}
source .bashrc
