## 1. Overview
The "TimeSyncProvider" application demonstrates the Time Master functionality of Adaptive AUTOSAR's Time Synchronization concept.

The "TimeSyncProvider" acts as a Time Master providing a "GPSClock" that toggles between two timepoints every 10 seconds.

The "TimeSyncProvider" app synchronizes time locally using ARA::TSYNC shared memory IPC and over Ethernet (PTP) via the ARA::TSYNC ara_tsync_daemon app.

For local time synchronization, the "TimeSyncProvider" app is deployed into the "core-image-apd-devel" image and for Ethernet time synchronization, the  
"TimeSyncProvider" app and the "ara_tsync_daemon" app are deployed into the "core-image-apd-devel-radar" image.

## 2. How to Run (Local Time Synchronization)
- ara-project/build$ bitbake core-image-apd-devel
- ara-project/build$ ../yocto-layers/meta-ara/scripts/runqemu-x86-autosar core-image-apd-devel REDIRECT_TERMINAL=0

## 3. How to Run (Ethernet Time Synchronization)
- ara-project/build$ bitbake core-image-apd-devel-radar
- ara-project/build$ ../yocto-layers/meta-ara/scripts/runqemu-x86-autosar core-image-apd-devel-radar REDIRECT_TERMINAL=0

`NOTE:`
- The Time Synchronization Provider interface and "ara_tsync_daemon" configuration are currently specified in a custom JSON file, "TimeSyncProviderInterface.json".

  `TODO:` Replace this JSON with the corresponding ARXMLs according to the TPS_MANIFEST specification (Chapter 9.4 Time Synchronization Deployment) and move the   
  "ara_tsync_daemon" PTP configuration to a custom JSON file or use PROCESS-ARGUMENTS and specify them as command-line arguments when launching the app. 
