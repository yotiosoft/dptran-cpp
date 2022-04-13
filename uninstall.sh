#!/bin/bash

DEST="${HOME}/dptran_bin"
PROGRAM="dptran"
SRC="${HOME}/.bashrc"

rm -rf ${DEST}
if [ "$(uname -a | grep Darwin)" ]; then
    sed -i '' '/dptran/d' ${SRC}
elif [ "$(uname -a | grep Linux)" ]; then
    sed -i '/dptran/d' ${SRC}
fi
source ${SRC}

echo "アンインストール完了. ターミナルを再起動してください"
