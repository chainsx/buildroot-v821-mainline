#!/bin/sh
set -eu

support/scripts/genimage.sh -c board/chainsx/avaota_f1/genimage.cfg
board/chainsx/avaota_f1/spi/make-spi.sh "${BINARIES_DIR}" "${HOST_DIR}"

if [ -n "${AW_PACK_TOOLS_DIR:-}" ]; then
    board/chainsx/avaota_f1/allwinner/pack-sdcard.sh \
        "${BINARIES_DIR}" "${AW_PACK_TOOLS_DIR}" "${AW_PACK_DATA_DIR:-}"
    board/chainsx/avaota_f1/allwinner/pack-spi.sh \
        "${BINARIES_DIR}" "${AW_PACK_TOOLS_DIR}" "${AW_PACK_DATA_DIR:-}"
fi
