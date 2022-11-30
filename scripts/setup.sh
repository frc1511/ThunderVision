#!/bin/bash

# Check if root.
if [ $EUID -ne 0 ]; then
  echo 'Setup Script must be run as Root'
  exit 1
fi

# Check machine.
if [ ! $(uname -o) = 'GNU/Linux' ] || [ ! $(uname -m) = 'armv7l' ]; then
  echo 'What are you doing?? Rolling Raspberry can only be installed on a Raspberry Pi'
  exit 1
fi

# Install Required Tools
sudo apt-get update && sudo apt-get install -y --no-install-recommends \
  make \
  cmake \
  libopencv-dev \
  libeigen3-dev \
  libapriltag-dev

pip3 install jinja2
pip3 install numpy

