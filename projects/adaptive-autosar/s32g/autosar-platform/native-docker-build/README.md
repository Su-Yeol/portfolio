# Build and Run the UCM Functional Clusters and Applications natively

This guide helps you to build and run the APD's Functional Clusters and Applications natively in the Docker environment without yocto dependency.

## Note

This repo is **archived**. The environment and documentation has been moved to `yocto-layers/docker/devel`.

## Setup

### Prerequisites

- Clone this repository.
- Build the docker image using `Dockerfile` in native folder of yocto-layers repo via Makefile target from this directory.

```sh
# change into directory native-docker-build
make build_dev
```

- The setup is tested with ubuntu distribution 22.04 (Jammy Jellyfish).

### Overview

This setup uses CMakeLists.txt in the top-level directory of `ara-api` and `sample-applications` to build a AP machine which can be used to compile all the Functional Clusters natively.

This repo should be placed parallel to `ara-api`, `sample-applications`.

```none
.
├── ara-api
├── native-docker-build
├── sample-applications
└── yocto-layers
```

### Starting the docker container

To start the docker container use the `dock_dev` Makefile target

```sh
make dock_dev
```

It should show you something like this:

```log
docker network inspect ap-native-network  > /dev/null 2>&1 || \
docker network \
	create \
	--driver=bridge \
	--subnet=192.168.7.0/24 \
	ap-native-network

docker run \
	-it     \
	--rm \
	--ip 192.168.7.2 \
	--net=ap-native-network \
	--volume /home/user/APD/native-docker-build/..:/mnt \
	--name nativebuildapd_user_s_RadarFusionMachine \
	--workdir /mnt/native-docker-build \
	--cap-add=SYS_PTRACE \
	--cap-add=NET_ADMIN \
	--cap-add=SYS_NICE \
	--env TARGET_MACHINE=RadarFusionMachine \
	-p 34902:3490 \
	nativebuildapd_devel:focal || true
To run a command as administrator (user "root"), use "sudo <command>".
See "man sudo_root" for details.

builduser@2af969177c50:/mnt/native-docker-build$ 

```

In the default case without any parameters, a new container is created with name as `RadarFusionMachine`. Additionally, A new bridge type network is created and the machine is added. You should be able to ping it from your host machine.

If you supply `NETWORK=host` the container will share the network stack of the host. In this mode, only one container can run per host because they will conflict on dlt daemon and vsomeip ports.

## Working with the native setup

The following commands have to be executed in the docker container started above.

### Building the machine

There is a build script(`docker_build.sh`) which builds all the components and install the APD libraries and executables in the required locations.

```none
root@e7c61c2389b0:/mnt/native-docker-build# ./docker_build.sh
```

The build script evaluates the environment variable `TARGET_MACHINE` and builds configured applications for this machine.

Optionally for the docker_build.sh script you can specify the build directory, otherwise the default value will be used, e.g.:

```bash
BUILD_DIR=/tmp/apdbuild ./docker_build.sh
```

Note: The script sets `INSTALL_DIR` to `/tmp/apd`. The APD stack can be started directly 
from there, no moving of files into root folder is necessary.

### Running the components

Use the below command to display the logs on the terminal.

```none
root@e7c61c2389b0:/mnt/native-docker-build# export REDIRECT_TERMINAL="false"
```

Run the execution-manager and supply the APD_ROOT parameter.
You can send EM to the background by appending `&` to the below command, so that you can reuse the terminal again.

```none
root@e7c61c2389b0:/mnt/native-docker-build# execution-manager ARA_ROOT=/tmp/apd/ &
```

The above command can also be issued by running Makefile target runapx:

```none
root@e7c61c2389b0:/mnt/native-docker-build# make runapx
```

```none
root@e7c61c2389b0:/mnt/native-docker-build# execution-manager ARA_ROOT=/tmp/apd/ &
```

Clean the UCM temporary data using the make target. It will be helpful for ITM to run multiple times.
This also rebuilds the expected preinstalled software clusters along
with everything else.

```none
root@e7c61c2389b0:/mnt/native-docker-build# make clean_ucm_data
```

