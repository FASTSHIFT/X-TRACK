# LIS3MDL library for Arduino

Version: 1.0.0<br>
Release date: 2016 January 19<br>
[www.pololu.com](https://www.pololu.com/)

## Summary

This is a library for the Arduino IDE that helps interface with ST's [LIS3MDL 3-axis magnetometer](https://www.pololu.com/product/2737). The library makes it simple to configure the LIS3MDL and read the raw magnetometer data through I&sup2;C.

## Supported platforms

This library is designed to work with the Arduino IDE versions 1.6.x or later; we have not tested it with earlier versions.  This library should support any Arduino-compatible board, including the [Pololu A-Star 32U4 controllers](https://www.pololu.com/category/149/a-star-programmable-controllers).

## Getting started

### Hardware

An [LIS3MDL carrier](https://www.pololu.com/product/2737) can be purchased from Pololu's website.  Before continuing, careful reading of the [product page](https://www.pololu.com/product/2737) as well as the LIS3MDL datasheet and application note is recommended.

Make the following connections between the Arduino and the LIS3MDL board:

#### 5V Arduino boards

(including Arduino Uno, Leonardo, Mega; Pololu A-Star 32U4)

    Arduino   LIS3MDL board
    -------   -------------
         5V - VIN
        GND - GND
        SDA - SDA
        SCL - SCL

#### 3.3V Arduino boards

(including Arduino Due)

    Arduino   LIS3MDL board
    -------   -------------
        3V3 - VIN
        GND - GND
        SDA - SDA
        SCL - SCL

### Software

If you are using version 1.6.2 or later of the [Arduino software (IDE)](http://www.arduino.cc/en/Main/Software), you can use the Library Manager to install this library:

1. In the Arduino IDE, open the "Sketch" menu, select "Include Library", then "Manage Libraries...".
2. Search for "LIS3MDL".
3. Click the LIS3MDL entry in the list.
4. Click "Install".

If this does not work, you can manually install the library:

1. Download the [latest release archive from GitHub](https://github.com/pololu/lis3mdl-arduino/releases) and decompress it.
2. Rename the folder "lis3mdl-arduino-master" to "LIS3MDL".
3. Move the "LIS3MDL" folder into the "libraries" directory inside your Arduino sketchbook directory.  You can view your sketchbook location by opening the "File" menu and selecting "Preferences" in the Arduino IDE.  If there is not already a "libraries" folder in that location, you should make the folder yourself.
4. After installing the library, restart the Arduino IDE.

## Examples

Several example sketches are available that show how to use the library. You can access them from the Arduino IDE by opening the "File" menu, selecting "Examples", and then selecting "LIS3MDL". If you cannot find these examples, the library was probably installed incorrectly and you should retry the installation instructions above.

## Library reference

* `vector<int16_t> m`<br>
  The last values read from the magnetometer.

* `uint8_t last_status`<br>
  The status of the last I&sup2;C write transmission. See the [`Wire.endTransmission()` documentation](http://arduino.cc/en/Reference/WireEndTransmission) for return values.

* `LIS3MDL(void)`<br>
  Constructor.

* `bool init(deviceType device, sa0State sa0)`<br>
  Initializes the library with the device being used (`device_LIS3MDL` or `device_auto`) and the state of the SA1 pin (`sa1_low`, `sa1_high`, or `sa1_auto`), which determines the second-least significant bit of the I&sup2;C slave address. Constants for these arguments are defined in LIS3MDL.h. Both of these arguments are optional; if they are not specified, the library will try to automatically detect the device address. A boolean is returned indicating whether the type of device was successfully determined (if necessary).

  *Note:* Automatic detection of the device type currently does not work with the Arduino Due because of issues with its Wire library. To work around this, specify the device and SA1 state manually (e.g. `init(LIS3MDL::device_LIS3MDL, LIS3MDL::sa1_high)`).

* `void getDeviceType(void)`<br>
  Returns the device type specified to or detected by `init()`.

* `void enableDefault(void)`<br>
  Turns on the magnetometer and enables a consistent set of default settings.

  This function will reset the magnetometer to &plusmn;4&nbsp;gauss full scale. See the comments in LIS3MDL.cpp for a full explanation of the settings.

* `void writeReg(uint8_t reg, uint8_t value)`<br>
  Writes a sensor register with the given value.

  Register address constants are defined by the regAddr enumeration type in LIS3MDL.h.<br>
  Example use: `mag.writeReg(LIS3MDL::CTRL_REG1, 0x70);`

* `uint8_t readReg(uint8_t reg)`<br>
  Reads a sensor register and returns the value read.

* `void read(void)`<br>
  Takes a reading from the magnetometer and stores the values in the vector `m`. Conversion of the readings to units of gauss depends on the magnetometer's selected gain (full scale setting).

* `void setTimeout(uint16_t timeout)`<br>
  Sets a timeout period in milliseconds after which the read functions will abort if the sensor is not ready. A value of 0 disables the timeout.

* `uint16_t getTimeout(void)`<br>
  Returns the current timeout period setting.

* `bool timeoutOccurred(void)`<br>
  Indicates whether a read timeout has occurred since the last call to `timeoutOccurred()`.

## Version history

* 1.0.0 (2016 Jan 19): Original release.
