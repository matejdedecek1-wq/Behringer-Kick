#!/usr/bin/env bash

main()
{
	UBOOT_DTS_DIR=$2
	if [ ! -d "$UBOOT_DTS_DIR" ]; then
		echo "ERROR: couldn't find dtb tree: $UBOOT_DTS_DIR"
		exit 1
	fi

	if grep -Eq "^BR2_PACKAGE_FREESCALE_IMX_PLATFORM_IMX8MM=y$" ${BR2_CONFIG}; then
		DTB1=${UBOOT_DTS_DIR}/imx8mm-var-dart-customboard.dtb
		DTB2=${UBOOT_DTS_DIR}/imx8mm-var-som-symphony.dtb
		cat ${BINARIES_DIR}/u-boot-spl.bin ${BINARIES_DIR}/ddr_fw.bin > ${BINARIES_DIR}/u-boot-spl-ddr.bin
		${HOST_DIR}/bin/imx8m_pad_image.sh ${BINARIES_DIR}/bl31.bin
		DEK_BLOB_LOAD_ADDR=0x40400000 BL31=${BINARIES_DIR}/bl31.bin BL33=${BINARIES_DIR}/u-boot-nodtb.bin ATF_LOAD_ADDR=0x00920000 ${HOST_DIR}/bin/mkimage_fit_atf.sh ${DTB2} ${DTB1} > ${BINARIES_DIR}/u-boot.its
		${HOST_DIR}/bin/mkimage -E -p 0x3000 -f ${BINARIES_DIR}/u-boot.its ${BINARIES_DIR}/u-boot.itb
		rm -f ${BINARIES_DIR}/u-boot.its

		${HOST_DIR}/bin/mkimage_imx8 -fit -loader ${BINARIES_DIR}/u-boot-spl-ddr.bin 0x7E1000 -second_loader ${BINARIES_DIR}/u-boot.itb 0x40200000 0x60000 -out ${BINARIES_DIR}/imx8-boot-sd.bin
	fi
	exit $?
}

main $@
