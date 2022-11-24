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

SCRIPT_DIR=$(realpath $(dirname $BASH_SOURCE))

# Install the program.
cmake --install $SCRIPT_DIR/build

# Install the systemd service.
cp $SCRIPT_DIR/RollingRaspberry.service /etc/systemd/system/RollingRaspberry.service

# Reload the daemon.
systemctl daemon-reload

# Enable the systemd service.
while true; do
  read -p 'Enable and start the systemd service? ' yn
  case $yn in
    [Yy]* ) systemctl enable RollingRaspberry.service && systemctl start RollingRaspberry.service; break;;
    [Nn]* ) break;;
    * ) echo 'Please answer y or n';;
  esac
done
