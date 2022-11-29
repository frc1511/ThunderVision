# Rolling Raspberry

Rolling Raspberry is FRC Team 1511's software for Raspberry Pi co-processors on its robots. The application's main purpose is to handle Vision Processing with AprilTags.

## Table of Contents

* [Building Program](#building-program)
* [Installing Service](#installing-service)
* [Disabling Service](#disabling-service)
* [Checking Logs](#checking-logs)

## Building Program
Rolling Raspberry obviously needs to be built on a Raspberry Pi running Raspberry Pi OS. It is recommended to be running Debian Bulseye to ensure compatibility. A recent C++20 compiler should be installed, such as GCC 10.

When cloning the repository onto the Raspberry Pi, make sure to resolve all the git submodules,
```
git submodule init
git submodule update
```

To install the required dependicies, run the `setup.sh` script as root.
```
sudo ./scripts/setup.sh
```

To build the application, run the `build.sh` script.
```
./scripts/build.sh
```

## Installing Service
To install the application, run the `install.sh` script as root. When prompted to enable the systemd service, select yes.
```
sudo ./scipts/install.sh
```
Now the application will be installed on the Pi and will start up at boot.

## Disabling Service

To disable the service, use systemctl as shown below,
```
sudo systemctl stop RollingRaspberry.service
sudo systemctl disable RollingRaspberry.service
```

## Checking Logs
To see a realtime feed of the systemd service's logs, use journalctl as shown below,
```
sudo journalctl -f -u RollingRaspberry
```
