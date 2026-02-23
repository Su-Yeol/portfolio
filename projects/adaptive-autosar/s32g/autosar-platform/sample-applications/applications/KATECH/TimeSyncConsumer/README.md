## 1. Overview
The "TimeSyncConsumer" application demonstrates the Time Slave functionality of Adaptive AUTOSAR's Time Synchronization
concept.

The "TimeSyncConsumer" acts as a Time Slave reading and displaying the Time value from the Time Master every second.

The "TimeSyncConsumer" app synchronizes time locally (with the "TimeSyncProvider" app that acts as the Time Master) using ARA::TSYNC shared memory IPC  
and over Ethernet (PTP) via the ARA::TSYNC ara_tsync_daemon app.

For local time synchronization, the "TimeSyncConsumer" app is deployed into the "core-image-apd-devel" image and for Ethernet time synchronization, the  
"TimeSyncConsumer" app and the "ara_tsync_daemon" app are deployed into the "core-image-apd-devel-fusion" image.

## 2. How to Run (Local Time Synchronization)
- ara-project/build$ bitbake core-image-apd-devel
- ara-project/build$ ../yocto-layers/meta-ara/scripts/runqemu-x86-autosar core-image-apd-devel REDIRECT_TERMINAL=0

## 3. How to Run (Ethernet Time Synchronization)
- ara-project/build$ bitbake core-image-apd-devel-fusion
- ara-project/build$ ../yocto-layers/meta-ara/scripts/runqemu-x86-autosar core-image-apd-devel-fusion REDIRECT_TERMINAL=0

`NOTE:`
- The Time Synchronization consumer interface and "ara_tsync_daemon" configuration are currently specified in a custom JSON file, "TimeSyncConsumerInterface.json".

  `TODO:` Replace this JSON with the corresponding ARXMLs according to the TPS_MANIFEST specification (Chapter 9.4 Time Synchronization Deployment) and move the  
  "ara_tsync_daemon" PTP configuration to a custom JSON file or use PROCESS-ARGUMENTS and specify them as command-line arguments when launching the app. 
