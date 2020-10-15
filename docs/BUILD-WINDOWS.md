# Building for Windows

## Steps to take with an Internet connection

Assuming you don't have ready Internet access on your boat, the following steps
should be taken offsite, where an Internet connection is available.

1. **Install Microsoft Visual Studio 2019 (Community Edition) and the `boost` library***

1. **Clone this repository into a local folder**

    By default, VS2019 stores cloned repositories in the `C:\Users\<username>\Source\Repos` directory.

1. **Build the code**.

1. **Test the application with a generic websocket server**.


1. If necessary, **troubleshoot** the application.

Once you have the expected output, you should be ready to use `h5000-logger` on
the H5000-equipped vessel.

## Steps to take on board the H5000-equipped boat

1. **Test the application** again.


The application will attempt to connect to the H5000 CPU webserver on port 2053
(which is the B&G default). If it succeeds, you should see something like the following:

```bash
<to come>
```

1. If necessary, **troubleshoot** the application.

    * Have you used the correct IP address for the H5000 CPU?
    * Can you ping the H5000 CPU?
    * Can a laptop connected to the same network access the H5000 CPU webserver?
