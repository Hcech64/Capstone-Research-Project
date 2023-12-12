// SPDX-FileCopyrightText: 2011 Limor Fried/ladyada for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Thermistor Example #3 from the Adafruit Learning System guide on Thermistors 
// https://learn.adafruit.com/thermistor/overview by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and consider buying parts from Adafruit

// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 100000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 50
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 4092
// the value of the 'other' resistor
#define SERIESRESISTOR 100000

//display stuff
//#include "SevSeg.h"
//SevSeg sevseg;

bool overheated = false;
bool TuningMode = true;
int fuck =0;
bool toTemp = false;
bool pinState = true;
int runawayCount = 0;
double p, I, d;
double PID = 0;

double Kp = 6;
double Kd = 0.1;
double Ki = 0.1;

int target = 215;
double PrevTemp = 20;
//double List(PrevTemp) = new ArrayList();
double PrevVal;

int samples[NUMSAMPLES];

void setup(void) {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  pinMode (12, OUTPUT);

}

void loop(void) {
  uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(5);
  }
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;

  //Serial.print("Average analog reading "); 
  //Serial.println(average);
  
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  //Serial.print("Thermistor resistance "); 
  //Serial.println(average);
  
  float read;
  read = average / THERMISTORNOMINAL;     // (R/Ro)
  read = log(read);                  // ln(R/Ro)
  read /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  read += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  read = 1.0 / read;                 // Invert
  read -= 273.15;                         // convert absolute temp to C
  
 // Serial.print("Temperature ");
if( (TuningMode)){//wait for data available while in tuning mode to Adjust K values
  if(Serial.available() > 0){
    String input = Serial.readString();  //read until timeout
  //input.trim();     // remove any \r \n whitespace at the end of the String
    if (input.substring(0,1).equalsIgnoreCase("d")) {
      Kd = input.substring(2, input.length()).toFloat();
    } 
    
    if (input.substring(0,1).equalsIgnoreCase("p")) {
      Kp = input.substring(2, input.length()).toFloat();
    } 

    if (input.substring(0,1).equalsIgnoreCase("i")) {
      Ki = input.substring(2, input.length()).toFloat();
    }
    
  }

  //Serial.print("Kd = ");
 // Serial.print(",");
  Serial.print(Kd);
  Serial.print(",");
 // Serial.print("Ki = ");
  //Serial.print(",");
  Serial.print(Ki);
  Serial.print(",");
  //Serial.print("Kp = ");
  //Serial.print(",");
  Serial.print(Kp);
  Serial.print(",");
  
  
}

fuck +=1;

  Serial.print(read);
  Serial.print(",");
  Serial.print(PID);
  Serial.print(",");
  Serial.print(target);
  Serial.print(",");
  Serial.println(p);
 /* Serial.print("runawayCount: ");
  Serial.println(runawayCount);
  Serial.print("relay on: ");
  Serial.println(pinState);
  Serial.println(toTemp);*/
 // Serial.println(" *C");

//difference between target and actua temp
p = target - read;

// take the integral of read (i+=p) while also clipping it to eliminate integral windup
if((PID > 255) && (I+p < 0)){
  I += p;
}
else if((PID < 0) && (I+p > 0)){
  I += p;
}
else if((PID >= 0) && (PID <= 255)){
  I += p;
}


//determine derivative of read by using the previous temp values for and AROC
/*
PrevTemp.add(read);
del PrevTemp[0];
*/

d = (read - PrevVal);
PrevVal = read;

PID = (Kp * p)+(Kd * d)+(Ki * I);
//digitalWrite (12, HIGH);
if (!overheated){
  if (PID >= 255){
    analogWrite (12, 255);
    //Serial.println("high");
  }
  else if (PID < 0){
    analogWrite (12, 0);
    //Serial.println("low");
  }
  else{
    analogWrite (12, int(PID));
    //Serial.println("pwm");
  }
}

if(read>250){
  analogWrite(12, 0);
  overheated = true;
  Serial.println("Your bitchass just got thermal runaway'd lmao");
}
if((PID >= 255) && (d < -10) && (p > 0)){
  runawayCount += 1;
}
else{runawayCount = 0;}
if (runawayCount > 20){
  analogWrite (12, 0);
  overheated = true;
  Serial.println("runawayCount expired motherfucker");
}

  delay(20);
}