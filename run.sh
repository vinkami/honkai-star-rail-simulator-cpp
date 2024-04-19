#!/bin/bash

if [ -e "Project" ]; then
  # run the executable directly if it exists
  ./Project
else
  if [ -e "Makefile" ]; then
    # run make first if the Makefile exists but the executable doesn't
    make
    ./Project
  else
    # run if cmake doesn't exist and is not installed
    if [ -z "$(command -v cmake)" ]; then
      if [ ! -e "./cmake-3.29.2-linux-x86_64/bin/cmake" ]; then
        echo "CMake is not found. Installing CMake..."
        wget -q https://github.com/Kitware/CMake/releases/download/v3.29.2/cmake-3.29.2-linux-x86_64.tar.gz -O cmake.tar.gz  # Download CMake
        tar -xvzf cmake.tar.gz  # Extract the archive
        rm cmake.tar.gz  # Remove the archive
      fi
    fi

    # run cmake, make and the executable
    ./cmake-3.29.2-linux-x86_64/bin/cmake .
    make
    ./Project
  fi
fi