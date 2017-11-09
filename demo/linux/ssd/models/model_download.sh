#! /usr/bin/env bash

function download() {
    URL=$1
    MD5=$2
    TARGET=$3

    if [ -e $TARGET ]; then
        md5_result=`md5sum $TARGET | awk -F[' '] '{print $1}'`
        if [ $MD5 == $md5_result ]; then
            echo "$TARGET already exists, download skipped."
            return 0
        fi
    fi

    wget -c $URL -O "$TARGET"
    if [ $? -ne 0 ]; then
        return 1
    fi

    md5_result=`md5sum $TARGET | awk -F[' '] '{print $1}'`
    if [ ! $MD5 == $md5_result ]; then
        return 1
    fi
}

case "$1" in
    "vgg_ssd_net")
    URL="http://cloud.dlnel.org/filepub/?uuid=1116a5f3-7762-44b5-82bb-9954159cb5d4"
    MD5="ad3d2dbd9e7db7e75c004e35822e49eb"
        ;;
    "")
    echo "Usage: sh model_download.sh vgg_ssd_net"
    exit 1
        ;;
    *)
    echo "The "$1" model is not provided currently."
    exit 1
        ;;
esac
TARGET=$1".paddle"

echo "Download "$TARGET" model ..."
download $URL $MD5 $TARGET
if [ $? -ne 0 ]; then
    echo "Fail to download the model!"
    exit 1
fi

exit 0
