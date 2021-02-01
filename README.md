Parts:
1pc Current sensor (SEN0098)
1pc Voltage divider (ST0430X5)
2pcs 5v regulator* (B06W54L3XK)
1pc Arduino Teensy 3.6
1pc LCD I2C 20x4 (B07PWWTB94)
1pc Push to make switch (CLW1078)
2pcs Resistors 2.7k

*Follow wiring diagram. Do not use a single voltage regulator 
as this will result in the current sensor failing.

Download Teensyduino to program teensy board: https://www.pjrc.com/teensy/td_download.html

Code info
-Set time and date for time of turning on logger (not compile time)
-Current sensor and voltage sensor must be calibrated using offset and
 multiplier values.
-SD card can be removed to load data to PC at anytime. However, if the hour value
 changes, a log will be missed. So it's best to remove the SD card a few minutes into
 an hour and return within the hour.
-The data log text layout is (Hour:Minute; Day/Month/Year; Volts, Watts, totalWattHours).

Calibrate Voltage Sensor
- Read the raw volt value with no current draw, this is the offset value.
- Apply a know current draw (measured from external wattmeter or charge controller).
- Divide the reading by the known current draw, this is the multiplier.

Calibrate Current Sensor
- Read the raw amp value with no current draw, this is the offset value.
- Apply a know current draw (measured from external wattmeter or charge controller).
- Divide the reading by the known current draw, this is the multiplier.
