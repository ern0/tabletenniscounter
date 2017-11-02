#!/bin/bash

clear
pio lib install 258  # TM1637
pio run -e prod -t upload
