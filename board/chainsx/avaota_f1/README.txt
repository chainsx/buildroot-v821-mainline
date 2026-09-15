Avaota F1 (Allwinner V821)

Build with an out-of-tree Buildroot output directory:

  make O=output avaota_f1_defconfig
  make O=output -j$(nproc)

The default build creates a raw SD card image and a raw 16 MiB SPI image.
Set BR2_PRIMARY_SITE to a nearby Buildroot source mirror if the default
upstream download servers are slow.

PhoenixSuite images are also generated when AW_PACK_TOOLS_DIR and
AW_PACK_DATA_DIR point to an Allwinner eDragonEx tool installation and its
board data files. The pack tools are proprietary and are therefore not
committed to Buildroot.
