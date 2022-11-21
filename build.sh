#!/bin/bash

# Check machine.
if [ ! $(uname -o) = 'GNU/Linux' ] || [ ! $(uname -m) = 'armv7l' ]; then
  echo 'What are you doing?? Rolling Raspberry can only be installed on a Raspberry Pi'
  exit 1
fi

SCRIPT_DIR=$(realpath $(dirname $BASH_SOURCE))

cmake $SCRIPT_DIR -B$SCRIPT_DIR/build -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

cmake --build $SCRIPT_DIR/build

