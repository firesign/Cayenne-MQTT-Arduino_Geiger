/* Counts per minute 
 * Connect the GND on Arduino to the GND on the Geiger counter.
 * Connect the 5V on Arduino to the 5V on the Geiger counter.
 * Connect the VIN on the Geiger counter to the D2 on Arduino.
 *
 *  by MBL June 2019
 */

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>


// WiFi network info
char ssid[] = "E2000";
char wifiPassword[] = "876-0643"; 

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "29064280-aa93-11e7-a9b2-a5d7f5484bce";
char password[] = "27926c04c28d1ebe246e6ce84af56096b27ddfae";
char clientID[] = "d2756c10-5fb9-11e9-9f74-8b741b61f48a";

unsigned long lastMillis = 0;

// Geiger counter variables
int counts;                         // variable for GM Tube events
unsigned long previousMillis;       // variable for time measurement
int cpm;                            // counts per minute
float microSv;                      // microSievert conversion
float multiplier;                   // microSievert multiplier for given GM tube
#define LOG_PERIOD 60000            // print counts every minute
// WeMos Pin
int gmPin = D5; //the count pulses are connected to Pin D1 on the WeMos unit


void ICACHE_RAM_ATTR impulse() {                    // called every time there is a FALLING signal on gmPin
    counts++;
    // trigger monostables for piezo and LED
    //digitalWrite(3, HIGH);
    //digitalWrite(4, HIGH);
    //digitalWrite(3, LOW);
    //digitalWrite(4, LOW);
}





void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Start program");
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);

  multiplier = 0.057; // multiplier for SBM-20 tube
    counts = 0;
    pinMode(gmPin, INPUT);          // set up to input counter signals
    //pinMode(3, OUTPUT);           // piezo clicker
    //pinMode(4, OUTPUT);           // count LED  
    attachInterrupt(digitalPinToInterrupt(gmPin), impulse, FALLING); // define external interrupt: 
    Serial.println("Start counter");
}



void loop() {
  Cayenne.loop();

  // Log CPM and microSieverts into counts variable
  unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > LOG_PERIOD) {
        previousMillis = currentMillis;
        Serial.print(counts);
        cpm = counts;
        microSv = counts * multiplier;
        Serial.print(" . . . ");
        Serial.print(microSv);
        Serial.println(" µSv/hour");
        counts = 0;
    }

  //Publish data every 60 seconds (60000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 60000) {
    lastMillis = millis();

    if (cpm != 0) {
      Cayenne.virtualWrite(0, cpm);
    }
  }
}

//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
