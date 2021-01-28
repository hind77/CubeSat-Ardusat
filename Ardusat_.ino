#include <DHT.h>
#include <DHT_U.h>
#include <DS1302.h>
#include <MQ135.h>
#include <SPI.h>
#include <SD.h>
#include <dht.h>
#include <Wire.h>  


DS1302:  RST pin    -> Arduino Digital 2
          DATA pin   -> Arduino Digital 3
          CLK pin  -> Arduino Digital 5
DS1302 rtc(2, 3, 5);
int greenPin = 8;
//dht DHT;
String UVs = "0"; 
int UVn;
const int mq135Pin = 0;
const int AOUTpin=0;
int limit;
int value;
int gas_sensor = A0; //Sensor pin
float R0 = 5.35; //Sensor Resistance in fresh air from previous code
float m = -0.318; //Slope
float b = 1.133; //Y-Intercept
int pinCS = 10; // Pin 53 on Arduino mega
File myFile;
#define co2Zero     55                        //calibrated CO2 0 level
MQ135 gasSensor = MQ135(mq135Pin);  // Initialise l'objet MQ135 sur le Pin spécifié
#define DHTPIN 8     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value


void setup()
{
  Serial.begin(9600);  // Used to type in characters  
  float rzero = gasSensor.getRZero();
  //Serial.print("R0: ");
  //Serial.println(rzero);  // Valeur à reporter ligne 27 du fichier mq135.h après 48h de préchauffage
    
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  
  // Setup LCD to 16x2 characters
  // lcd.begin(16, 2);


  // The following lines can be commented out to use the values already stored in the DS1302
  rtc.setDOW(WEDNESDAY);        // Set Day-of-Week to FRIDAY
  rtc.setTime(17, 3, 40);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(14, 2, 2018);   // Set the date to August 6th, 2010
  pinMode(gas_sensor, INPUT);
  pinMode(pinCS, OUTPUT);
  
  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
}


void loop()
{

 UVs = readSensor();
 UVn =val();
 
int co2now[10];                               //int array for co2 readings
int co2raw = 0;                               //int for raw value of co2
int co2comp = 0;                              //int for compensated co2 
int co2ppm = 0;                               //int for calculated ppm
int zzz = 0;                                  //int for averaging
int grafX = 0;                                //int for x value of graph


  
for (int x = 0;x<10;x++){                   //samplpe co2 10x over 2 seconds
    co2now[x]=analogRead(A1);
    delay(200);
  }

for (int x = 0;x<10;x++){                     //add samples together
    zzz=zzz + co2now[x];
    
  }
  co2raw = zzz/10;                            //divide samples by 10
  co2comp = co2raw - co2Zero;                 //get compensated value
  co2ppm = map(co2comp,0,1023,400,5000);      //map value for atmospheric levels
  int ck = DHT.read11(greenPin);
  float ppm2 = gasSensor.getPPM();

  // methan gas sensor

  int sensorValue;
  int meth;
  sensor_volt; //Define variable for sensor voltage
  float RS_gas; //Define variable for sensor resistance
  float ratio; //Define variable for ratio
  float sensorValue = analogRead(gas_sensor); //Read analog values of sensor
  sensor_volt = sensorValue * (5.0 / 1023.0); //Convert analog values to voltage
  RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0; //Get value of RS in a gas
  ratio = RS_gas / R0;   // Get ratio RS_gas/RS_air

  double ppm_log = (log10(ratio) - b) / m; //Get ppm value in linear scale according to the the ratio value
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale
  double ppb = 1000*ppm;


  sensorValue = analogRead(0);       // read analog input pin 0
  meth = map(sensorValue, 0, 1023, 0, 5000);

   hum = dht.readHumidity();
    temp= dht.readTemperature();  
  // file work
  Serial.print(rtc.getDOWStr(FORMAT_SHORT));
  Serial.print(",");
  Serial.print(rtc.getDateStr());
  Serial.print(",");
  Serial.print(rtc.getTimeStr());
  Serial.print(",");
  Serial.print(hum);
  Serial.print(",");
  Serial.print(temp);
  Serial.print(",");
  Serial.print(meth);
  Serial.print(",");
  Serial.print(co2ppm);
  Serial.print(",");
  Serial.print(UVn);
  Serial.print(",");
  Serial.println(UVs);


  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {   
  myFile.print(rtc.getDOWStr(FORMAT_SHORT));
  myFile.print(",");
  myFile.print(rtc.getDateStr());
  myFile.print(",");
  myFile.print(rtc.getTimeStr());
  myFile.print(",");
  myFile.print(hum);
  myFile.print(",");
  myFile.print(temp);
  myFile.print(",");
  myFile.print(meth);
  myFile.print(",");
  myFile.print(co2ppm);
  myFile.print(",");
  myFile.print(UVn);
  myFile.print(",");
  myFile.println(UVs);


    myFile.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  }
  delay(3000);
 
}

int val(){
    int sensorValue = 0;
    sensorValue = analogRead(2);                        //connect UV sensor to Analog 0   
  int voltage = (sensorValue * (5.0 / 1023.0))*1000;  //Voltage in miliVolts
  return voltage;
}

String readSensor()
{
  String UVIndex = "0";
  int sensorValue = 0;
  
  
  sensorValue = analogRead(2);                        //connect UV sensor to Analog 0   
  int voltage = (sensorValue * (5.0 / 1023.0))*1000;  //Voltage in miliVolts
  
  if(voltage<50)
  {
    UVIndex = "low";
  }else if (voltage>50 && voltage<=227)
  {
    UVIndex = "low";
  }else if (voltage>227 && voltage<=318)
  {
    UVIndex = "low";
  }
  else if (voltage>318 && voltage<=408)
  {
    UVIndex = "low";
  }else if (voltage>408 && voltage<=503)
  {
    UVIndex = "medium";
  }
  else if (voltage>503 && voltage<=606)
  {
    UVIndex = "medium";
  }else if (voltage>606 && voltage<=696)
  {
    UVIndex = "medium";
  }else if (voltage>696 && voltage<=795)
  {
    UVIndex = "High";
  }else if (voltage>795 && voltage<=881)
  {
    UVIndex = "High";
  }
  else if (voltage>881 && voltage<=976)
  {
    UVIndex = "Very High";
  }
  else if (voltage>976 && voltage<=1079)
  {
    UVIndex = "Very High";
  }
  else if (voltage>1079 && voltage<=1170)
  {
    UVIndex = "Very High";
  }else if (voltage>1170)
  {
    UVIndex = "Extremmely High";
  }
  return UVIndex;
}
