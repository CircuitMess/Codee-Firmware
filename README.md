# Codee Firmware

# Building

To build the Codee base firmware, you'll need the ESP-IDF. You can find the getting started
guide [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/). The
production firmware is built using IDF version 5.3.1.

Clone all submodules from the project root

```shell
git submodule update --init --recursive
```

### LovyanGFX patching

Contained in the repository is [LovyanGFX.patch](LovyanGFX.patch).
Apply it onto the cloned LovyanGFX submodule:

```shell
cd components/LovyanGFX
git apply ../../LovyanGFX.patch
```

After patching, in the root directory of the project:

**To build the firmware** run ```idf.py build```

**To upload the firmware to the device** run ```idf.py -p <PORT> flash```. Replace `<PORT>` with
the port Codee is attached to, for ex. ```COM6``` or ```/dev/ttyACM0```.

### Merging a single binary file

After building (or flashing) the firmware, you can merge the resulting files into a single
binary file. This is useful for sharing a build or flashing it directly onto the board.

The necessary tool for this is [esptool](https://github.com/espressif/esptool), which comes
preinstalled with ESP-IDF

From the project root go into your build folder (usually just /build):

```shell
cd build
esptool --chip esp32s3 merge_bin -o Codee.bin --flash_mode dio --flash_freq 80m --flash_size 4MB --fill-flash-size 4MB 0x0 bootloader/bootloader.bin 0x20000 Codee-Firmware.bin 0x10000 partition_table/partition-table.bin 0x11a000 storage.bin
```

# Restoring the stock firmware

To restore the stock firmware, you can download the prebuilt binary on
the [releases page](https://github.com/CircuitMess/Codee-Firmware/releases) of this repository
and flash it manually using esptool:

```shell
esptool write_flash 0x0 Codee-Firmware.bin
```

Alternatively, you can also do so using [CircuitBlocks](https://code.circuitmess.com/) by
logging in, clicking the "Restore Firmware" button in the top-right corner, and following the
on-screen instructions.