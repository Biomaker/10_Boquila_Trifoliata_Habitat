#include <Time.h>
#include <TimeLib.h>
#include <DHT.h>
#include <rgb_lcd.h>
#include <Wire.h>
rgb_lcd lcd;

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

//Set parameters for day night
float tday=25;
float tnight=15;

//Start time
int t0=18;
    

void setup() 
{
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

    
    delay(1000);
}

void loop() 
{
    float h1 = dht1.readHumidity();
    float t1 = dht1.readTemperature();
    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();

    float del1;
    float del2;

    //get hour of the day
    int tod=(second()+t0)%24;
    
    printToLCD(h1, h2, t1, t2, tod);

    // If night
    if (tod<12) {
      //switch off LED arrays
      digitalWrite(led11,HIGH);
      //calculate difference between desired temperature and measured one (night)
      del1=tnight-t1;
      del2=tnight-t2;
      Serial.print(del1);
      Serial.print(del2);
    }
    else if (tod>=12) {
      //switch on LED arrays
      digitalWrite(led11,LOW);
      //calculate difference between desired temperature and measured one (day)
      del1=tday-t1;
      del2=tday-t2;
    }
    //Set Peltier elements to heating or cooling
    //chamber1
    if (del1<-1){
      //Cooling
      Serial.print("foo");
      digitalWrite(pel11,HIGH);
      digitalWrite(pel12,LOW);
      lcd.setCursor(14, 0);
      lcd.print("C");
    }
    else if (del1>1){
      //Heating
      digitalWrite(pel11,LOW);
      digitalWrite(pel12,HIGH);
      lcd.setCursor(14, 0);
      lcd.print("H");
    }
    else{  
      //Nothing
      digitalWrite(pel11,LOW);
      digitalWrite(pel12,LOW);
      lcd.setCursor(14, 0);
      lcd.print("-");
    }
    //chamber2
    if (del2<-1){
      //Cooling
      digitalWrite(pel21,HIGH);
      digitalWrite(pel22,LOW);
      lcd.setCursor(15, 0);
      lcd.print("C");
    }
    else if (del2>1){
      //Heating
      digitalWrite(pel21,LOW);
      digitalWrite(pel22,HIGH);
      lcd.setCursor(15, 0);
      lcd.print("H");
    }
    else{
      //Nothing
      digitalWrite(pel21,LOW);
      digitalWrite(pel22,LOW);
      lcd.setCursor(15, 0);
      lcd.print("-");
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

