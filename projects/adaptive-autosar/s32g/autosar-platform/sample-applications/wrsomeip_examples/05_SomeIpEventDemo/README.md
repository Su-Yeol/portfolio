# 05 SomeIP Event Demo

This example is demonstrating how a client can subscribe to events and be notified by a server. The Example corresponds to the [Subscribe/Notify Example of vsomeip](https://github.com/GENIVI/vsomeip/wiki/vsomeip-in-10-minutes#subscribe--notify).

The _NotifyDemo_ is the server side and is offering a service on startup. The identifiers of the service is defined in _00\_DemoUtil_. The server is offering the service and event. The application is than waiting for a subscription for a default of 20 seconds. After that the application is stop offering the services and exiting.

The _SubscribeDemo_ is the client side and waiting for services to be offered. The application is subscribing to the default event, defined in _00\_DemoUtil_, when the service becomes available. After a default of 20 seconds the application exits independent of the service to become available.

## How to use

0. If your wrseomip configuration is not installed in the default location then set the path with `export WRSOMEIP_CONFIG_PATH="/etc/wrsomeip/wrsomeip.json"`.
1. Start the someip daemon with `./usr/bin/someipDaemon &`.
2. Export the application name and start the server with `export WRSOMEIP_APP_NAME=server && ./usr/bin/NotifyDemo`.
3. Export the application name and start the client with `export WRSOMEIP_APP_NAME=client && ./usr/bin/SubscribeDemo`.

### Options

- The options available can be printed with `-h`. Eg. `./usr/bin/NotifyDemo -h`.
- a range of services and instances can be demonstrated. With `export WRSOMEIP_APP_NAME=server && ./usr/bin/NotifyDemo -I 2 -S 2` four service instances in total are offered. The services have to be defined in the _wrsomeip.json_.
