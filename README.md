# SMArTSerial
The SimpleMaker Arduino Technical Serial monitoring software

Usable with an Arduino (or any 5v compatible board) connected to 
1. An SD card slot
2. An Adafruit RGB i2c LCD
3. A TTL to RS-232 converter

SMArTSerial can:
1. Select a 5 character filename (A-Z, -), and automatically append a zero-padded file number based upon pre-existing files
2. Log 9600-baud data collected off an RS-232 port to a file stored on an SD card, and output the data onto the LCD for inspection
3. Prevent accidental stops with a 2-stage exit routine, which continues to log until completely stopped.

For logging data from scales, meters, and other serial devices, SMArTSerial can be used in setups that cost less than $100.

For a full guide on how to use, see the Guide.txt file.
