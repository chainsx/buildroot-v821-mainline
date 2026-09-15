#!/bin/sh

set -eu

board_dir="$(dirname "$0")"

install -m 0644 "${board_dir}"/extlinux.conf \
	"${BINARIES_DIR}"/extlinux.conf
