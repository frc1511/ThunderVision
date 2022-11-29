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

echo "Installing program 'RollingRaspberry'"

# Install the program.
cmake --install $PROJECT_DIR/build

echo "Installing the configuration json 'RollingRaspberryConfig.json' to '/boot/RollingRaspberryConfig.json'"

# Install the config file.
cp $PROJECT_DIR/RollingRaspberryConfig.json /boot/RollingRaspberryConfig.json

echo "Installing the systemd service 'RollingRaspberry.service'"

# Install the systemd service.
cp $PROJECT_DIR/RollingRaspberry.service /etc/systemd/system/RollingRaspberry.service

echo 'Reloading the systemd daemon'

# Reload the daemon.
systemctl daemon-reload

# Enable the systemd service.
while true; do
  read -p 'Enable and start the systemd service? (y/n) ' yn
  case $yn in
    [Yy]* ) systemctl enable RollingRaspberry.service && systemctl start RollingRaspberry.service; break;;
    [Nn]* ) break;;
    * ) echo 'Please answer y or n';;
  esac
done
