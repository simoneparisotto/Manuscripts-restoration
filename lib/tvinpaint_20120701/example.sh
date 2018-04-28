#! /bin/bash
# TV inpainting example BASH shell script

# Echo shell commands
set -v

# Generate an inpainting domain D.bmp from random text
./randmask text mountain.bmp D.bmp

# Set the image to gray within the inpainting domain to create masked.bmp
./applymask mountain.bmp D.bmp masked.bmp

# Inpaint masked.bmp using the inpainting domain D.bmp with lambda = 100.
# The result is saved to inpainted.bmp.
./tvinpaint D.bmp 100 masked.bmp inpainted.bmp

