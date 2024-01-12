#!/bin/bash

# Check root.
if [ $EUID -ne 0 ]; then
  echo 'Install Script must be run as Root'
  exit 1
fi

# Check machine.
if [ ! $(uname -o) = 'GNU/Linux' ] || [ ! $(uname -m) = 'armv7l' ]; then
  echo 'What are you doing?? Rolling Raspberry can only be installed on a Raspberry Pi'
  exit 1
fi

PROJECT_DIR=$(realpath $(dirname $BASH_SOURCE))/..

echo "Installing program 'ThunderVision'"

# Install the program.
cmake --install $PROJECT_DIR/build --target ThunderVision

echo "Installing configuration json files to '/var/frc1511/'"

# Install the config files.

mkdir -p /var/frc1511
cp $(find $PROJECT_DIR/server/config -name '*.json') /var/frc1511

echo "Installing systemd service 'ThunderVision.service'"

# Install the systemd service.
cp $PROJECT_DIR/server/ThunderVision.service /etc/systemd/system/ThunderVision.service

echo "Installing systemd service 'MountUSBDrive.service'"

# Install the systemd service.
cp $PROJECT_DIR/server/MountUSBDrive.service /etc/systemd/system/MountUSBDrive.service

echo 'Reloading systemd daemon'

# Reload the daemon.
systemctl daemon-reload

# Enable the systemd service.
while true; do
  read -p 'Enable and start the systemd services? (y/n) ' yn
  case $yn in
    [Yy]* ) systemctl enable ThunderVision.service && systemctl start ThunderVision.service && systemctl enable MountUSBDrive.service && systemctl start MountUSBDrive.service; break;;
    [Nn]* ) break;;
    * ) echo 'Please answer y or n';;
  esac
done
