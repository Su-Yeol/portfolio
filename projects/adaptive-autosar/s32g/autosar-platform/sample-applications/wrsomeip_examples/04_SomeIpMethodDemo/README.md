# 04 SomeIP Method Demo

This example is demonstrating how a server client connection works. The Example corresponds to the [Request/Response Example of vsomeip](https://github.com/GENIVI/vsomeip/wiki/vsomeip-in-10-minutes#request).

The _ResponseDemo_ is the server side and is offering services on startup. The identifiers of the services are defined in _00\_DemoUtil_. After a default time of 20 seconds the application is stop offering the services and exiting.

The _RequestDemo_ is the client side and waiting for services to be offered. When the services are available or the default time of 20 seconds expired the application will start sending method requests to the services. After that the application exits.

## How to use

0. If your wrseomip configuration is not installed in the default location then set the path with `export WRSOMEIP_CONFIG_PATH="/etc/wrsomeip/wrsomeip.json"`.

1. Start the someip daemon with `./usr/bin/someipDaemon &`.

2. Export the application name and start the server with `export WRSOMEIP_APP_NAME=server && ./usr/bin/ResponseDemo`.

3. Export the application name and start the client with `export WRSOMEIP_APP_NAME=client && ./usr/bin/RequestDemo`.

### Options

- The options available can be printed with `-h`. Eg. `./usr/bin/ResponseDemo -h`.
- a range of services and instances can be demonstrated. With `export WRSOMEIP_APP_NAME=server && ./usr/bin/ResponseDemo -I 2 -S 2` four service instances in total are offered. The services have to be defined in the _wrsomeip.json_.
- extended package size can be tested for Some/IP-TP with the option `-e` eg. `export WRSOMEIP_APP_NAME=client && ./usr/bin/RequestDemo -e`.
