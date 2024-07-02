//#include <SPI.h>
//#include <LoRa.h>
//#include <LoRaWanPacket.h>
//#include <HardwareSerial.h>
//#include <ArduinoJson.h>
//
//// rxtx
//#define RXD0 3
//#define TXD0 1
//
//// Tbeam Config
//const int csPin = 18;
//const int resetPin = 23;
//const int irqPin = 26;
//
//byte messageData[30];
//
//// data serial 
//String dataIn;
//String dt[10];
//int i;
//
//boolean parsing = false;
//
//// Antares Config
//const char *devAddr = "84b115d9";
//const char *nwkSKey = "4b54c9ad922ceb2fd6ebe8167fdca15a";
//const char *appSKey = "c7e75e077fd5a68aa61c67eef45d09f9";
//
//struct LoRa_config
//{
//  long Frequency;
//  int SpreadingFactor;
//  long SignalBandwidth;
//  int CodingRate4;
//  bool enableCrc;
//  bool invertIQ;
//  int SyncWord;
//  int PreambleLength;
//};
//
//static LoRa_config txLoRa = {922000000, 15, 125000, 5, true, false, 0x34, 8};
//
//uint64_t customerId;
//float userTotalUsage;
//float userTotalCost;
//char currentDate[11];
//char currentTime[8];
//
//void LoRa_setConfig(struct LoRa_config config)
//{
//  LoRa.setFrequency(config.Frequency);
//  LoRa.setSpreadingFactor(config.SpreadingFactor);
//  LoRa.setSignalBandwidth(config.SignalBandwidth);
//  LoRa.setCodingRate4(config.CodingRate4);
//  if (config.enableCrc)
//    LoRa.enableCrc();
//  else
//    LoRa.disableCrc();
//  if (config.invertIQ)
//    LoRa.enableInvertIQ();
//  else
//    LoRa.disableInvertIQ();
//  LoRa.setSyncWord(config.SyncWord);
//  LoRa.setPreambleLength(config.PreambleLength);
//}
//
//void LoRa_TxMode()
//{
//  LoRa_setConfig(txLoRa);
//  LoRa.idle();
//}
//
//void setup()
//{
//  Serial.begin(9600, SERIAL_8N1, RXD0, TXD0);
//  dataIn = "";
//  //  Serial.println("Serial Txd is on pin: " + String(TX));
//  //  Serial.println("Serial Rxd is on pin: " + String(RX));
//  // Wait for serial connection with timeout
//  unsigned long startMillis = millis();
//  while (!Serial && millis() - startMillis < 5000) {
//      // Wait for up to 5 seconds
//  }
//
//  LoRaWanPacket.personalize(devAddr, nwkSKey, appSKey);
//
//  LoRa.setPins(csPin, resetPin, irqPin);
//
//  if (!LoRa.begin(txLoRa.Frequency))
//  {
//    Serial.println("LoRa init failed. Check your connections.");
//    while (true);
//  }
//
//  Serial.println("LoRa init succeeded.");
//  Serial.println();
//}
//
//DynamicJsonDocument jsonDoc(256);
//
//void nerimadata() {
//
//  if (Serial.available() > 0) {
//    char inChar = (char)Serial.read();
//    if (inChar == '*') {
//      parsing = true;
//      dataIn = ""; // Bersihkan data yang ada sebelumnya
//    } else if (inChar == '\n') {
//      parsing = false;
//    } else if (parsing) {
//      dataIn += inChar;
//    }
//  }
//
//  if (!parsing && dataIn.length() > 0) {
//    parsingData();
//    dataIn = ""; // Bersihkan data setelah diproses
//  }
//
//}
//
//void parsingData() {
//
//  int commaIndex = dataIn.indexOf(',');
//  String dataPacket;
//  if (commaIndex != -1) {
//    String idPelanggan = dataIn.substring(0, commaIndex);
//    String penggunaanStr = dataIn.substring(commaIndex + 1);
//    String penggunaan = penggunaanStr.substring(penggunaanStr.indexOf(":") + 1); // Menghapus "Penggunaan : " dari penggunaanStr
//    if (idPelanggan.equals("1") || idPelanggan.equals("2") || idPelanggan.equals("3") || idPelanggan.equals("4")) {
//      LoRa_sendMessage(idPelanggan, penggunaan);
//    
//    
////    Serial.print("ID pelanggan : ");
////    Serial.println(idPelangganInt);
////    Serial.print("Penggunaan : ");
////    Serial.println(penggunaan);
//
//    dataPacket += "ID pelanggan : ";
//    dataPacket += idPelanggan;
//    dataPacket += ", Penggunaan : ";
//    dataPacket += penggunaanStr;
//    
//  }
//  if (!dataPacket.isEmpty()) {
//    Serial.println(dataPacket); // Print data packet
////    LoRa_sendMessage(dataPacket); // Send data packet via LoRa
//  }
//  
//}
//
//void loop()
//{
//  nerimadata();
//  
////  parsingData();
//  if (runEvery(10000))
//  {
//    LoRa_sendMessage();
////    Serial.println("Send Message!");
//  }
//}
//
//
//
//boolean runEvery(unsigned long interval)
//{
//  static unsigned long previousMillis = 0;
//  unsigned long currentMillis = millis();
//  if (currentMillis - previousMillis >= interval)
//  {
//    previousMillis = currentMillis;
//    return true;
//  }
//  return false;
//}

#include <SPI.h>
#include <LoRa.h>
#include <LoRaWanPacket.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

// rxtx
#define RXD0 3
#define TXD0 1

// Tbeam Config
const int csPin = 18;
const int resetPin = 23;
const int irqPin = 26;

