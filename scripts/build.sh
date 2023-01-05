#!/bin/bash

PROJECT_DIR=$(realpath $(dirname $BASH_SOURCE))/..

cmake $PROJECT_DIR -B$PROJECT_DIR/build -DCMAKE_BUILD_TYPE=Release

cmake --build $PROJECT_DIR/build

