#!/usr/bin/python3
"""
Converts .pal palettes to txt files formatted to work in VirtuanNES
"""
import itertools
import struct
from sys import argv

lengthPal = 191
output = open(argv[1] + '.txt', 'w')

def convert(pal):
	pos = 0
	while pos < lengthPal:
		output.write('{')
		output.write(str(pal[pos]))
		output.write(',')
		output.write(str(pal[pos+1]))
		output.write(',')
		output.write(str(pal[pos+2]))
		if pos+3 >= lengthPal:
			output.write('}')
		else:
			output.write('},' + '\n')
		pos += 3

palette = open(argv[1], 'rb').read()		
convert(palette)