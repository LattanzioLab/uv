/*Program code for lab UV preference logger for small-bodied animals. The code here was used for
 * Conley & Lattanzio's study on UV and temperature regulation in lizards. 
 * Components used: Arduino Pro-Mini 3.3V [1], Adafruit Analog GUVAS12-SD Breakout board [6], 
 * Adafruit microSD Breakout board [1], mini speaker [1]. GUVAS12-SD boards are interfaced to the 
 * Pro Mini using magnet wires to minimize mass influences on animal behavior. A battery pack (e.g.,
 * 3-AA or 1 9V) provides power to the system. 
 * We used a 6-lane arena for estimating UV preferences with this system; your input pin numbers,
 * and sensor numbers, will vary depending on your lab set-up and available pins on your microcontroller. 
 * We affixed sensors to the dorsal surface of each lizard using surgical tape, allowing us to track real-time
 * UVI exposure values of each lizard in our arena. We set our device to record UVI values at 30-second intervals
 * over a 1-hour period (to match recordings of body temperature in another part of our experiment). 
 */
#include "SdFat.h"
  SdFat SD;
  File myFile;

  //variables used to convert analog readings to UVI values
  float dat1,dat2,dat3,dat4,dat5,dat6;
  float volt1,volt2,volt3,volt4,volt5,volt6;
  float uvi1,uvi2,uvi3,uvi4,uvi5,uvi6;
  float vref;

  //analog inputs of each lane's sensor (=pin). Our arena had 6 total lanes.
  int pin1 = A6;
  int pin2 = A4;
  int pin3 = A7;
  int pin4 = A0;
  int pin5 = A2;
  int pin6 = A1;

  //reference pin for actual battery output, which may change over time.
  int refPin = A3;
  
void setup () {
  SD.begin(10); //invokes microSD reader, which is connected to pin 10 on Pro Mini. Change this value to your pin if needed!

  //calling the file uvdata.csv; naming is arbitrary. 
  myFile = SD.open("uvdata.csv", FILE_WRITE);

  /*generating a header for the file prior to data collection. I am detail-oriented, so I opted to save raw voltages (in mV)
   * and converted UVI values. It does not meaningfully affect the time to read + write to the card, so worth it despite 
   * their redundancies. 
   * First column is time in milliseconds, then the voltages for each lane, then the converted UVI values for each lane. 
   */
  if (myFile) {
      myFile.print("Time");
      myFile.print(',');
      myFile.print("L1_raw");
      myFile.print(',');
      myFile.print("L2_raw");
      myFile.print(',');
      myFile.print("L3_raw");
      myFile.print(',');
      myFile.print("L4_raw");
      myFile.print(',');
      myFile.print("L5_raw");
      myFile.print(',');
      myFile.print("L6_raw");
      myFile.print(',');
      myFile.print("L1_UVI");
      myFile.print(',');
      myFile.print("L2_UVI");
      myFile.print(',');
      myFile.print("L3_UVI");
      myFile.print(',');
      myFile.print("L4_UVI");
      myFile.print(',');
      myFile.print("L5_UVI");
      myFile.print(',');
      myFile.print("L6_UVI");
      myFile.println();
      myFile.close();
      } else {}
}

void loop () {
  //slight delay for sensors; can be reduced if desired (impact on measurement times is minimal). 
  delay(100);
  
  /*loop to record UV values at 30 second intervals for a 1-hour period (=120 measurements). 
   *Changing the delay changes the recording frequency, and changing the max value for i adjusts
   *the total number of measurements to be taken. 
    */
  for (int i = 0;  i <=119; i++) {

    //obtaining raw counts from each pin. 
  vref = analogRead(refPin);
  dat1 = analogRead(pin1);
  dat2 = analogRead(pin2);
  dat3 = analogRead(pin3);
  dat4 = analogRead(pin4);
  dat5 = analogRead(pin5);
  dat6 = analogRead(pin6);

  /*converting raw counds from each pin into voltages (in mV).
   * Here, we use vref, which is interfaced to VCC directly. We do this because
   * battery voltage output changes over time as the battery runs. This allows us
   * to calculate voltages using the actual output of VCC, rather than assuming
   * it remains at 1023 (the max counts input for an analog pin). IF you keep your 
   * Arduino plugged in to a constant power source, change each to: voltX = datX/1023*3.3,
   * where 'X' is the pin (and in our case, lane the sensor is used for our arena) of interest. 
   */
  volt1 = dat1/vref*3.3; 
  volt2 = dat2/vref*3.3; 
  volt3 = dat3/vref*3.3; 
  volt4 = dat4/vref*3.3; 
  volt5 = dat5/vref*3.3; 
  volt6 = dat6/vref*3.3; 

  /*Coversion factor (=slope) of mV to UVI will vary by light source. We used
   * Arcadia T5 UVB lights in this study. Change the conversion to the value
   * you calibrate for your study. 
   */
  uvi1 = 31.341*volt1; 
  uvi2 = 31.341*volt2; 
  uvi3 = 31.341*volt3; 
  uvi4 = 31.341*volt4;
  uvi5 = 31.341*volt5;
  uvi6 = 31.341*volt6;
  
  /*time to reopen the uvdata.csv file, and enter the data into the table. 
   * The numbers after each value refer to the decimal places to round to. We
   * recommend at least 4 decimals for these estimates. 
   */
  myFile = SD.open("uvdata.csv", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
      myFile.print(millis());
      myFile.print(',');
      myFile.print(volt1,8);
      myFile.print(',');
      myFile.print(volt2,8);
      myFile.print(',');
      myFile.print(volt3,8);
      myFile.print(',');
      myFile.print(volt4,8);
      myFile.print(',');
      myFile.print(volt5,8);
      myFile.print(',');
      myFile.print(volt6,8);
      myFile.print(',');
      myFile.print(uvi1,6);
      myFile.print(',');
      myFile.print(uvi2,6);
      myFile.print(',');
      myFile.print(uvi3,6);
      myFile.print(',');
      myFile.print(uvi4,6);
      myFile.print(',');
      myFile.print(uvi5,6);
      myFile.print(',');
      myFile.print(uvi6,6);
      myFile.println();
      myFile.close();
      } else {}

      //delay to establish recording intervals. We used 30 second (=30000 ms) intervals.
      delay(30000);
  }

  //Code to run once for loop has ended (AKA, all the data are recorded!). Delays are arbitrary.
  //User should power down system and retrieve data once tones sound. 
  delay(1000);

  //Plays end tones on speaker (speaker is connected to pin 6). 
  tone(6,523,300); 
  delay(500);
  tone(6,523,300);
  delay(500);
  tone(6,523,1000); 

  //Repeats tones after another 2-min delay. 
  delay(120000); 
  tone(6,523,300); 
  delay(500);
  tone(6,523,300);
  delay(500);
  tone(6,523,300); 
  delay(500);
  tone(6,523,300);

  //Repeats 2 minute delay
  delay(120000); //repeat 2-min delay. 
  
  //User should clear microSD card prior to re-inserting it for a new trial. Enjoy! 
}
