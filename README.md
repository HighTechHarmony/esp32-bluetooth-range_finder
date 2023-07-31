# esp32-bluetooth-range_finder
Track and display signal strength of bluetooth devices using an ESP32 and SSD1306 OLED display

# hardware
## This project currently uses:

* Seeed Studio Xiao ESP32C3 
* SSD1306 .96 inch i2c OLED display, 2 color (yellow/blue)
* (Optional, but recommended): Yagi or other directional 2.4 GHz antenna

## Pinout:
* SDA on SSD1306 to SDA (D4) on Xiao
* SCL on SSD1306 to SCL (D5) on Xiao
* VCC on SSD1306 to 5V on Xiao
* GND on SSD1306 to GND on Xiao

See https://wiki.seeedstudio.com/XIAO_ESP32C3_Pin_Multiplexing/ for hardware connection reference

# configuration
Replace your target address in the #define TARGET_ADDRESS near the top of the code using the following format example:

`#define TARGET_ADDRESS "c1:83:1a:c4:1b:09"`


# Function
The device begins scanning on power up and displays all devices it sees, along with their RSSI number.  RSSI is less then 0, 
so the signal is stronger as the number gets closer to 0.

Devices that are not the target device are scrolled continously on the bottom (blue) portion of the screen.  
This is mostly to give confidence that the device is working.

If the target device is seen, it is pinned to the top of the screen (yellow), and a timer starts indicating the time in seconds since it was last seen.

# Notes
* The scanning is fairly continuous, but there are 2 second break intervals. 
* The last seen timer display is only updated when (any) device has been detected (even if not the target device).

