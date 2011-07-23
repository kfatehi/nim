#!/bin/sh
gcc nim.c -lncurses -o nim
if [ "$?" = "0" ]; then 
  ./nim
fi