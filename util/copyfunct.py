#!/usr/bin/env python3

import sys
sys.dont_write_bytecode = True
import os


class CopyFunct:

	def main(self):

		if len(sys.argv) != 4:
			print(
				os.path.basename(sys.argv[0]) + 
				" source.c test.c functionName"
			)
			quit(0)

		fi = open(sys.argv[1],"r")
		fo = open(sys.argv[2],"w")
		copy = False

		while True:
			line = fi.readline()
			if line == "": break
			line = line.replace("\n","")

			begins = False
			if sys.argv[3] in line: begins = True
			if line.count("(") != 1: begins = False
			if line.count(")") != 1: begins = False
			if line.count("{") != 1: begins = False
			if line.count("}") != 0: begins = False
			if begins: copy = True

			if copy: fo.write(line + "\n")

			ends = False
			if sys.argv[3] in line: ends = True
			if line.count("(") != 1: ends = False
			if line.count(")") != 1: ends = False
			if line.count("}") != 1: ends = False
			if line.count("{") != 0: ends = False

		fi.close()
		fo.close()

if __name__ == "__main__": (CopyFunct()).main()
