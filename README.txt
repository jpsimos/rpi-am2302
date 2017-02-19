Raspberry Pi DHT22 (AM2302) Sensor Reader
Author Jacob Psimos ; Released 2/19/2017

ACKNOWLEDGEMENT
This program contains refactored code that was originally
written by Tony DiCola at Adafruit Industries.  He provides
a python library that reads the DHT22 sensor.  The underlying
C code that composes the python library has been used as a template
for this program's core functionality.  It has been completely refactored
so the program can be compiled directly as a C program along with my own
functionality and changes.

SEE THE BOTTOM OF THIS DOCUMENT FOR THE ORIGINAL PYTHON LIBRARY DOCUMENTATION


AM2302 / DHT22 Sensor Reader
**This program requires root privleges to interface the GPIO pins**

COMPILING
	To compile the program, simply run the `build` script like this:
	./build

USAGE:
	./am2302 [data pin] [maximum read attempts] [re-read interval secs]
	Argument 2 and 3 are optional, defaulting to 10 and 2
	
	re-read interval is the time in seconds inbetween another read attempt
	if the previous one failed.

Example Output:
sudo ./am2302 22
Temperature = 69 Humidity = 39 Failed Checksums = 1 Timeouts = 0 Iterations = 2

Where Iterations is the total number of times that a read cycle was attempted
including the successful one if there was one.

===========================================================================================

Adafruit Python DHT Sensor Library

Python library to read the DHT series of humidity and temperature sensors on a Raspberry Pi or Beaglebone Black.

Designed specifically to work with the Adafruit DHT series sensors ----> https://www.adafruit.com/products/385

Currently the library is only tested with Python 2.6/2.7.

For all platforms (Raspberry Pi and Beaglebone Black) make sure your system is able to compile Python extensions. On Raspbian or Beaglebone Black's Debian/Ubuntu image you can ensure your system is ready by executing:

sudo apt-get update
sudo apt-get install build-essential python-dev

Install the library by downloading with the download link on the right, unzipping the archive, and executing:

sudo python setup.py install

You can ommit the sudo if you use raspberry pi.

See example of usage in the examples folder.

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Tony DiCola for Adafruit Industries.

MIT license, all text above must be included in any redistribution