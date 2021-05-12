# This simple module converts image format
import sys
import Image

if len(sys.argv) != 3:
	print("Usage: " + sys.argv[0] + " <input-img> <output-img>")
	exit()

inputImg = sys.argv[1]
outputImg = sys.argv[2]

print("Converting " + inputImg + " to " + outputImg)
img = Image.open(inputImg)
img.save(outputImg)
