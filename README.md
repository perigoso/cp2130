# CP2130 C Layer

This is an open source C library for use with the <a href="https://www.silabs.com/products/interface/usb-bridges/classic-usb-bridges/device.cp2130">CP2130</a> USB-SPI Bridge by Silabs Writen by <a href="https://github.com/gimbas">Rafael Silva</a>

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

There is an [Example](src/cp2130example.c) file to get you started.

### Prerequisites

- You will need root access to a Linux machine.
- You will need to install libusb
```
sudo apt-get install libudev-dev libusb-1.0-0-dev
```

### Compiling

A step by step series of examples that tell you how to get a development environment running

- Clone this repository to your working directory

```
sudo git clone https://github.com/gimbas/cp2130
```

- Go to the source directory

```
cd cp2130/
```

- Compile

```
make
```

See the [Makefile](Makefile) for more information on compile flags etc.

## Running

- You may need to add the provided udev rules to the /etc/udev/rules.d directory.

```
sudo cp cp2130.rules /etc/udev/rules.d
```
- Depending on your user permissions, you might need to run the compilled binary with root access (sudo)

## Authors

* **Rafael Silva** - [gimbas](https://github.com/crying-face-emoji)

See also the list of [contributors](https://github.com/gimbas/cp2130/graphs/contributors) who participated in this project.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* <a href="https://github.com/Henneberg-Systemdesign/cp2130">Henneberg-Systemdesign</a> - *cp2130 kernel driver*
* <a href="https://github.com/HarmonInstruments/CP2130">HarmonInstruments</a> - *cp2130 c++ wraper*
