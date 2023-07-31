/* 
 * The purpose of this is to scan for bluetooth device addresses and display them on an OLED screen.
 * A target address can be specified, and it will be pinned at the top of the screen along with time since last seen.
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <ArduinoSort.h>
#include <SimpleMap.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define TARGET_ADDRESS "c1:83:1a:c4:1b:09"


// An associative array of device names and RSSI
SimpleMap<String, String> *devices = new SimpleMap<String, String>([](String &a, String &b) -> int {
	if (a == b) return 0;      // a and b are equal
	else if (a > b) return 1;  // a is bigger than b
	else return -1;            // a is smaller than b
});

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void print_to_line(String text);
// Prints less important devices to the blue part of the screen
// void print_top_8(void);
// void print_device_info(String address, String rssi);

// Prints target device address and RSSI at the the top (yellow portion) of the screen (time will be added by update_display())
void print_to_yellow(String address, String rssi);

// Prints (and scrolls) other devices and their RSSI to the blue portion of the screen
void print_to_blue(String address, String rssi);

// Updates the display with the combination of the yellow and blue buffers
void update_display(void);

BLEScan* pBLEScan;

int i;
String display_buffer_yellow = "\n";
String display_buffer_blue = "";
String display_buffer = "";

int deviceCount = 0;
unsigned long last_seen_time = 0;  // This  is the last time the target device was seen, updated by the callback function
String last_seen_rssi = "-999";  // This is the last seen RSSI of the target device, updated by the callback function

bool seen = false;  // Keeps track of whether the target device has been seen or not




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function definitions
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      char chBuffer[128];
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());

      // Update the device map entry for this device.
      // Replace the address with the name, if there is one.
      if (advertisedDevice.haveName()) {
          devices->put(advertisedDevice.getName().c_str(), String(advertisedDevice.getRSSI()));
      }
      else {
        devices->put(advertisedDevice.getAddress().toString().c_str(), String(advertisedDevice.getRSSI()));
      }
      
      

      if (strcmp(advertisedDevice.getAddress().toString().c_str(), TARGET_ADDRESS) == 0) 
      { 
        Serial.print ("Found target device: ");
        Serial.println (advertisedDevice.getAddress().toString().c_str());

        // Reset the yellow portion of the display
        display_buffer_yellow = "";
        // Add the device info to the first line
        display_buffer_yellow = String(advertisedDevice.getAddress().toString().c_str()) + " " + String(advertisedDevice.getRSSI()) + "\n";
        last_seen_time = millis();
        seen = true;        
      }     
      
      else 
      {
        // Try to print the name of the device instead of the address, if there is one.
        if (advertisedDevice.haveName()) 
        {
          print_to_blue (String(advertisedDevice.getName().c_str()), String(advertisedDevice.getRSSI()));
          // devices->put(advertisedDevice.getName().c_str(), String(advertisedDevice.getRSSI()));
        }
        else {        
          print_to_blue (String(advertisedDevice.getAddress().toString().c_str()), String(advertisedDevice.getRSSI()));
        }
      }

      update_display();
      
    }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Setup.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{

  Serial.begin(115200);
  Serial.println("Scanning...");

  // WiFi.mode(WIFI_STA);

  // Initialize the graphics library.
  
  // u8g2.begin();
  // u8g2.setFont(u8g2_font_6x10_tf);
  // u8g2.setFontRefHeightExtendedText();
  // u8g2.setDrawColor(1);
  // u8g2.setFontPosTop();
  // u8g2.setFontDirection(0);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);


  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main loop.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  loop()
{
    // Obtain network count.
    
    // int nNetworkCount = WiFi.scanNetworks();
    BLEScanResults foundDevices = pBLEScan->start(5); //5 second scan duration

    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Serial.println("Scan done!");
  
    // Display devices.
    
    if(foundDevices.getCount() == 0) 
    {
        // No networks found.
        
        
        // print_to_line("0 devices found.");
    } 
    else 
    {
        // Networks found.
        
        char    chBuffer[128];
        char    chEncryption[64];
        char    chRSSI[64];
        char    chNAME[64];
        char    chADDR[64];

        int intnNetwork;
        int nNetwork;
      
        // Display network count.
        
        // sprintf(chBuffer, "%d devices found:", foundDevices.getCount());
        // u8g2.drawStr(0, 0, chBuffer);
        // print_to_line(String(foundDevices.getCount()) + " devices found:");

        // Display the networks.
        Serial.print("Devices found: ");
        Serial.println(foundDevices.getCount());
        Serial.println("Scan done!");
    }

  // This is run in the callback function so there is no delay
  // print_top_8();

  // Delay.
  
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000); //wait for 5 seconds before starting a new scan

  deviceCount = 0;
}



// This scrolls everything in the screen buffer up one line (eliminating the first line)
// Then adds the given text to the bottom previously 
// Then dumps the display buffer to the screen
void print_to_blue(String address, String rssi)
{
  
  int max_lines = 6; // Number of lines that can fit before we start chopping off the top
  
  // Count the number of lines in the display buffer
  int num_lines = 0;
  for (int i = 0; i < display_buffer_blue.length(); i++)
  {
    if (display_buffer_blue.charAt(i) == '\n')
    {
      num_lines++;
    }
  }
  
  // if the number of lines in the buffer is max_lines, chop off the first line
  if (num_lines >= max_lines)
  {
    int index = display_buffer_blue.indexOf('\n');
    if (index >= 0)
    {
      display_buffer_blue = display_buffer_blue.substring(index + 1);
    }
  }

  // Add the new text to the display buffer at bottom
  display_buffer_blue += address + " " + rssi + "\n";

  // Serial.println("Display buffer after: " + display_buffer);
}




void print_device_info(String address, String rssi)
{
  Serial.println("Device: " + address + " " + rssi);

  if (address == TARGET_ADDRESS)  
  { 
    // Reset the yellow portion of the display
    display_buffer_yellow = "";
    // Add the device info to the first line
    display_buffer_yellow = address + " " + rssi + "\n";
    last_seen_time = millis();
    last_seen_rssi = rssi;
    seen = true;
  }
}

void update_display(void) 
{
  String last_seen = "\n";

  // Compute the target last seen time 
  if (seen) {
    long target_last_seen_time = millis() - last_seen_time;
    last_seen = String(target_last_seen_time/1000) + "s ago\n";
  }

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font


  // Clear the display
  display.clearDisplay();

  // Write the buffer to the screen
  display.setCursor(0, 0);
  display.print(display_buffer_yellow + last_seen + display_buffer_blue);  
  display.display();
}