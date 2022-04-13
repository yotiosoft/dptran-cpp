#!/bin/bash

DEST="${HOME}/dptran_bin"
PROGRAM="dptran"
SRC="${HOME}/.bashrc"

mkdir -p ${DEST}
install -s ${PROGRAM} ${DEST}
if [ "$(uname -a | grep Darwin)" ]; then
    sed -i '' '/dptran/d' ${SRC}
elif [ "$(uname -a | grep Linux)" ]; then
    sed -i '/dptran/d' ${SRC}
fi
echo "export PATH=\"\$PATH:${DEST}\"" >> ${SRC}
source ${SRC}

echo "インストール完了. ターミナルを再起動してください"
