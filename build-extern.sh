#!/bin/bash

SCRIPT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" > /dev/null && pwd)"

pushd $SCRIPT_ROOT
mkdir -p .extern/build
cd .extern/build

CMAKE_GENERATOR='Unix Makefiles'

if command -v ninja >/dev/null 2>&1; then
    CMAKE_GENERATOR='Ninja'
fi

cmake -G "$CMAKE_GENERATOR" -DCMAKE_INSTALL_PREFIX=../install ../../cmake/extern
cmake --build .
popd
