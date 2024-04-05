# Demonstration of USB and FatFs operation of PICO flash memory

This implementation builds a FAT12 file system in the Raspberry Pi Pico's onboard
flash memory, supporting both file operations from inside the Pico using the FatFs
library and from the USB host using the USB Mass storage class.

## Build and install

```
git submodule update --init
cd lib/pico-sdk; git submodule update --init; cd ../../

mkdir build; cd build
cmake ..
make
```

Now you have `picofs.uf2` and can drag and drop it onto your Raspberry Pi Pico to
install and run it.

## Concurrent Access Issues

FAT12 does not take concurrent access into account, so concurrent access, including
updates from the USB host and PICO, will have unintended consequences; with proper
unmounting and mounting operations on the USB host, the files can be manipulated
relatively safely.
For example, if Pico is in the process of continuously appending temperature data to
a file, it will be cached when it is connected to the USB host, and the USB host
will not know about the update.

## Reference

- FatFs http://elm-chan.org/fsw/ff/
- pico-sdk https://github.com/raspberrypi/pico-sdk
