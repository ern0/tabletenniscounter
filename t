#!/bin/bash
clear

util/copyfunct.py src/ttc.cpp test/ttc_selectIdleBeep.cpp selectIdleBeep
g++ -Itest test/test.cpp -o bin/`util/platform.sh`/test
bin/`util/platform.sh`/test
