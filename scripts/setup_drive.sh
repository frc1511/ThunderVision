#!/bin/bash

# Check if root.
if [ $EUID -ne 0 ]; then
  echo 'Setup Script must be run as Root'
  exit 1
fi

if [ "$(ls -A /media/usb)" ]; then
  echo 'Device already mounted at /media/usb/'
  exit 0
fi


if [ "$(lsblk | grep 'sda1')" ]; then
  sudo mount /dev/sda1 /media/usb
  echo 'Mounted /dev/sda1 at /media/usb'
  exit 0
else
  echo 'Nothing at /dev/sda1 to mount'
  exit 1
fi

