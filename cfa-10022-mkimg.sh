#!/bin/sh
IMAGE_NAME=cfa-10022.img
IMAGE_NAME_BASE=${IMAGE_NAME%%.img}
IMAGE_SIZE_MB=500
FAT_IMAGE_SIZE_MB=17
BOOT_SECT_OFFSET=1
EXT3_IMAGE_SIZE_MB=$(( $IMAGE_SIZE_MB - $FAT_IMAGE_SIZE_MB - $BOOT_SECT_OFFSET ))

function assemble_image {
    echo Assemble the image...
    POSIXLY_CORRECT=1
    rm -f $1
    dd if=${2} of=${1} seek=$BOOT_SECT_OFFSET bs=1M count=$FAT_IMAGE_SIZE_MB status=noxfer > /dev/null 2>&1
    dd if=${3} of=${1} bs=1M seek=$(( $FAT_IMAGE_SIZE_MB + $BOOT_SECT_OFFSET )) count=$EXT3_IMAGE_SIZE_MB status=noxfer > /dev/null 2>&1
    rm -f $2 $3
}

function part_image {
    echo Partition the image...
    parted $1 'mktable msdos'
    parted $1 "mkpart primary fat16 $BOOT_SECT_OFFSET $(( $FAT_IMAGE_SIZE_MB + $BOOT_SECT_OFFSET ))"
    parted $1 "mkpart primary ext3 $(( $FAT_IMAGE_SIZE_MB + $BOOT_SECT_OFFSET + 1)) -1"
    parted $1 "set 1 lba off"
}

function mk_blank_parts {
    echo Make blank sub images...
    POSIXLY_CORRECT=1
    dd if=/dev/zero of=${1}-fat.img bs=1M count=$FAT_IMAGE_SIZE_MB status=noxfer > /dev/null 2>&1
    dd if=/dev/zero of=${1}-ext3.img bs=1M count=$EXT3_IMAGE_SIZE_MB status=noxfer > /dev/null 2>&1
    mkfs.vfat -n cfboot ${1}-fat.img > /dev/null 2>&1
    mkfs.ext3 -q -F -L cfroot ${1}-ext3.img > /dev/null 2>&1
}

function fill_parts {
    MPT=temp_dir_$$
    mkdir -p $MPT
    echo Populate FAT image...
    sudo mount -t vfat -o loop,uid=$UID ${1}-fat.img $MPT
    fakeroot cp fat_files/* $MPT
    sudo umount $MPT
    echo Populate ext3 image...
    sudo mount -o loop ${1}-ext3.img $MPT
    sudo tar -x -C $MPT -f rootfs.tar
    #sudo tar -x -C $MPT -f modules.tgz
    if [ -e add.tar ]; then
        echo Adding Customizations...
        sudo tar -x -C $MPT -f add.tar
    fi
    if [ -e uImage ]; then
        echo Adding Updated Kernel...
        sudo cp uImage $MPT/boot/.
    fi
    sudo umount $MPT
    #fsck.ext3 -f ${1}-ext3.img

    rm -fr $MPT
}

#echo $IMAGE_SIZE_MB $FAT_IMAGE_SIZE_MB $EXT3_IMAGE_SIZE_MB 
#exit 1

mk_blank_parts ${IMAGE_NAME_BASE}

fill_parts ${IMAGE_NAME_BASE}

assemble_image ${IMAGE_NAME} ${IMAGE_NAME_BASE}-fat.img ${IMAGE_NAME_BASE}-ext3.img

echo Syncing...
sync

part_image $IMAGE_NAME

exit
