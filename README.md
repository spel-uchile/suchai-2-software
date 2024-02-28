# SUCHAI Flight and Ground Software

This repository contains the source code of the SUCHAI-2, SUCHAI-3 and PlantSat satellites onboard software (aka
flight software), and the ground segment software. Both software are based on the 
[SUCHAI Flight Software](https://gitlab.com/spel-uchile/suchai-flight-software) framework, so please refer to its 
documentation for further details.

## Repository initialization

This repository has the following dependencies:

- SUCHAI Flight Software: https://gitlab.com/spel-uchile/suchai-flight-software

So please run the following to clone/download these dependencies (we decided not using git submodules):

```shell
sh init.sh
```

### Repository structure

The repository simplified structure is as follows:

```shell
suchai-2-software
├── apps
│   ├── groundstation       # Ground station application
│   ├── plantsat            # SUCHAI-2, SUCHAI-3, and PlantSat flight software application
│   └── simple              # Example application (not relevant)
└── suchai-flight-software  # SUCHAI Flight Software repository (external repository)
```

## Build and run

### Dependencies

Please check and install the [SUCHAI Flight Software](https://gitlab.com/spel-uchile/suchai-flight-software) list of 
dependencies depending on your development environment. The following is the (non-comprehensive) dependencies list:

| Library name         | Ubuntu and family | Archlinux and family |
|----------------------|-------------------|----------------------|
| 3.16 <= cmake < 3.25 | cmake             | cmake                |
| gcc >= 7.5           | gcc               | gcc                  |
| make >= 4.1          | make              | make                 |
| ninja >= 1.10.0      | ninja-build       | ninja                |
| python3 >= 3.8       | python3           | python               |
| python2 >= 2.7       | python            | python2              |
| zmq >= 4.2.5         | libzmq3-dev       | zeromq               |
| pyzmq >= 18.1.1      | python3-zmq       | python-pyzmq         |
| pkg-config >= 0.29.1 | pkg-config        | pkgconf              |

NOTE ⚠️
> CMake >= 3.25 sets the `LINUX` variable by default and therefore always activates the variable in 
> `LINUX` in `suchai-flight-software/include/suchai/config.h` affecting the build for other platforms.
> Please use a previous version of CMake, e.g., installed via pip.
> 
> REF: https://cmake.org/cmake/help/latest/variable/LINUX.html

NOTE ⚠️
> Python 2 is required to build the SUCHAI 2, 3 and Plantsat software using the Nanomind A3200 SDK (based on waf).
> `pyenv` can help switching python versions.
> 
> Please contact us if you need access to the SDK repositories.

### Build

For convenience refer to the build scripts that set up the required CMAKE variables and build.

- [build_simple.sh](build_simple.sh)
- [build_groundstation.sh](build_groundstation.sh)
- [build_plantsat.sh](build_plantsat.sh)
- [build_rpi.sh](build_rpi.sh)
- [build_suchai2.sh](build_suchai2.sh)


To run an example we have to build two apps: the `simple` (linux example) and the `groundstation` (ground segment) apps.
For this, we provide convenient scripts that set up the required CMAKE variable and build:

```shell
sh build_simple.sh
sh build_groundstation.sh
```

The results are two executables located at:

```shell
├── build-gnd
│   ├── apps
│   │   └── groundstation
│   │       └── ground-app
└── build-simple
    └── apps
        └── simple
            └── suchai-app
```

## Run

The space and ground segment use the CSP communication protocol. In this test the `libcsp` uses the `ZMQ` interface 
to communicate both ends. So we require a third component, the `ZMQHUB`, as explained in the diagram below:

```mermaid
flowchart LR
    A[Ground App] <== CSP/ZMQ ==> B(ZMQHUB)
    B <== CSP/ZMQ ==> C[Simple App]
```

So the steps to execute our test are the following:

1. In a separate terminal run the provided `ZMQHUB` component (keep it running in background):

    ```shell
    cd suchai-flight-software/sandbox/csp_zmq
    python3 zmqhub.py
    ```

2. Now open two terminals side by side. On the first terminal execute the ground station app:

    ```shell
    ./build-gnd/apps/groundstation/ground-app
    ```
    
    Example output:

    ```
    --------- FLIGHT SOFTWARE START ---------
             Version: 3.0.0.rc-2-11-g6199
             Device : 10 (GROUNDSTATION)
    -----------------------------------------
   
    ______________________________________________________________________________
                      ___ _   _  ___ _  _   _   ___ 
                     / __| | | |/ __| || | /_\ |_ _|
                     \__ \ |_| | (__| __ |/ _ \ | | 
                     |___/\___/ \___|_||_/_/ \_\___|
    ______________________________________________________________________________
    SUCHAI>
    ```
   
3. On the other terminal execute the satellite app:

    ```shell
    ./build-simple/apps/simple/suchai-app
    ```
    
    Example output:

    ```
    --------- FLIGHT SOFTWARE START ---------
         Version: 3.0.0.rc-2-11-g6199
         Device : 2 (PLANTSAT)
    -----------------------------------------

    ______________________________________________________________________________
                      ___ _   _  ___ _  _   _   ___ 
                     / __| | | |/ __| || | /_\ |_ _|
                     \__ \ |_| | (__| __ |/ _ \ | | 
                     |___/\___/ \___|_||_/_/ \_\___|
    ______________________________________________________________________________
    SUCHAI>
    ```
   
If you see an output similar to the listed above, then success.

Now, you can try testing the communication between the ground and space segment. We can
use the `com_ping` command to send a CSP ping from Node 10 (ground station app) to the Node 1
(satellite app):

```mermaid
flowchart LR
    A[GROUND App : 10] <==> B(ZMQHUB)
    A -. com_ping 1 .-> C
    C -. echo .-> A
    B <==> C[SAT App : 11]
```

So in the `ground-app` terminal execute the following:

```shell
SUCHAI> com_ping 11
```

You should see the following output, indicating that a CSP message was sent and a 
response (echo) was received and therefore, the ping command was executed successfully: 

```shell
SUCHAI> com_ping 11
[INFO ][1709132218][Executer] Running the command: com_ping...
OUT: S 10, D 11, Dp 1, Sp 52, Pr 2, Fl 0x00, Sz 10 VIA: ZMQHUB
INP: S 11, D 10, Dp 52, Sp 1, Pr 2, Fl 0x00, Sz 10 VIA: ZMQHUB
[RES  ][1709132218][cmdCOM] Ping to 11 took 1
[INFO ][1709132218][Executer] Command result: 1
SUCHAI> 
```

While the `simple-app` terminal should display the following, indicating that a ping message
was received and a response was sent:

```shell
INP: S 10, D 11, Dp 1, Sp 52, Pr 2, Fl 0x00, Sz 10 VIA: ZMQHUB
SERVICE: Ping received
OUT: S 11, D 10, Dp 52, Sp 1, Pr 2, Fl 0x00, Sz 10 VIA: ZMQHUB
```

The opposite test can be executed from the `simple-app` using the `com_ping 10` command.

## License

This work in licensed under the GNU GPLv3

See [LICENSE](LICENSE) for further details.

## More information

### How to cite

Plain text
```
Garrido, Cristobal, et al. "The First Chilean Satellite Swarm: Approach and Lessons Learned." 
Proceedings of the AIAA/USU Conference on Small Satellites (2023), Year in Review, SSC23-WVII-07.
https://digitalcommons.usu.edu/smallsat/2023/all2023/56/
```