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

if [ -f $SWAPFILE ]; then
  echo "$SWAPFILE already exists! Run 'reset_swap.sh' to remove it."
  exit 1
fi

echo "Creating partition at $SWAPFILE"

sudo dd if=/dev/zero of=$SWAPFILE bs=1024 count=1048576

echo "Granting root permissions to $SWAPFILE"

sudo chmod 600 $SWAPFILE

echo "Creating swap area at $SWAPFILE"

sudo mkswap $SWAPFILE

echo "Activating swap area at $SWAPFILE"

sudo swapon $SWAPFILE

echo -e "\033[1m\033[32m*** Successfully Activated 1G Swap Area at $SWAPFILE ***\033[0m"

echo -e "\033[1m\033[31mMake sure to execute the 'reset_swap.sh' script when finished using the swap area.\033[0m"
