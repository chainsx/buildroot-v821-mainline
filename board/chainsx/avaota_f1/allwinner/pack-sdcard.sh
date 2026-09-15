#!/bin/sh
set -eu

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 BINARIES_DIR AW_PACK_TOOLS_DIR AW_PACK_DATA_DIR" >&2
    exit 2
fi

binaries=$1
tools=$2
data=$3
board_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
work="${binaries}/allwinner-sdcard"

for tool in eDragonEx/dragon mod_update/script mod_update/update_mbr; do
    if [ ! -x "${tools}/${tool}" ]; then
        echo "Allwinner packing tool '${tool}' not found; set AW_PACK_TOOLS_DIR" >&2
        exit 1
    fi
done

for file in sys_config.fex config.fex split_xxxx.fex sunxi.fex dlinfo.fex; do
    if [ ! -f "${data}/${file}" ]; then
        echo "Allwinner packing data '${file}' not found; set AW_PACK_DATA_DIR" >&2
        exit 1
    fi
done

mkdir -p "${work}"
cp "${data}/sys_config.fex" "${data}/config.fex" "${data}/split_xxxx.fex" \
   "${data}/sunxi.fex" "${data}/dlinfo.fex" "${work}/"
cp "${board_dir}/image-sdcard.cfg" "${work}/image.cfg"
cp "${board_dir}/sys_partition-sdcard.fex" "${work}/sys_partition.fex"
cp "${binaries}/boot.vfat" "${work}/boot.fex"
cp "${binaries}/rootfs.ext4" "${work}/rootfs.fex"
cp "${binaries}/u-boot-sunxi-with-spl.bin" "${work}/boot0_sdcard.fex"
cp "${binaries}/u-boot-sunxi-with-spl.bin" "${work}/boot_package.fex"

PATH="${tools}/eDragonEx:${tools}/mod_update:${PATH}"
cd "${work}"
rm -f sys_partition.bin sunxi_mbr.fex
script sys_partition.fex
update_mbr sys_partition.bin 4
dragon image.cfg sys_partition.fex
cp avaota_f1_buildroot_phoenix_sdcard.img "${binaries}/"
