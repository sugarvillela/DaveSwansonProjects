# Wave Steganographer
A steganographer hides a small file within a large file

## Overview
* This program encodes a file of any type into an audio wave file
* I wrote it back in 2014.
* Challenges at the time included: researching the .wav format, reading the header portion of the file to get file attributes and doing bit-wise operations to get at the hidden data

## How it works
* A typical wave file is large, 2 or 3 bytes times 44,100 samples per second of audio.
* If you replace the least significant bit of each sample with some arbitrary 1 or 0, it has little effect on the audio quality
* You can iterate the bits of the file you want to hide and replace the LSB of the wave file with those bits.
* The only constraint is that the hidden file must be around 1/16 the size of the wave file.

## Class Diagrams etc.
* Find the project report from 2014 in the WaveSteg/parasites folder:  Project2_WriteUp.pdf
