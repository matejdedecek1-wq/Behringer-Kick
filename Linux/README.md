# Kick Linux (Buildroot)

Buildroot tree for the BE 0700-AAM Linux image.

## Building

```sh
sh builder.sh        # installs host packages, then: make KTKick_defconfig && make
```

Board files are in `buildroot/board/freescale/imx8mm_var_som/`, and the configurations are
`buildroot/configs/KTKick_defconfig` and `KTKick_Setu_defconfig`.

The downloaded source tarballs (`buildroot/dl/`) are not included. Buildroot downloads them on the
first build. Some NXP packages (GPU, multimedia codecs, firmware) are downloaded under NXP's own
license terms, which Buildroot will ask you to accept.

The image is a development build: the root account has **no password** and the Dropbear SSH server is
enabled. Do not use it as-is on a networked device.

## License

Buildroot is licensed under GPL-2.0, see `buildroot/COPYING`. Packages built by Buildroot keep their
own licenses. Factory audio samples were removed from the root filesystem overlay.
