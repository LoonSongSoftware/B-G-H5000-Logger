# H5000 Logger

An application to log an H5000 CPU websocket data stream to two files:

* a raw log of all monitored data items (a .log file), and 

* an Expedition-style .csv file (with observations grouped by timestamp).

This application has been designed to run a Raspberry Pi, but it can be
compiled and run on other machines as well.

**This application was written for a single use case, but it is hoped that
it will be useful to others. Comments on the documentation, functionality and,
in particular, specific pull requests are encouraged.**

## Introduction

This application was developed for several reasons. Principal among them was to
allow constant logging of vessel data on a lightweight computer as a historical
log, for performance evaluation purposes, and for instrument calibration. (The
latter two of these functions are provided by separate applications that interpret
the log files created by this program.) For more detail, see "USE-CASE.md."

## Building the Application - Debian
### Install prerequisites

```shell
sudo apt install libboost-dev
```

### Compile the application
```shell
cd B-G-H5000-Logger
./make4linux
```

## Running the Application

### Examples

Display 'usage' information, with available options:

      h5000-logger

Test connectivity to the websocket.org echo server:

      h5000-logger -h echo.websocket.org -p 80 -t

Establish a live connection to a B&G H5000 CPU, with both CSV and "flatfile" output:

      h5000-logger -h 192.168.77.234 -p 2053 -c -f -o d:\ 

Convert an existing "flatfile" into the corresponding CSV file format:

      h5000-logger -l d:\flatfile.log -o d:\

## External Requirements

1) A **B&G H5000 CPU** with access to its "webserver" interface. This interface is
described in the B&G "H5000 Operation Manual."

1) In particular, you'll need to know the **IP address** of the H5000 CPU. Methods
for determining this address are also described in the H5000 Operation Manual.

1) A **suitable computer** (ranging from a desktop(!) to a laptop to a single-board
computer that can connect to the H5000 CPU webserver (i.e., has compatible&mdash;Ethernet 
or WiFi, as required&mdash;capabilities). This application was developed for a 
"headless" (no attached monitor) Raspberry Pi that connects to the CPU via GoFree WiFi.

## Building H5000 Logger

H5000 Logger is distributed as source code, which means that (for now) it needs to 
be built by the user.

Two build environments are currently supported:

1) A **Debian-based (such as Ubuntu) Linux machine**.

2) A **Windows machine** with **Visual Studio 2019** (including basic C++ components)
installed.

*For development purposes, it may be most efficient to alter and debug (to the extent
possible) the code on an Internet-based Windows (or Linux) machine with a monitor and 
GUI, then to copy the code to the headless machine and compile it locally for 
execution using only command-line commands.*

For detailed instructions on building the application in these environments, see
"BUILD-LINUX.md" and "BUILD-WINDOWS.md."
