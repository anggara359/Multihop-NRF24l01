//Anggara Wahyu 
//Program untuk LORAWAN 

//-------------------------------------------------------------------------------
#include <SPI.h>
#include <LoRa.h>
#include <LoRaWanPacket.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

//rxtx
#define RXD0 3
#define TXD0 1

//Tbeam Config
const int csPin = 18;
const int resetPin = 23;
const int irqPin = 26;

byte messageData[50];

//Antares Config ABP
const char *devAddr = "959524bd";
const char *nwkSKey = "0a8dc5ff5fc7def792c05cfedf586666"; //e8095173e07be5740000000000000000
const char *appSKey = "531961b419b961bc3ab59b3df3c2cc5a";

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
//  Serial.begin(115200);
  Serial.begin(9600, SERIAL_8N1, RXD0, TXD0);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  
  while (!Serial);

  LoRaWanPacket.personalize(devAddr, nwkSKey, appSKey);

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(txLoRa.Frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();

  LoRa_sendMessage();

}

String Message;
uint64_t newCustomerId = 0; // ID Pelanggan baru, tentukan nilai awal yang sesuai
float newUserTotalUsage = 0.0;   // Total Penggunaan baru dalam meter kubik, tentukan nilai awal yang sesuai
float newUserTotalCost = 0.0;    // Biaya baru dalam rupiah, tentukan nilai awal yang sesuai
String newCurrentDate = "";      // Tanggal baru, tentukan nilai awal yang sesuai
String newCurrentTime = "";      // Waktu baru, tentukan nilai awal yang sesuai

DynamicJsonDocument jsonDoc(256);

void nerimadata(){
   if (Serial.available() >= sizeof(messageData)) {
    // Baca data dari Serial
    Serial.readBytes(messageData, sizeof(messageData));

    // Proses data yang diterima sesuai kebutuhan
    processReceivedData();
  }
}

void processReceivedData(){
  uint64_t customerId;
  float userTotalUsage;
  float userTotalCost;
  char currentDate[11];
  char currentTime[8];

  // Mendekode data yang diterima dari byte array
  memcpy(&customerId, &messageData[0], sizeof(customerId));
  memcpy(&userTotalUsage, &messageData[8], sizeof(userTotalUsage));
  memcpy(&userTotalCost, &messageData[12], sizeof(userTotalCost));
  memcpy(currentDate, &messageData[16], sizeof(currentDate));
  memcpy(currentTime, &messageData[27], sizeof(currentTime));

  // Menampilkan data yang diterima
  Serial.print("ID Pelanggan : ");
  Serial.println(static_cast<unsigned long>(customerId));

  Serial.print("Total Penggunaan Air : ");
  Serial.print(userTotalUsage, 2); // Tampilkan 2 digit desimal
  Serial.println(" m³");
  Serial.print("Biaya : Rp");
  Serial.println(userTotalCost);
  Serial.print("Tanggal : ");
  Serial.println(currentDate);
  Serial.print("Waktu : ");
  Serial.println(currentTime);
}
       
void loop() {

  nerimadata();
 
  if (runEvery(10000)) {
    LoRa_sendMessage();
    Serial.println("Send Message!");
  }
}

void LoRa_sendMessage()
{
  LoRa_TxMode();
  LoRaWanPacket.clear();

//   StaticJsonDocument<256> jsonDoc;

//   Menambahkan data ke objek JSON
  jsonDoc["ID Pelanggan Baru"] = customerId;
  jsonDoc["Total Penggunaan Air Baru"] = userTotalUsage;
  jsonDoc["Biaya Baru"] = userTotalCost;
  jsonDoc["Tanggal Baru"] = currentDate;
  jsonDoc["Waktu Baru"] = currentTime;

 //  Serializasikan objek JSON ke dalam String
  String jsonMessage;
  serializeJson(jsonDoc, jsonMessage);

  //  Kirim data JSON melalui LoRaWAN
  LoRaWanPacket.print(jsonMessage);
  
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
