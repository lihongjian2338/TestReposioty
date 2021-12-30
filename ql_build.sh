#!/bin/bash

set -e

function print_err()
{
    echo -en "\033[33;1m[ Failed ]\033[0m "
    echo $1
}

# Usage
function usage()
{
    echo "Usage: setup-drivers.sh [-b path] [-k kernel_version] [-h]"
    echo "    Options:"
    echo "    -b path        Build directory."
    echo "    -k version     Kernel version."
    echo "    -h             Display these help informations."
    exit 0
}

function build_kernel()
{
    build_kernel_source_path=${build_path}/kernel/source

    mkdir -p ${build_kernel_source_path}

    # Qualcomm open driver sources
    if [ x"5.4" = x"${kernel_version}" ]; then
        cp -rf sources/eap-qti-kernel/drivers-5.x ${build_kernel_source_path}/drivers
    else
        cp -rf sources/eap-qti-kernel/drivers ${build_kernel_source_path}
    fi
    cp -rf sources/eap-qti-kernel/include ${build_kernel_source_path}
    cp -rf sources/eap-qti-kernel/Documentation ${build_kernel_source_path}

    # Quectel driver pathcs
    cd ${build_kernel_source_path}
    patch -p1 < ${PWD_DIR}/quectel/data-kernel/eap-quectel-kernel-patch/${kernel_version}/patches/${modem_machine}/0002-Add-Support-Quectel-${modem_machine}-rmnet-usb-driver.patch
    patch -p1 < ${PWD_DIR}/quectel/data-kernel/eap-quectel-kernel-patch/${kernel_version}/patches/${modem_machine}/0003-Add-Support-Quectel-${modem_machine}-usb-bridge-driver.patch
    patch -p1 < ${PWD_DIR}/quectel/data-kernel/eap-quectel-kernel-patch/${kernel_version}/patches/${modem_machine}/0004-Add-Support-Quectel-${modem_machine}-qrtr-driver.patch

    cd - > /dev/null
}

function build_kernel_patch()
{
    build_kernel_patch_path=${build_path}/kernel/patch

    mkdir -p ${build_kernel_patch_path}

    if [ ! -d sources/eap-qti-kernel/patches-${kernel_version} ]; then
        print_err "the ${patches_dir} directory is not exist!!!"
        exit 1
    fi

    if [ x"5.4" = x"${kernel_version}" ]; then
        if [ x"sa415m" = x"${modem_machine}" ]; then
            cp -rf sources/eap-qti-kernel/patches-${kernel_version}/008-Add-support-for-kernel-API-used-in-daig-and-mhi.patch .
            cp -rf sources/eap-qti-kernel/patches-${kernel_version}/009-mhi_bus-core-Add-support-for-MHI-host-interface.patch .
            cp -rf sources/eap-qti-kernel/patches-${kernel_version}/010-net-cdc_ether-Ensure-that-default-MTU-is-minimum-204.patch .
            cp -rf sources/eap-qti-kernel/patches-${kernel_version}/011-net-usbnet-Update-net_device-stats-on-tx-and-rx-succ.patch .
        fi
    else
        cp -rf sources/eap-qti-kernel/patches-${kernel_version}/* ${build_kernel_patch_path}
    fi

    # Quectel driver patchs
    if [ -f quectel/data-kernel/eap-quectel-kernel-patch/${kernel_version}/patches/${modem_machine}/0001-Add-Support-Quectel-${modem_machine}-usb-serial-driver.patch ]; then
        cp -rf quectel/data-kernel/eap-quectel-kernel-patch/${kernel_version}/patches/${modem_machine}/0001-Add-Support-Quectel-${modem_machine}-usb-serial-driver.patch ${build_kernel_patch_path}
    fi
}

function build_kernel_config()
{
    build_kernel_config_path=${build_path}/kernel/config

    mkdir -p ${build_kernel_config_path}
    cp -rf sources/eap-qti-kernel/defconfig ${build_kernel_config_path}

    if [ x"5.4" = x"${kernel_version}" ]; then
        sed -e '/CONFIG_QTI_RMNET/d' ${build_kernel_config_path}/defconfig -i ${build_kernel_config_path}/defconfig
        sed -e '/CONFIG_QTI_QMI_RMNET/d' ${build_kernel_config_path}/defconfig -i ${build_kernel_config_path}/defconfig
        sed -e '/CONFIG_QTI_RMNET_CTL/d' ${build_kernel_config_path}/defconfig -i ${build_kernel_config_path}/defconfig
        sed -e '/CONFIG_QTI_QMI_DFC/d' ${build_kernel_config_path}/defconfig -i ${build_kernel_config_path}/defconfig
    fi
}

function build_rootfs()
{
    build_rootfs_path=${build_path}/rootfs

    mkdir -p ${build_rootfs_path}
    #find prebuilt_HY11 -name \*.tar | xargs -I {} tar -jxf {} -C ${build_rootfs_path} 1>/dev/null
    for tar_file in `find prebuilt_HY11 -name \*.tar`
    do
        if [ x${tar_file##*/} = x"glib-2.0-binaries.tar" ]; then
            tar -jxf ${tar_file} -C ${build_rootfs_path}  usr/include usr/lib
        else
            tar -jxf ${tar_file} -C ${build_rootfs_path}
        fi
    done

    if [ -f ${build_rootfs_path}/etc/systemd/system/qrtr_ns.service ]; then
        # create qrtr.service symbolic link
        ln -s /lib/systemd/system/qrtr_ns.service ${build_rootfs_path}/etc/systemd/system/qrtr.service
    fi
}

## MAIN ##
PWD_DIR=`pwd`
build_path="imx-cv2x-build"
kernel_version="4.14"
modem_machine=`cat sources/meta-quectel-eap/conf/machine/include/modem-machine.conf | grep QUECTEL_MACHINE | awk -F "\"" '{ print $2 }'`

# Usage: setup-drivers.sh -b directory
while getopts :b:k:h opt
do
    case "${opt}" in
        b) build_path=${OPTARG} ;;
        k) kernel_version=${OPTARG} ;;
        h) usage ;;
        *) echo "Unknown option: ${opt}" && exit 1 ;;
    esac
done

if [ x"${modem_machine}" = x"AG15" ]; then
    modem_machine="mdm9150"
elif [ x"${modem_machine}" = x"AG520R" ]; then
    modem_machine="sa415m"
elif [ x"${modem_machine}" = x"AG55xQ" ]; then
    modem_machine="sa515m"
else
    print_err "Can not found modem machine configuration."
    exit 1
fi

build_path=${PWD_DIR}/${build_path}

if [ -d ${build_path} ]; then
    print_err "The \"${build_path}\" directory alread exists, please delete it"
    exit 1
fi

mkdir -p ${build_path}

build_kernel
build_kernel_patch
build_kernel_config
build_rootfs
