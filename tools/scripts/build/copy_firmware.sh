#!/bin/bash

set -e
set -o pipefail
###############################################################################
#Variables
SOURCE_DIR=$1
OUTPUT=$2
IC_CONFIG=$3
BOARD_CONFIG=$4
BIN_FILENAME=$5
PROJ_DIR=$6
FLASHBIN_NAME=flash.bin
BOOTLOADERMAXSIZE=65536  # 64KB


###############################################################################
#Functions
get_board () {
    i=`pwd | awk -F"/" '{print NF}'`
    while [ $i -gt 0 ]; do
        last_name=$item_name
        item_name=`pwd | awk -F"/" '{print $'$i'}'`
        if [ "$item_name" == "project" ]; then
            BOARD=$last_name
            break
        fi
        let i=i-1
    done
    echo BOARD=$BOARD
    if [ "$BOARD" == "" ]; then
        echo "Error: It cannot find board name"
        exit 1
    fi
}

change_bin_filename () {

    awk \
    '{ \
        if ($0 ~ /\[Section.+\]/) { \
            if ($0 ~ /\[Section2\]/) { \
                sect2=1 \
            } else { \
                sect2=0 \
           } \
        } \
    \
        if (sect2==1 && $0~/Location=/) { \
            print "Location="'\"$2\"' \
        } else { \
            print $0 \
        } \
    }' $1 > $1.tmp

    mv $1.tmp $1

}

change_begin_end_address_filename () {

    awk \
    '{ \
        if ($0 ~ /\[Section.+\]/) { \
            if ($0 ~ /\[Section2\]/) { \
                sect2=1 \
            } else { \
                sect2=0 \
           } \
        } \
    \
        if (sect2==1 && $0~/BeginAddress=/) { \
            print "BeginAddress="'\"$2\"' \
        } else if (sect2==1 && $0~/EndAddress=/) { \
            print "EndAddress="'\"$3\"' \
        } else { \
            print $0 \
        } \
    }' $1 > $1.tmp

    mv $1.tmp $1
}

# Combine bootloader and CM4 Firmware if it's neccessary
combine_bl_cm4 () {
    echo "Start combining bootloader & CM4 firmware..."
    CM4_FRIMWARE_BIN=$1/$2
    BOOTLOADER_BIN=$1/$3
    FLASHBIN=$1/$4
    echo "CM4_FRIMWARE_BIN = $CM4_FRIMWARE_BIN"
    echo "BOOTLOADER_BIN = $BOOTLOADER_BIN"
    echo "FLASHBIN = $FLASHBIN"
    if [ -f "$FLASHBIN" ]; then
        echo "delete $FLASHBIN"
        rm $FLASHBIN
    fi
    BOOTLOADER_BIN_TMP=$1/`basename $BOOTLOADER_BIN .bin`.tmp
    cp $BOOTLOADER_BIN $BOOTLOADER_BIN_TMP
    #BOOTLOADER_BIN_SIZE=`wc --bytes $BOOTLOADER_BIN | cut -d ' ' -f1`
    BOOTLOADER_BIN_SIZE=`wc -c $BOOTLOADER_BIN | awk '{print $1}'`
    PADDINGCOUNT=$(($BOOTLOADERMAXSIZE-$BOOTLOADER_BIN_SIZE))
    printf '\377%.0s' $(eval echo "{1..${PADDINGCOUNT}}")  >> $BOOTLOADER_BIN_TMP
    cat $BOOTLOADER_BIN_TMP $CM4_FRIMWARE_BIN > $FLASHBIN
    rm $BOOTLOADER_BIN_TMP
    echo "Done for combining bootloader & CM4 firmware..."
}

###############################################################################
#Begin here
echo `basename $0`"...."
get_board

if [ "$BOARD_CONFIG" == "mt2523_hdk" ]; then
    # copy download config file
    if [ "$MTK_GNSS_ENABLE" == 'y' ]; then
        cp $SOURCE_DIR/project/$BOARD/template/download/gnss/flash_download.cfg $OUTPUT/
    else
        cp $SOURCE_DIR/project/$BOARD/template/download/default/flash_download.cfg $OUTPUT/
    fi

    # copy bootloader
    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        cp $SOURCE_DIR/driver/board/mt25x3_hdk/bootloader/prebuilt_bootloader.bin $OUTPUT/$IC_CONFIG'_bootloader.bin'
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi
    if [ "$WITH_BOOTLOADER" != "n" ]; then
        combine_bl_cm4 $OUTPUT $BIN_FILENAME ${IC_CONFIG}_bootloader.bin $FLASHBIN_NAME
    fi

elif [ "$BOARD_CONFIG" == "mt7687_hdk" ]; then
    # copy download config file

    echo "bin filename is ${BIN_FILENAME}"
    if [ "$BIN_FILENAME" == 'mt7697_iot_sdk_lite.bin' ]; then
	echo "copy 2M flash ini"
        cp $SOURCE_DIR/project/$BOARD/template/download/flash_download_2M.ini $OUTPUT/flash_download.ini
    else
        cp $SOURCE_DIR/project/$BOARD/template/download/flash_download.ini $OUTPUT/
    fi
    change_bin_filename $OUTPUT/flash_download.ini $BIN_FILENAME
    
    LD_FILE_NAME="mt7687_flash.ld"
    if [[ `echo "$BIN_FILENAME" | grep -c  "bootloader"` -gt 0 ]]; then
        LD_FILE_NAME="bootloader.ld"
    fi
    ORIGIN=`grep "XIP_CODE.*(arx)" "$PROJ_DIR/$LD_FILE_NAME" | cut -d ',' -f1 | cut -d '=' -f2 | tr -d ' '`
    LENGTH=`grep "XIP_CODE.*(arx)" "$PROJ_DIR/$LD_FILE_NAME" | cut -d ',' -f2 | cut -d '/' -f1 | cut -d '=' -f2 | tr -d ' '`
    BEGIN_ADDRESS=`printf "0x%08X" $(($ORIGIN & 0x0FFFFFFF))`
    END_ADDRESS=`printf "0x%08X" $(($BEGIN_ADDRESS + $LENGTH))`
    change_begin_end_address_filename $OUTPUT/flash_download.ini $BEGIN_ADDRESS $END_ADDRESS

    # copy WIFI fiwmware
    cp $SOURCE_DIR/driver/chip/mt7687/wifi_n9/WIFI_RAM_CODE_MT76X7_in_flash.bin $OUTPUT/

    # copy bootloader
    if [ ! -f "${OUTPUT}/${IC_CONFIG}_bootloader.bin" ]; then
        if [ "$BOARD" == 'mt7697_hdk' ]; then
            cp $SOURCE_DIR/driver/board/mt76x7_hdk/bootloader/loader_MT7697_inflash.bin $OUTPUT/$IC_CONFIG'_bootloader.bin'
        else
            cp $SOURCE_DIR/driver/board/mt76x7_hdk/bootloader/loader_inflash.bin $OUTPUT/$IC_CONFIG'_bootloader.bin'
        fi
        echo "${OUTPUT}/${IC_CONFIG}_bootloader.bin doesn't exist. copy default bootloader done."
    fi
fi

# copy .cmm file
for i in $SOURCE_DIR/$TARGET_PATH/*.cmm ; do
        [ -e "$i" ] || continue
        echo "cp $i to ${OUTPUT}/"
        cp -f $i ${OUTPUT}/
done

exit 0
