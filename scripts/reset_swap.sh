#!/bin/bash

# Check root.
if [ $EUID -ne 0 ]; then
  echo 'Script must be run as Root'
  exit 1
fi

# Check machine.
if [ ! $(uname -o) = 'GNU/Linux' ] || [ ! $(uname -m) = 'armv7l' ]; then
  echo 'What are you doing?? This script should only be on a Raspberry Pi in dire circumstances!'
  exit 1
fi

SWAPFILE=/swapfile

if [ ! -f $SWAPFILE ]; then
  echo "$SWAPFILE doesn't exist. No swap to reset!"
  exit 1
fi

echo "Deactivating swap area at $SWAPFILE"

sudo swapoff $SWAPFILE

echo "Deleting $SWAPFILE"

sudo rm $SWAPFILE

echo -e "\033[1m\033[32m*** Successfully Removed Swap Area at $SWAPFILE ***\033[0m"
