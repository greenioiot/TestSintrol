 
#include "REG_SINTROL_203.h"
#include <ModbusMaster.h>
#include <HardwareSerial.h>
 
 


HardwareSerial modbus(2);
 
 
 
ModbusMaster node;
void t2CallgetProbe();
void t1CallcheckAlert();
void t3CallsendViaNBIOT();
//TASK
//Task t2(1000, TASK_FOREVER, &t2CallgetProbe);



//const long interval = 1000;  //millisecond
//unsigned long previousMillis = 0;

boolean initBase = true;
//const long low = 2;
const long medium = 3;
const long high = 5;

long baseline = 0;
long baselineXmedium = 0;
long baselineXhigh = 0;
 
 
long resultDust = 0;

const int bufferSize = 10;
long circular[bufferSize];
int count = 0;
double avgDust = 0;
double avg = 0;
 



void t2CallgetProbe() {     // Update read all data
  //    delay(1000);                              // เคลียบัสว่าง
  // testing
  for (char i = 0; i < Total_of_Reg ; i++) {
    DATA_METER [i] = Read_Meter_float(ID_meter, Reg_addr[i]);//แสกนหลายตัวตามค่า ID_METER_ALL=X
  }
}

void addReading(long reading) {
  //  Serial.print("add:");
  //  Serial.println(reading);
  circular[count] = reading;
  Serial.print("count:");

  count++;
  Serial.println(count);
 


}

long average() {
  long sum = 0;
  for (int i = 0; i < bufferSize; i++) {
    sum += circular[i];

  }
  int divider = count;
  count = 0;
  memset(circular, 0, sizeof(10));
  long avg = (long) (sum / divider);
  if (initBase) {
      baseline = avg;
      baselineXmedium = baseline * medium;
      baselineXhigh = baseline * high; 
      initBase = false;
  }
  return avg;
}


float Read_Meter_float(char addr , uint16_t  REG) {
  unsigned int i = 0;
  uint32_t j, result;
  uint16_t data[2];
  uint32_t value = 0;
  node.begin(255, modbus);
  result = node.readInputRegisters (REG, 2); ///< Modbus function 0x04 Read Input Registers
  delay(400);
  if (result == node.ku8MBSuccess) {
    for (j = 0; j < 2; j++)
    {
      data[j] = node.getResponseBuffer(j);
      //      Serial.println(data[j]);
    }
    resultDust = getResult(data[1], data[0]);
    addReading(resultDust);
    Serial.println(resultDust);
    return i;
  } else {
    Serial.print("Connec modbus fail. REG >>> "); Serial.println(REG, HEX); // Debug
//    delay(1000);
    return 0;
  }
}



 

//**************************************************************************************************************
void setup() {

  Serial.begin(115200);
 
  modbus.begin(38400, SERIAL_8N1, 16, 17);
  Serial.println(F("Starting... Sintrol Monitor"));
 

  // communicate with Modbus slave ID 1 over Serial (port 2)
  node.begin(ID_meter, modbus);
 
 
}

void loop() {

   t2CallgetProbe();
   delay(3000);
  t3CallsendViaNBIOT();
  delay(2000);
}

void t3CallsendViaNBIOT () {
 

  String json = "";
//  json.concat("{\"Tn\":\"");
////  json.concat(deviceToken);
////  json.concat("\",\"dust\":");
  json.concat(avgDust);
//  json.concat(",\"rssi\":");
//  json.concat(meta.rssi);
//  json.concat(",\"csq\":");
//  json.concat(meta.csq);
//  json.concat(",\"base\":");
//  json.concat(baseline);
//  json.concat(",\"1\":");
//  json.concat(baselineXmedium);
//  json.concat(",\"2\":");
//  json.concat(baselineXhigh);
//  json.concat(",\"wifi\":");
//  json.concat(RSSIWiFi);

//  json.concat("}");
  Serial.println(json);
//  SerialBT.println(json);
 
//  Serial.print("rssi:");
//  Serial.println(meta.rssi);
//  SerialBT.print("rssi:");
//  SerialBT.println(meta.rssi);
}
 
String decToHex(int decValue) {

  String hexString = String(decValue, HEX);
  return hexString;
}


unsigned int hexToDec(String hexString) {

  unsigned int decValue = 0;
  int nextInt;

  for (int i = 0; i < hexString.length(); i++) {

    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);

    decValue = (decValue * 16) + nextInt;
  }

  return decValue;
}

long getResult( unsigned int x_high, unsigned int x_low)
{
  String hex2 = "";
  hex2.concat(decToHex(x_low));
  hex2.concat(decToHex(x_high));
  //  Serial.print("hex:");  Serial.println(hex2);  Serial.print("dec:");
  //  Serial.println(hexToDec(hex2));                                                               //rightmost 8 bits
  long dust = hexToDec(hex2);
  return dust;
}
