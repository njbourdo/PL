#!/bin/bash

set -e  # exit on error

if ! dpkg -l | grep libcmocka-dev &>/dev/null; then
  echo "CMocka not found or dpkg not installed. Attempting to install CMocka..."
  
  if [[ "$(uname)" == "Linux" ]]; then
    if which apt-get &>/dev/null; then
      sudo apt-get update
      sudo apt-get install -y libcmocka-dev
    else
      echo "Unsupported Linux package manager. Please install CMocka manually."
      exit 1
    fi
  else
    echo "Unsupported OS. Please install CMocka manually."
    exit 1
  fi
else
  echo "CMocka is already installed."
fi
