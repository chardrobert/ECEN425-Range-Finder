/*

*/
#include <SoftwareSerial.h> // setup the LCD
SoftwareSerial mySerial(3,2);
byte precision = 3;
char floatBuffer[7]; 
char printBuffer[15];
unsigned long previousMillis = 0;        
// constants won't change :
const long interval = 100;
///////////////////////////////////////////////////////////////
void setup() {
  pinMode(11, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  mySerial.begin(9600);
  // wait for lcd to warm up
  delay(500);
  // writing the default writing onto board
  mySerial.write(254); // cursor to beginning of first line
  mySerial.write(128);
  mySerial.write("Dist(m):        ");
  mySerial.write("Sensor:         ");
}
//setup all of our placeholders for math
int dist;
char diststring[8], sensstring[9];
// averaging arrays
int irarr[8];
int usarr[8];
unsigned long count=0;
// output
float out=0;

/////////////////////////////////////// the loop routine runs over and over again forever amen:
void loop() {
  //reset the sums for the running averages
  int irsum=0;
  int ussum=0;
  // drive buffer high to switch FET gate on to drive LEDs
  digitalWrite(11, HIGH);
  
  // start our mill timers
  unsigned long currentMillis = millis();
  /////////////////////// introduce delayless delay so LCD doesn't freak out
  if (currentMillis - previousMillis >= interval) {
    // so we always reset the thingy
    previousMillis = currentMillis;
    //reading the IR phototransistor
    int sensorValue = analogRead(A1);
    //chuck the latest incoming value into the the slot of the oldest data
    irarr[count%8]=sensorValue;
    // iterate and sum
    for(int i=0;i<8;i++){
      irsum=irsum+irarr[i];
    }
    // bit-shift by 3 is divide by 8
    int irav=irsum>>3;
    ///////// STILL NEED TO LINEARISE IR DATA
    
    ////////////////////////////////same stuff but for the US sensor
    usarr[count%8]=sensorValue;
    for(int i=0;i<8;i++){
      ussum=ussum+usarr[i];
    }
    int usav=ussum>>3;
   ///////////////////// if the distance is less than 25cm then throw IR distance to LCD
    if (irav>25){
      out=float(irav)/100.00;
      
      dtostrf(out, precision+3, precision, floatBuffer);
      char sens[3]="IR";
      sprintf(printBuffer,"Dist(m): %s",floatBuffer); // create strings from the numbers
      sprintf(sensstring,sens);
    }
    //else Ultrasensor
    else{
      out=float(usav)/100.00;
      dtostrf(out, precision+3, precision, floatBuffer);
      char sens[3]="US";
      sprintf(printBuffer,"Dist(m):  %s",floatBuffer); // create strings from the numbers
      sprintf(sensstring,sens);
    }
    //write it to the lcd now
    mySerial.write(254); 
    mySerial.write(128);
  
    mySerial.write(printBuffer); 
  
    mySerial.write(254); 
    mySerial.write(199);
  
    mySerial.write(sensstring);
  
    // print out the value you read to the computer monitor
    Serial.println(printBuffer);
    // increment the count to change to next array position
    count++;
  } 
  delayMicroseconds(1300);        // for US sensor
}
