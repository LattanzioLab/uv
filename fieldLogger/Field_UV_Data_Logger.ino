/*Program code for Field UV Data Logger for recording long-term UV exposures in natural habitats for the study of behavioral UV regulation in wildlife. 
*Components used: Arduino Pro-Mini 3.3V [1], Adafruit Analog GUVAS12-SD Breakout board [1], Adafruit microSD Breakout board [1], Adafruit DS3231 Precision RTC module [1], 
*and a SparkFun Nano Power Timer [1]. A lithium polymer battery (I used 1000mAh capacity) provides power to the system. Lithium batteries are preferred for ease of connection to the power timer. 
*Recording intervals are set on the Nano Power Timer itself vial onboard resistor switches. Connection wiring is described in the images included in this repository folder. 
Additional information on the logger system and assembly is forthcoming (including housing components, calibration, and output examples).
*/

/*A note on library usage: Make sure to rely on the following library-version combinations; I have had issue with the microSD board and RTC module not working correctly with newer libraries. 
*The logger program works using the following library versions:
*SdFat version 1.1.2
*RTClib version 1.2.2
*/

//Load required libraries
#include <SPI.h>
#include "SdFat.h"
#include "RTClib.h"
#include <Wire.h>
SdFat SD;
RTC_DS3231 rtc;

//Set pins
const int chipSelect = 10; 
int donePin = 8; 
int sensorPin = A2;
int refPin = A3;

//Specify voltage and UV Index calculations below as float
float vref, countdat, voltdat, uvi;

//initiate a file for the microSD card
File myFile;

void setup() {
  SD.begin(10); //this might allow me to invoke SD directly by specifying the CS pin (when >1 SPI attached)
 // Run this section the first time you power it all up, then re-install program with this section commented out to get the RTC set up to your date/time! 
 // if (rtc.lostPower()) {  
 //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 //  }

  //set the donePin as output
  pinMode(donePin, OUTPUT); 
}

void loop() {
    delay(100);
    DateTime now = rtc.now();
    delay(100);

    //obtain reference voltage from refPin (pin is connected directly to VCC so provides an analog reading of the voltage)
    vref = analogRead(refPin);
    
    //read the UV sensor
    countdat = analogRead(sensorPin);

    //convert the sensor reading to voltage
    voltdat = countdat/vref*3.3;
    uvi = 8.8529*voltdat; //This is a calibration to southeastern Arizona sunlight; replace with your own calibration results! See Readme for more infomation!

    //create and open a data file on the microSD card
    myFile = SD.open("data.csv", FILE_WRITE); //I recommend a unique name for each data logger you create [e.g., data_01.csv, data_02.csv]

/* If the file opened okay, write to it! Here I just made a column for all date/time information, the tempreature of the RTC module [to ensure it is within expected range], 
* and provide the raw sensor reading (in counts), the converted voltage (in V), and the converted UV Index value (UVI has no units).  
*/
   if (myFile) {
     myFile.print(now.year(), DEC);
     myFile.print(',');
     myFile.print(now.month(), DEC);
     myFile.print(',');
     myFile.print(now.day(), DEC);
     myFile.print(',');
     myFile.print(now.hour(), DEC);
     myFile.print(',');
     myFile.print(now.minute(), DEC);
     myFile.print(',');
     myFile.print(now.second(), DEC);
     myFile.print(',');
     myFile.print(rtc.getTemperature());
     myFile.print(',');
     myFile.print(countdat,5);
     myFile.print(',');
     myFile.print(voltdat,8);
     myFile.print(',');
     myFile.print(uvi,8);
     myFile.println();
     // close the file:
     myFile.close();
   } else {
   }
   delay(1000); 
/* Send the signal to the Timer that data collection is done for that interval; this turns off power to the microcontroller until the next interval */
   digitalWrite(donePin, LOW); 
   digitalWrite(donePin, HIGH); 
   }
