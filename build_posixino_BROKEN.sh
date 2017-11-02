#!/bin/bash
clear

BIN=bin/`util/platform.sh`
time make -j4 $BIN/ttc
