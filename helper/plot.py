# This simple module plots the given file integers
import sys
import matplotlib.pyplot as plt
from ctypes import *

class Histogram(Structure):
    _fields_ = [('t', c_int)]

def readIntegers(filename):
	result = []
	with open(filename, 'rb') as file:
		x = Histogram()
		while file.readinto(x) == sizeof(x):
			result.append(x.t)
	return result

# check arguments
if len(sys.argv) != 2:
	print "Usage: " + sys.argv[0] + " <input-file>"
	exit()

# read integers from file to array
numbers = readIntegers(sys.argv[1])

# plot the numbers
plt.plot(numbers)
plt.xlabel('Color In Grayscale')
plt.ylabel('Contain Number')
plt.show()