### Running machine state and function group settings

You can call the ITM-like machine state script with this Makefile target:

```none
root@e7c61c2389b0:/mnt/native-docker-build# make fgchange
```



### Develop and debug the components using VSCode

#### Prerequisites for debug

Install these extensions in VS Code

* [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers), 
* [Docker](https://marketplace.visualstudio.com/items?itemName=ms-azuretools.vscode-docker)

For debugging and developing build the devel container. It preinstalls the VS code server components and some extensions into the container image so the install step is not repeated on each container instance.

* [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* [Tasks Shell Input](https://marketplace.visualstudio.com/items?itemName=augustocdias.tasks-shell-input)


To start the container, use this make target.

```sh
# Start the devel container
make dock_dev
```

**!! WARNING: Only one container instance per machine should run at a time since they use same IP address and tmp folder !!**

#### Container registry (outdated)

You can pull prebuild images from the native-docker-build repositories container registry.

```bash
# setup your login credentials
docker login code.autosar.org:4567

# pull the image
docker pull code.autosar.org:4567/wg-ucm-coder/native-docker-build
```

##### Maintainer information

These steps are not needed for developers, only added for maintenance documentation.

```bash
# build the image and tag it
docker build -t code.autosar.org:4567/wg-ucm-coder/native-docker-build yocto-layers/docker/native/

# push it into the registry
docker push code.autosar.org:4567/wg-ucm-coder/native-docker-build
```

#### Connect to the container

[Attach visual studio to docker container](https://code.visualstudio.com/docs/remote/attach-container).

![Attach](images/vscode_attach_container.png)

Next you will have to select the container to connect to.

#### Setup the workspace

Add folders for applications you want to develop to your workspace. An example workspace can be found here: [UCM workspace](.vscode/ucm.code-workspace),
see also [how-do-i-open-a-vs-code-workspace](https://code.visualstudio.com/docs/editor/workspaces#_how-do-i-open-a-vs-code-workspace).

##### Launch type debugging

If you want to debug an application manually started, you have to use a launch type configuration. This mode has several issues:

- EM is not aware of the application being executed
- Dependencies and function group or machine states have to be checked manually.
You have to prevent that the application is started by EM as well, e.g. by modifying the execution manifest (`/tmp/apd/opt/process/etc/MANIFEST.json`) `startup_configs.machine_states` value to `Debug`. Other applications that depend on this application can then not be started resp. EM will detect that there are invalid dependencies and stop the whole platform
- `docker_build.sh` script will modify the file on rebuild
- Environment variables and arguments have to be supplied manually

Nevertheless it might be the only way to debug issues at a very early state in the application. You can find an example in the file [launch.json](.vscode/launch.json).

##### Attach type debugging

If you want to debug an application started by execution manager, you have to use attach type debugging.

The file [launch.json](.vscode/launch.json) contains attach debug configurations for some example applications. *Tasks Shell Input* extension is used to get the PID to attach to from the process name. For example, to attach to UCM flashing adapter process, select the entry from the list and start debug. Afterwards, you can set breakpoints and GDB should stop at those.

![Debug](images/vscode_debug_attach.png)

If you want to debug early in your application, you can add a `std::this_thread::sleep_for()` in your main function. However, do it after reporting `ara::exec::ExecutionState::kRunning` to the EM, since otherwise it will detect failed startup of this application and shutdown the platform.

##### Creating and analyzing core dump files

If your application crashes because of segfaults or aborts, it can be useful to activate and analyze a core dump file.

In the host environment, set the variable kernel.core_pattern to desired value, e.g.

```bash
sudo sysctl -w kernel.core_pattern=/tmp/core-%e-%s-%u-%g-%p-%t
```

Now, if an application crashes a core dump file will be created in `/tmp` folder (inside the docker container).
To analyze it, you can use the gdb profile like this:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "cppdbg",
            "request": "launch",
            "name": "Core dump(c/c++)",
            "program": "/opt/AdiOsiAdapter/bin/AdiOsiAdapter", // fill in path to binary
            "coreDumpPath": "${input:coreFileName}",
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb"
        }
    ],
    "inputs": [
        {
            "id": "coreFileName",
            "type": "command",
            "command": "shellCommand.execute",
            "args": {
                "command": "ls -1 /tmp/core-*",
                "description": "select core dump"
            }
        }
    ]
}
```

If you ran execution-manager as root you need to make the coredump files readable for the builduser as by default it is owned by root:

```bash
sudo chmod +r /tmp/core*
```

Next, select the debug profile and choose the core dump file you want to analyze.

![](./images/vscode_coredump_select.png)

Now you can do a postmortem analysis on the dump file.

![](./images/vscode_coredump_analyze.png)

To test if coredump settings are correctly set, you can kill a running process manually with SIGABRT in another shell:

```bash
sudo pkill -ABRT -f "radar"
```

#### ARXML editing

The container has the XML plugin preinstalled and configured with a [catalog file](catalog.xml). Open an ARXML file and you have schema validation and code completion available:

![ARXML](images/vscode_arxml.png)

#### Spec item fingerprint tracing

The devel container also has the [AUTOSAR Spec Item Hover And Fingerprint Tool](https://code.autosar.org/f.frank1/vscode-apd) preinstalled and configured. However, it expects a checkout of the SVN folder as explained in the [README](https://code.autosar.org/f.frank1/vscode-apd/-/blob/master/README.md) in the workspace folder. In the end, it should look like this:

```none
.
├── ara-api
├── native-docker-build
├── sample-applications
├── svn
│   ├── 000_config
│   │   ├── ar_document_catalog.mft.json
│   │   └── config.mft.rb
│   ├── AP_SWS_UpdateAndConfigurationManagement_888
│   │   └── zz_generated
│   │       └── AUTOSAR_AP_SWS_UpdateAndConfigurationManagement.arxml
│   └── ZGEN_Tracing
│       └── 10_Supplement
│           └── spec_item_fingerprints.json
└── yocto-layers
```

For usage, see the linked README.

### Building a subproject

To build a subproject, use this command. It will also build all dependencies. It requires to execute the `docker_build.sh` so that the CMake project is set up.

```sh
cmake --build build/RadarFusionMachine --target apd_updatable_app -j $(nproc)
```


### Cleaning a subproject

To clean a subproject, supply build folder to cmake and call target clean, e.g.:

```sh
cmake --build build/RadarFusionMachine/apd_updatable_app-prefix/src/apd_updatable_app-build --target clean
```

Output should be something like:
```none
[1/1] Cleaning all built files...
Cleaning... 9 files.
```

### Building and Running the Unit test cases
  - Pass `ARA_ENABLE_TESTS=ON` to the cmake for enabling the unit test cases.
    This is default in `docker_build.sh` script.
  - Build the required components using `cmake` command (see above).
    This will install the unit test in the `$INSTALL_DIR` location (default `/tmp/apd`)
  - After running the `docker_build.sh` script, all the unit test executables will be copied to `/usr/bintest/`.

### Execute ITM test

From your host environment (not inside docker container) change to directory [itm](./itm/).
Then execute the script [run_itm.sh](./itm/run_itm.sh) with your parameters. It will run for specified number of runs (or until CTRL^C is hit), produce some stats output and archive logfiles by run number.
Each test is run inside a newly created docker container, so the initial state is always the same. The script builds and installs the APD environment (usually from build cache).

### Working with DLT viewer

Compile and install the DLT viewer as described on this [Wikipage](https://wiki.autosar.org/doku.php?id=ap_dev_guide:infra:vsomeip_dlt). You can build it for Windows or Linux depending on your environment.

#### Attaching to the docker container

The [Makefile](./Makefile) contains parameters that forward the DLT port 3490 to a dedicated host port for each machine. Radar and RadarFusion cannot run simultaneously as they have the same IP address.

|Radar|Fusion|RadarFusion|
|-|-|-|
|34902|34904|34902|

![DLT](./images/dlt_addecu.png)

If you run the docker container on a dedicated machine, you have to replace `localhost` with the IP address (or DNS name) of that machine.

#### Opening DLT Offline files

The ITM script above will create DLT logfiles which can be opened with the DLT viewer.

### Network capture with Wireshark

Wireshark can attach to the bridge device created by docker. Run `ip addr` in a command line to find the interface:

```none
60: br-f3f74332d480: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default
    link/ether 02:42:02:5f:44:3d brd ff:ff:ff:ff:ff:ff
    inet 192.168.7.1/24 brd 192.168.7.255 scope global br-f3f74332d480
       valid_lft forever preferred_lft forever
    inet6 fe80::42:2ff:fe5f:443d/64 scope link
       valid_lft forever preferred_lft forever
```

You can also use `docker network inspect ap-native-network` to show the full `Id` parameter of the bridge.

This is a useful alias if you want to use tcpdump. It also contains

```bash
alias tcpdump_docker='sudo tcpdump -v -w ~/AUTOSAR/native-docker-build/caps/docker_$(date +%Y%m%d%H%M%S).pcap -i br-$(docker network inspect ap-native-network | jq -r  .[].Id | head -c 12) not port 3490 and not port 22 and not port 443'
```

The filter `not port 3490 and not port 22 and not port 443` is recommended (also for Wireshark) to filter out DLT, SSH and HTTPS traffic (from VS Code plugins).

### VSOMEIP tracing

Vsomeip has options to trace all incoming and outgoing messages of applications. To enable this feature, modify these variables in the `docker_build.sh` script.

#### Client side logging

```bash
ENABLE_CLIENTSIDELOGGING="1"
```

This will add clientside logging into all application manifest environment variables. If you only need it for specific applications, then change the `for manifest in $(find /opt -name MANIFEST.json); do` loop into something like `for manifest in /opt/app1/etc/MANIFEST.json /opt/app2/etc/MANIFEST.json; do`.

As a result, you will see handler calls in your application logfile like this for the client side (taken from pkgmgr_sample):

```log
2023/04/13 12:03:36.152551 2191561153 010 ECU1 UCMS MAIN log info V 1 [SWCL check: expecting SWCL_BASE SwclSample]
2023-03-13 12:03:36.525702 [info] application_impl::send: (0110): [0275.032c.74cc:0004:0110] type=0 thread=7f750a06e100
2023-03-13 12:03:36.525827 [info] routing_manager_proxy::send: (0110): [0275.032c.74cc:0005:0110] type=0 thread=7f750a06e100
2023-03-13 12:03:36.539816 [info] Invoking handler: (0110): [0275.032c.74cc:0005] type=0 thread=7f7503fff700
2023/04/13 12:03:36.154023 2191561168 011 ECU1 UCMS MAIN log info V 1 [SWCL check: SWCL 0: UpdatableApp_swcl, 1.0.0-1234567890, kPresent]
```

And this for the service side.

```log
2023-03-13 12:03:36.526435 [info] Invoking handler: (0104): [0275.032c.74cc:0005] type=0 thread=7fc94ec54700
2023/04/13 12:03:36.152713 2191561155 005 ECU1 UCM- PKGM log debug V 2 [CALL FROM CLIENT: GetSwClusterInfo()]
2023/04/13 12:03:36.153837 2191561166 000 ECU1 UCM- FSSM log debug V 3 [RETURN TO CLIENT: GetSwClusterInfo UpdatableApp_swcl/1.0.0-1234567890:Present SwclSample/1.0.0:Present SWCL_BASE/1.0.0-0:Present ]
2023-03-13 12:03:36.538854 [info] application_impl::send: (0104): [0275.032c.74cc:0005:0110] type=80 thread=7fc94d451700
2023-03-13 12:03:36.539024 [info] routing_manager_proxy::send: (0104): [0275.032c.74cc:0005:0110] type=80 thread=7fc94d451700
```

Lines with [info] tag are from the vsomeip library, the other are internal DLT log statements. As seen above, client calls method 0x74cc at service with request id 0x0004 and server responds with response id 0x0005 causing handler invocation at the client.

#### Routingmangerd

```bash
ENABLE_VSOMEIPTRACE="1"
```

This will inject the tracing option into the vsomeip routingmanagerd config file via jq. You will then see (all) message flow inside the file `/var/redirected/routingmanagerd` that passes the network interface. On **RadarFusionMachine**, all communication is handled directly between the applications via unix domain sockets though and you will only see service discovery logging here. In a multi machine setup you will also see SOMEIP network communication here that is then forwarded to the receiving application via its socket.

Example for single machine:

```log
2023-03-13 12:03:36.507023 [info] REQUEST(0110): [0275.032c:1.4294967295]
2023-03-13 12:03:36.515522 [info] Client [100] is closing connection to [109]
2023-03-13 12:03:36.515879 [info] RELEASE(0110): [0275.032c]
2023-03-13 12:03:36.517087 [info] REGISTER EVENT(0110): [0275.032c.a0f6:is_provider=0:reliability=ff]
```

#### Message content logging

Message content logging must be enabled in source code of vsomeip. There are several prepared statements in the vsomeip code, but they are guarded by `#ifdef 0` which must be changed to `#ifdef 1`.

An example patch file can be found here: [enable-vsomeip-tracing.patch](./enable-vsomeip-tracing.patch). It might be needed to rebase or you just search and replace some `ifdef` statements in relevant modules.

## Open points and known issues

### Alternative set up for Windows (Docker for windows)

The native build works in Windows with Docker installed (WSL1 or WSL2 based engine).
To make the native build working on Windows with Docker installed, below changes are required.

First Git and Docker are required, and configuration might be required for company network (e.g. proxy).

This chapter command are for Windows Powershell.

- When cloning the repositories use the option ```--config core.autocrlf=input``` to assure line ending compatibility
  ```none
  git clone https://code.autosar.org/tf-apd/ara-api.git --config core.autocrlf=input
  ```
-  Adapt current folder path variable in docker run command
    ```none
     make dock
    ```

### TLS inspection

If your corporate firewall does TLS inspection you need to install the firewall certificate into the docker container. Otherwise it will fail to download https resources used in the build. Add these lines to the top of the [Dockerfile](docker/Dockerfile) (e.g. after `ENV LANG=C.UTF-8`) and place the certificate in the same folder.

```docker
ADD proxycert.crt /usr/local/share/ca-certificates/proxycert.crt
RUN update-ca-certificates
```

The script [download-vs-code-server.sh](docker/download-vs-code-server.sh) called in the devel [Dockerfile](docker/Dockerfile) tests for the above path and sets an environment variable so that extensions can be installed.

### Corporate proxy

In case docker build fails because of your corporate proxy, try the command with the below arguments :
```
docker build --build-arg HTTP_PROXY=http://login:password@proxy:8080 \
             --build-arg HTTPS_PROXY=http://login:password@proxy:8080 \
             --build-arg http_proxy=http://login:password@proxy:8080 \
             --build-arg https_proxy=http://login:password@proxy:8080 \
              -t nativebuildapd:focal \
              yocto-layers/docker/native/

    where:
    
    login = your Windows account
    password = your Windows password
    proxy = the proxy address of your company e.g.: http.ntlm.mycompany.com
```

### VS Code version matching

The script [download-vs-code-server.sh](docker/download-vs-code-server.sh) downloads the latest release version. This has to match with your host version, otherwise your host VS code will install the server components matching its version when you attach to the container. 

To verify check that `ls ~/.vscode-server/bin/` contains a folder named with the commit hash (here: `7f329fe6c66b0f86ae1574c2911b681ad5a45d63`).

![](./images/vscode_version.png)

If you need a specific version, then replace the `commit_sha` variable in the script with the hash string.

### Permissions

The docker container bind mounts the workspace directory into the container space. To be able to edit files the external host uid should match the internal uid of the builduser (default 1000).
You can customize the uid by an argument to the Dockerfile and calling `make build` or `make build_dev` will do that automatically to use the same id as the caller.

If you use the same image to create containers for several host user ids, you might need to add additional users to match their ids.