byte messageData[30];

// data serial 
String dataIn;
String dt[10];
int i;

boolean parsing = false;

// Antares Config
const char *devAddr = "43f421da";
const char *nwkSKey = "8230a76cba587eac814474b27ecdab9b";
const char *appSKey = "f3ec4f0980c17e4ce8b329e200d99237";

struct LoRa_config
{
  long Frequency;
  int SpreadingFactor;
  long SignalBandwidth;
  int CodingRate4;
  bool enableCrc;
  bool invertIQ;
  int SyncWord;
  int PreambleLength;
};

static LoRa_config txLoRa = {922000000, 15, 125000, 5, true, false, 0x34, 8};

uint64_t customerId;
float userTotalUsage;
float userTotalCost;
char currentDate[11];
char currentTime[8];

void LoRa_setConfig(struct LoRa_config config)
{
  LoRa.setFrequency(config.Frequency);
  LoRa.setSpreadingFactor(config.SpreadingFactor);
  LoRa.setSignalBandwidth(config.SignalBandwidth);
  LoRa.setCodingRate4(config.CodingRate4);
  if (config.enableCrc)
    LoRa.enableCrc();
  else
    LoRa.disableCrc();
  if (config.invertIQ)
    LoRa.enableInvertIQ();
  else
    LoRa.disableInvertIQ();
  LoRa.setSyncWord(config.SyncWord);
  LoRa.setPreambleLength(config.PreambleLength);
}

void LoRa_TxMode()
{
  LoRa_setConfig(txLoRa);
  LoRa.idle();
}

void setup()
{
  Serial.begin(9600, SERIAL_8N1, RXD0, TXD0);
  dataIn = "";
  //  Serial.println("Serial Txd is on pin: " + String(TX));
  //  Serial.println("Serial Rxd is on pin: " + String(RX));
  // Wait for serial connection with timeout
  unsigned long startMillis = millis();
  while (!Serial && millis() - startMillis < 5000) {
      // Wait for up to 5 seconds
  }

  LoRaWanPacket.personalize(devAddr, nwkSKey, appSKey);

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(txLoRa.Frequency))
  {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
}
DynamicJsonDocument jsonDoc(256);

void parsingData() {

  int commaIndex = dataIn.indexOf(',');
  String dataPacket;
  if (commaIndex != -1) {
    LoRa_sendMessage2(dataIn);
     Serial.println(dataIn);
    
//    String idPelanggan = dataIn.substring(0, commaIndex);   
//    String penggunaanStr = dataIn.substring(commaIndex + 1);
//    String penggunaan = penggunaanStr.substring(penggunaanStr.indexOf(":") + 1); // Menghapus "Penggunaan : " dari penggunaanStr
//    if (idPelanggan.equals("1") || idPelanggan.equals("2") || idPelanggan.equals("3") || idPelanggan.equals("4")) {
//      LoRa_sendMessage(idPelanggan, penggunaan);
//      dataPacket += "ID pelanggan : ";
//      dataPacket += idPelanggan;
//      dataPacket += ", Penggunaan : ";
//      dataPacket += penggunaanStr;
//    }
  }
  
    //  if (!dataPacket.isEmpty()) {
    //  Serial.println(dataPacket); // Print data packet
    //  LoRa_sendMessage(dataPacket); // Send data packet via LoRa
    // }
  
}

void nerimadata() {

  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    if (inChar == '*') {
      parsing = true;
      dataIn = ""; // Bersihkan data yang ada sebelumnya
    } else if (inChar == '\n') {
      parsing = false;
    } else if (parsing) {
      dataIn += inChar;
    }
  }

  if (!parsing && dataIn.length() > 0) {
    parsingData();
    dataIn = ""; // Bersihkan data setelah diproses
  }

}

void loop()
{
  nerimadata();
    // parsingData();
  if (runEvery(10000))
  {
    // LoRa_sendMessage();
    // Serial.println("Send Message!");
  }
}

String idPelangganArray[10]; 
String penggunaanArray[10]; 
int dataSize = 0;
void LoraCollect(String idPelanggan, String penggunaan)
{
 LoRa_TxMode(); 
  if (dataSize < 10) { 
    idPelangganArray[dataSize] = idPelanggan; 
    penggunaanArray[dataSize] = penggunaan; 
    dataSize++; 
  }
  else {
    sendLoRaData();
  }
}

void sendLoRaData() 
{
  for (int i = 0; i < dataSize; i++) 
  {
    LoRa_sendMessage(idPelangganArray[i], penggunaanArray[i]);
  }
  dataSize = 0;
}

  LoRaWanPacket.clear();
  LoRaWanPacket.print(idPelanggan);
  LoRaWanPacket.print(",");
  LoRaWanPacket.print(penggunaan);
  LoRaWanPacket.print(",");
  
  if (LoRaWanPacket.encode()) 
  {
    LoRa.beginPacket();
    LoRa.write(LoRaWanPacket.buffer(), LoRaWanPacket.length());
    LoRa.endPacket();
  }
}

void LoRa_sendMessage2(String datas)
{
  LoRa_TxMode();
  LoRaWanPacket.clear();

//  LoRaWanPacket.print(idPelanggan);
//  LoRaWanPacket.print(",");
//  LoRaWanPacket.print(penggunaan);
//  LoRaWanPacket.print(",");

  LoRaWanPacket.print(datas);
  if (LoRaWanPacket.encode()) 
  {
    LoRa.beginPacket();
    LoRa.write(LoRaWanPacket.buffer(), LoRaWanPacket.length());
    LoRa.endPacket();
  }
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
