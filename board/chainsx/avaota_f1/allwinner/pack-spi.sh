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
work="${binaries}/allwinner-spi"

if [ ! -x "${tools}/eDragonEx/dragon" ]; then
    echo "Allwinner packing tool 'eDragonEx/dragon' not found; set AW_PACK_TOOLS_DIR" >&2
    exit 1
fi

mkdir -p "${work}"
cp "${data}/sys_config.fex" "${data}/config.fex" "${data}/split_xxxx.fex" \
   "${data}/sunxi.fex" "${work}/"
cp "${board_dir}/image-spi.cfg" "${work}/image.cfg"
cp "${board_dir}/sys_partition-spi.fex" "${work}/sys_partition.fex"
cp "${binaries}/avaota_f1_buildroot_spi.img" "${work}/full_img.fex"

cd "${work}"
PATH="${tools}/eDragonEx:${PATH}" dragon image.cfg sys_partition.fex
cp avaota_f1_buildroot_phoenix_spi.img "${binaries}/"
