#!/bin/bash
clear

BIN=bin/`platform.sh`
time make -j4 $BIN/ttc
