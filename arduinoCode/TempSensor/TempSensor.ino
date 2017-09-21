#include <TimeLib.h>
#include <DHT.h>
#include <rgb_lcd.h>
#include <Wire.h>
rgb_lcd lcd;

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

// Define the pin to which the temphum sensor is connected.
#define DHTPIN1 A0
#define DHTPIN2 A3   
#define DHTTYPE DHT22   // DHT 22  (AM2302) pro sensor  

// setup temphum sensor
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

// define display color
const int colorR = 255;
const int colorG = 255;
const int colorB = 255;

//LED array pins
int led11=8;
int led12=9;
int led13=10;
int led21=11;
int led22=12;
int led23=13;

//peltier pins
int pel11=6;
int pel12=7;

int pel21=4;
int pel22=5;

void setup() 
{
  
    //start serial console
    Serial.begin(9600);
    setSyncProvider( requestSync);  //set function to call when sync required
    Serial.println("Waiting for sync message");
    
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);

    //LED array pin
    pinMode(led11, OUTPUT);

    //peltier elements pin
    pinMode(pel11, OUTPUT);

    //start temphum sensor
    dht1.begin();
    dht2.begin();

    //Set parameters for day night
    float tday=25;
    float tnight=15;
    
    delay(1000);
}

void loop() 
{
    float h1 = dht1.readHumidity();
    float t1 = dht1.readTemperature();
    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();

    //Sync to connected PC system time if possible
    if (Serial.available()) {
      processSyncMessage();
    }
    //get hour of the day
    int tod=hour();
    
    printToLCD(h1, h2, t1, t2, tod);

    // If night
    if (tod<12) {
      //switch off LED arrays
      digitalWrite(led11,HIGH);
      
      //calculate difference between desired temperature and measured one
      float del1=tnight-t1;
      float del2=tnight-t2;

      //chamber1
      if (del1<-1){
        //Cooling
        digitalWrite(pel11,HIGH);
        digitalWrite(pel12,LOW);
        lcd.setCursor(15, 1);
        lcd.print("C");
      }
      elseif (del1>1){
        //Heating
        digitalWrite(pel11,LOW);
        digitalWrite(pel12,HIGH);
        lcd.setCursor(15, 1);
        lcd.print("H");
      }
      else{
        //Nothing
        digitalWrite(pel11,LOW);
        digitalWrite(pel12,LOW);
        lcd.setCursor(15, 1);
        lcd.print("-");
      }
      
      //chamber2
      if (del2<-1){
        //Cooling
        digitalWrite(pel21,HIGH);
        digitalWrite(pel22,LOW);
        lcd.setCursor(16, 1);
        lcd.print("C");
      }
      elseif (del2>1){
        //Heating
        digitalWrite(pel21,LOW);
        digitalWrite(pel22,HIGH);
        lcd.setCursor(16, 1);
        lcd.print("H");
      }
      else{
        //Nothing
        digitalWrite(pel21,LOW);
        digitalWrite(pel22,LOW);
        lcd.setCursor(16, 1);
        lcd.print("-");
      }
      
    }
    else {
      digitalWrite(8,LOW);
    }
    delay(1000);
}

void printToLCD(float h1, float h2, float t1, float t2, float tod)
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 0);
    // print the number of seconds since reset:
    lcd.print("T1=");
    lcd.print(round(t1));
    lcd.print("C");
    lcd.print(" H1=");
    lcd.print(round(h1));
    lcd.print("%");

    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print("T2=");
    lcd.print(round(t2));
    lcd.print("C");
    lcd.print(" H2=");
    lcd.print(round(h2));
    lcd.print("%");

    lcd.print(" ");
    lcd.print(tod);
    lcd.print("h");
}



void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
