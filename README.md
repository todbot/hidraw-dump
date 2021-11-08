# hidraw-dump

Use Linux HIDRAW to print info about HID devices (USB &amp; Bluetooth/BLE),
including, most importantly, HID Report Descriptors.

## Demo


## Installation

```
apt-get install build-essential pkg-config libudev-dev
git clone https://github.com/todbot/hidraw-dump
cd hidraw-dump
make
sudo ./hidraw-dump
```

## Parsing output

To parse the HID Report Descriptor, use either [`hidrd-convert`](https://github.com/DIGImend/hidrd)
or https://eleccelerator.com/usbdescreqparser/





