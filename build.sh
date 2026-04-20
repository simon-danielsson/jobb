#!/usr/bin/env bash

cd ~/dev/c/jobb
mkdir build 2>/dev/null
cc -o ./nob/nob ./nob/nob.c
./nob/nob
./build/main
