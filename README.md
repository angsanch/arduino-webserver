# Arduino Web Server

This program allows your Arduino to run a basic web server.

### Description

If someone wants to use this for anything practical, be warned: the highest speed this server has achieved is `24KB/s`.

I started this project years ago, and after several revisions, it is finally finished. Initially, I used Strings, only to discover memory fragmentation, which led me to arrays and pointers. This project introduced me to many C/C++ concepts and memory management techniques. I wouldn't recommend this to anyone, but I've learned a lot from it.

## Getting Started

### Hardware

- [Arduino UNO](https://store.arduino.cc/products/arduino-uno-rev3) (or similar)
- [Ethernet shield](https://store.arduino.cc/products/arduino-ethernet-shield-2?queryID=undefined)
- Ethernet cable
- USB cable for the Arduino
- SD card

### Dependencies

- [Arduino IDE](https://www.arduino.cc/en/software)
- [SdFat](https://www.arduino.cc/reference/en/libraries/sdfat/)

### Installation

1. Connect the Arduino to the computer using the USB cable.
2. Open the project with Arduino IDE.
3. Consider reviewing the [static configuration](#static-configuration).
4. In Tools > Board, select **Arduino Uno**. If not done automatically, select the USB port in Tools > Port.
5. Press the Upload button on the top left.

## Usage

### Static Configuration

To change these settings, you must recompile the program to the Arduino.

#### Buffers

This configures the amount and size of different buffers. Ensure you leave at least `50B` for local variables.

- **BUFF_SIZE** The size of the main buffer used for various tasks, including reading files. Smaller sizes can slow the server or cause it to fail.

- **CLIENTS** The number of clients that can be in the queue. Only the first client receives active service, as switching between clients takes one full second.

- **FORMAT_SIZE** Maximum length of any hosted file format. Consider this when defining content types.

#### Paths

Locations of files on the SD card. The dynamic configuration explanation below assumes default values.

- **BASE_PATH** The directory containing the files to be hosted.

- **CYCLES_FILE** The file from which the cycles value is read.

- **CONTENT_TYPE_DIR** Where the content types are stored.

- **DEFAULT_FILE** The file read when a directory is requested.

- **IP_FILE** File where the IP should be found

#### Other

- **DEFAULT_CONTENT_TYPE** The content type sent if the requested content type does not exist.

### Dynamic Configuration

#### Connection

Configuration regarding the connection with clients.

- **ip** The IP your server will use. Ensure it is usable on your network. If this file is not present DHCP will be used.

- **port** The port your server will use. It is recommended to use port 80, the standard port for HTTP, but you can use any port in the range `[0-65535]`.

- **cycles** The number of times the buffer is filled and sent to the main client before checking new connections. To determine how many bytes are sent in a full cycle, multiply this number by [BUFF_SIZE](#BUFF_SIZE): `cycles * BUFF_SIZE`.

#### HTTP

Files to directly fulfill requests.

- **ctype** This directory stores the content-type of each file format. Several formats can share a content-type, but each format must have only one content-type.

- **htdocs** This directory contains the hosted files.

- **header** The file from which the header is read. All relevant information is added where the `;` are located. You can safely add more headers at the bottom of the file.

#### Codes

###### 404, 405, 429

Responses for unsuccessful requests. `404` and `405` are used as the body of the response, while `429`, as the server is already under considerable load, is the entire response.

## Authors

* **Daniel Sanchez** ([GitHub](https://github.com/angsanch) / [LinkedIn](https://www.linkedin.com/in/angeldanielsanchez/))

## License

This project is licensed under the GNU Public License version 3.0 - see the [LICENSE](LICENSE) file for details.
