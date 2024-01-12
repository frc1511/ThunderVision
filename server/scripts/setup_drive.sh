#!/bin/bash

# Check if root.
if [ $EUID -ne 0 ]; then
  echo 'Setup Script must be run as Root'
  exit 1
fi

DEV_MOUNTED=false

if [ "$(ls -A /media/usb)" ]; then
  echo 'Device already mounted at /media/usb/'
  DEV_MOUNTED=true
fi

if [ "$(lsblk | grep 'sda1')" ]; then
  if [ "$DEV_MOUNTED" = false ]; then
    sudo mount /dev/sda1 /media/usb
    echo 'Mounted /dev/sda1 at /media/usb'
  fi

  exit 0
else
  echo 'Nothing at /dev/sda1 to mount'

  if [ "$DEV_MOUNTED" = true ]; then
    sudo umount /media/usb
    echo 'Unmounted device at /media/usb'
  fi

  exit 1
fi

