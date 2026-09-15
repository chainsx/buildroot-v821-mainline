#!/bin/sh
set -eu

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 BINARIES_DIR HOST_DIR" >&2
    exit 2
fi

binaries=$1
host_dir=$2
board_dir="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
work="${binaries}/spi"

mkdir -p "${work}"
cp "${binaries}/Image" "${work}/Image"
cp "${binaries}/sun300i-v821-avaota-f1.dtb" "${work}/"
cp "${binaries}/rootfs.cpio.gz" "${work}/"
gzip -n -9 -c "${work}/Image" > "${work}/Image.gz"
cp "${board_dir}/spi/kernel-fit.its" "${work}/"

(
    cd "${work}"
    "${host_dir}/bin/mkimage" -f kernel-fit.its kernel.fit
)

dd if=/dev/zero bs=1M count=16 2>/dev/null | tr '\000' '\377' \
    > "${binaries}/avaota_f1_buildroot_spi.img"
dd if="${binaries}/u-boot-sunxi-with-spl.bin" \
    of="${binaries}/avaota_f1_buildroot_spi.img" conv=notrunc 2>/dev/null
dd if="${work}/kernel.fit" of="${binaries}/avaota_f1_buildroot_spi.img" \
    bs=1M seek=6 conv=notrunc 2>/dev/null
