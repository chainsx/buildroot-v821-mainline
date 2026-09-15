#!/bin/sh

set -eu

board_dir="$(dirname "$0")"

amp_fw="${AVAOTA_AMP_FW:-}"
if [ -z "${amp_fw}" ] && [ -n "${AW_PACK_DATA_DIR:-}" ]; then
	amp_fw="${AW_PACK_DATA_DIR}/amp_rv0.bin"
fi
if [ -n "${amp_fw}" ] && [ -f "${amp_fw}" ]; then
	mkdir -p "${TARGET_DIR}"/lib/firmware
	install -m 0644 "${amp_fw}" "${TARGET_DIR}"/lib/firmware/amp_rv0.bin
else
	echo "AVAOTA E907 firmware not found; set AW_PACK_DATA_DIR or AVAOTA_AMP_FW" >&2
fi

install -m 0644 "${board_dir}"/extlinux.conf \
	"${BINARIES_DIR}"/extlinux.conf
