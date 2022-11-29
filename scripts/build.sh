#!/bin/bash

# Check machine.
if [ ! $(uname -o) = 'GNU/Linux' ] || [ ! $(uname -m) = 'armv7l' ]; then
  echo 'What are you doing?? Rolling Raspberry can only be built on a Raspberry Pi'
  exit 1
fi

PROJECT_DIR=$(realpath $(dirname $BASH_SOURCE))/..

cmake $PROJECT_DIR -B$PROJECT_DIR/build -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

cmake --build $PROJECT_DIR/build

