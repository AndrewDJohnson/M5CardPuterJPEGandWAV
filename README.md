Created by modifying M5 Stack examples.

Andrew Johnson - 24 Aug 2024

Simple and daft thing which can be adapted for more sensible use.

Reads keypresses from Cardputer keyboard and then loads 2 different JPEGS and displays and then plays various WAV files.
Assumes the 2 jpegs and WAV files are stored in the "/StarTrek/" folder of the CardPuter's SD card/

The order of "include" files here is important - the SD.H must come before the  "M5CardPuter.h" or the JPEG loading function won't work

There seems to be a delay after the jpeg loads or before the sound is played... Not sure what causes that!

I will add the WAV and 2 JPG files to the repository.
