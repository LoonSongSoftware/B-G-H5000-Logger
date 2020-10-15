# Building for Linux

The following build instructions are all command-line based, since the application
has been designed to run on "headless" (no monitor, therefore no GUI) machines. If
you need help accessing the command line on the target machine, see "ACCESS-TERMINAL.md."

They have been tested on both x64 and armv6 machines running Ubuntu Server 20.04, but should
work for other Debian-based Linux configurations as well (with, perhaps some minor changes 
to commands and output).

## Steps to take with an Internet connection

Assuming you don't have ready Internet access on your boat, the following steps
should be taken offsite, where an Internet connection is available.

1. **Install Linux build tools and the `boost` library***

    Other than standard C++ 17, the application relies only on one external dependency:
the `boost` library. That library provides the code for communicating with a websocket-based
server.

    ```bash
    sudo apt-get install -y build-essential libboost-all-dev
    ```

1. **Clone this repository into a local folder**

    The source code is typically stored in a folder in the user's home folder
(in Linux, this might be `/home/scott/`). The `git clone`
command will create a folder called `h5000-logger` by default to hold the source code.

    ```bash
    cd <desired parent folder>
    git clone <url>
    ```

1. **Move to the local `h5000-logger` folder and build the code**.

    ```bash
    cd h5000-logger
    ./make4linux
    ```

1. **Test the application with a generic websocket server**.

    ```bash
    ./h5000-logger echo.websocket.org 80
    ```

1. If necessary, **troubleshoot** the application.

Once you have the expected output, you should be ready to use `h5000-logger` on
the H5000-equipped vessel.

## Steps to take on board the H5000-equipped boat

Once again, access the terminal/command line on the target machine. (For `ssh` 
access, this will likely require knowing the IP address assigned by the system
to the target machine.)

1. **Test the application** again.

    ```bash
    ./h5000-logger xxxxxxxxx xxxxxxxx
    ```

The application will attempt to connect to the H5000 CPU webserver on port 2053
(which is the B&G default). If it succeeds, you should see something like the following:

```bash
<to come>
```

1. If necessary, **troubleshoot** the application.

    * Have you used the correct IP address for the H5000 CPU?
    * Can you ping the H5000 CPU?
    * Can a laptop connected to the same network access the H5000 CPU webserver?
