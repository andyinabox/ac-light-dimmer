/*

 AC Light Dimmer v.2 - Inmojo 
 AC Voltage dimmer with Zero cross detection
 
 Author: Charith Fernanado http://www.inmojo.com charith@inmojo.com 
 License: Released under the Creative Commons Attribution Share-Alike 3.0 License. 
 http://creativecommons.org/licenses/by-sa/3.0
 Target:  Arduino
 
 Attach the Zero cross pin of the module to Arduino External Interrupt pin
 Select the correct Interrupt # from the below table
 
 Pin    |  Interrrupt # | Arduino Platform
 ---------------------------------------
 2      |  0            |  All
 3      |  1            |  All
 18     |  5            |  Arduino Mega Only
 19     |  4            |  Arduino Mega Only
 20     |  3            |  Arduino Mega Only
 21     |  2            |  Arduino Mega Only
 
 Please select your utility power frequency from frq variable.
 
 */
#define  fullOn    10
#define  fullOff   127
#define  FQ_50      1 // in case of 50Hz
#define  FQ_60      0 // in case of 50Hz
#define  VER       "2.0"

int inbyte;
int AC_LOAD = 3;    // Output to Opto Triac pin
int dimming = fullOff;  // Dimming level (0-128)  0 = ON, 128 = OFF

boolean frq = FQ_50;     // change the frequency here. 
boolean countUp = false;
boolean simulate = true; 

void setup()
{
  pinMode(AC_LOAD, OUTPUT);	      // Set the AC Load as output
  attachInterrupt(0, zero_crosss_int, RISING);  // Choose the zero cross interrupt # from the table above
  Serial.begin(115200);
}

void zero_crosss_int()  // function to be fired at the zero crossing to dim the light
{
  // Firing angle calculation
  // 50Hz-> 10ms (1/2 Cycle) → (10000us - 10us) / 128 = 78 (Approx)
  // 60Hz-> 8.33ms (1/2 Cycle) → (8333us - 8.33us) / 128 = 65 (Approx)
  int dimtime = 0;
  float propTime = 0;
  if(frq){
    dimtime = (77*dimming);
    propTime = 10.0; 
  }
  else{
    dimtime = (65*dimming);
    propTime = 8.33; 
  }
  delayMicroseconds(dimtime);    // Off cycle
  digitalWrite(AC_LOAD, HIGH);   // triac firing
  delayMicroseconds(propTime);         // triac On propogation delay
  digitalWrite(AC_LOAD, LOW);    // triac Off

}

void loop()
{
  if(simulate){
    if(countUp)
      dimming++; 
    else
      dimming--;
  }

  if(dimming < fullOn)
    countUp = true;
  else if (dimming > fullOff)
    countUp = false;  

  delay(20);

  if (Serial.available() > 0)
    _serial_int();

}



void displayMenu() {
  Serial.println(" -------- InMojo Digital Dimmer v.2 | inmojo.com -------- ");
  Serial.println("");
  Serial.println("[m] Menu");
  Serial.println("[d] Simulate Diming");
  Serial.println("[1] Turn ON Light");
  Serial.println("[0] Turn OFF Light");
  Serial.println("[q] Query Dimming Amount");
  Serial.println("[v] Version");
  Serial.println("");
}



void _serial_int(){
  while (Serial.available() > 0) {
    inbyte = Serial.read();

    switch (inbyte) {
    case 'd':
      attachInterrupt(0, zero_crosss_int, RISING);
      if(simulate)
        simulate = false;
      else
        simulate = true;
      break;
    case 'm':
      displayMenu(); 
      break;
    case '1':
      simulate = false;
      detachInterrupt(0); 
      digitalWrite(AC_LOAD, HIGH); 
      break;      
    case '0':
      simulate = false;
      detachInterrupt(0); 
      digitalWrite(AC_LOAD, LOW); 
      break;   
    case 'q':
      Serial.print("Dimming: "); 
      Serial.println(dimming);
      break;        
    case 'v':
      Serial.println(VER);
      break;
    }    

  }
}







