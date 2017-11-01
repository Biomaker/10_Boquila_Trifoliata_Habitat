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
int led1=A1;
//int led2=??;
//peltier pins
int pel11=8;
int pel12=11;
int pel1drive=9;

int pel21=12;
int pel22=13;
int pel2drive=10;

//Set parameters for day night
float tday=15;
float tnight=15;

//Start time
int t0=17;

//Peldrive speed
int speed=50;
    

void setup() 
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);

    //peltier elements pin
    pinMode(pel11, OUTPUT);
    pinMode(pel12, OUTPUT);
    pinMode(pel1drive, OUTPUT);
    pinMode(pel21, OUTPUT);
    pinMode(pel22, OUTPUT);
    pinMode(pel2drive, OUTPUT);
    //setup LED pins
    pinMode(led1,OUTPUT);
//    pinMode(led2,OUTPUT);

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
    int tod=(hour()+t0)%24;
    
    printToLCD(h1, h2, t1, t2, tod);

    // If night
    if (tod<6 || tod>18) {
      //switch off LED arrays
      digitalWrite(led1,HIGH);
//      digitalWrite(led2,HIGH);
      //calculate difference between desired temperature and measured one (night)
      del1=tnight-t1;
      del2=tnight-t2;
      Serial.print(del1);
      Serial.print(del2);
    }
    else if (tod>=6 && tod<=18) {
      //switch on LED arrays
      digitalWrite(led1,LOW);
//      digitalWrite(led2,HIGH);  
      //calculate difference between desired temperature and measured one (day)
      del1=tday-t1;
      del2=tday-t2;
    }
    //Set Peltier elements to heating or cooling
    //chamber1
    if (del1<-1){
      //Cooling
      analogWrite(pel1drive,speed);
      digitalWrite(pel11,HIGH);
      digitalWrite(pel12,LOW);
      lcd.setCursor(14, 0);
      lcd.print("C");
    }
    else if (del1>1){
      //Heating
      analogWrite(pel1drive,speed);
      digitalWrite(pel11,LOW);
      digitalWrite(pel12,HIGH);
      lcd.setCursor(14, 0);
      lcd.print("H");
    }
    else{  
      //Nothing
      analogWrite(pel1drive,0);
      digitalWrite(pel11,LOW);
      digitalWrite(pel12,LOW);
      lcd.setCursor(14, 0);
      lcd.print("-");
    }
    if (del2<-1){
      //Cooling
      analogWrite(pel2drive,speed);
      digitalWrite(pel21,HIGH);
      digitalWrite(pel22,LOW);
      lcd.setCursor(15, 0);
      lcd.print("C");
    }
    else if (del2>1){
      //Heating
      analogWrite(pel2drive,speed);
      digitalWrite(pel21,LOW);
      digitalWrite(pel22,HIGH);
      lcd.setCursor(15, 0);
      lcd.print("H");
    }
    else{  
      //Nothing
      analogWrite(pel2drive,0);
      digitalWrite(pel21,LOW);
      digitalWrite(pel22,LOW);
      lcd.setCursor(15, 0);
      lcd.print("-");
    }
    delay(1000);
}

//Function to print Information on GOVE LCD
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

