#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Konfigurasi Alamat
RF24 radio(10, 9);
const uint64_t beforeModuleIdmodul1 = 0xF0F0F0F0E2LL;
const uint64_t beforeModuleId = 0xF0F0F0F0E3LL;
const uint64_t moduleId = 0xF0F0F0F0E4LL;
const uint16_t dev_id = 3;

//Time for Sending millis
unsigned long previousMillis = 0;
const long interval = 2000;

//Data
byte messageData[12];
byte newData2[6];
byte datagabung[18];

//Counter data yang diterima
bool dataReceived = false;
int  dataCounter = 0;
int  dataCounterNewCustomer = 0;

//Konfigurasi LED
const unsigned long ledDuration = 200;
unsigned long ledOnMillis = 0;
const int module2Pin = 7;

//Konfigurasi Flow sensor
unsigned long pulseDuration = 0;
const float pulseToCubicMeters = 0.00000247;
const int sensorPin = A0;
float totalLiters = 0;

void setup() {

  //Set up radio Frekuensi
  radio.begin();
  radio.openReadingPipe(1, beforeModuleId);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(85);
  pinMode(module2Pin, OUTPUT);
  Serial.begin(115200);

  //LCD
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.display();
  //  delay(2000);
  display.clearDisplay();
}

void lcd() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Center "MODUL 2" at the top
  int16_t x = (display.width() - 8 * 6) / 2; // Assuming 6 characters for "MODUL 2"
  int16_t y = 0;

  display.setCursor(x, y);
  display.print("MODUL 3");

  // Place "Jumlah Data Modul 1" below "MODUL 2"
  x = 10;
  y = 9;

  display.setCursor(x, y);
  display.print("Jumlah Data Modul 2 = ");
  display.println(dataCounterNewCustomer);
  display.display();
}

unsigned long lastResponseTime = 0;
bool beforeModuleId2ResponseReceived = false;

void bacanrf() {

  //Setting Listening
  radio.openReadingPipe(1, beforeModuleId); //ketika ini if (radio.available()) tidak terdeteksi pindah ke  radio.openReadingPipe(2, beforeModuleIdmodul1); // problem stuck membaca modul 1 (beforeModuleIdmodul1)
  radio.startListening();

  //Cek Radio
  if (radio.available()) {
    Serial.print("sedang baca data");
    Serial.println();
    radio.read(&messageData, sizeof(messageData));
    dataReceived = true;
    dataCounter++;
    uint16_t customerId;
    memcpy(&customerId, &messageData[6], sizeof(customerId));
    if (customerId == 2) {
      dataCounterNewCustomer ++;
      beforeModuleId2ResponseReceived = true;
      lastResponseTime = millis();
    }
//    else if (!beforeModuleId2ResponseReceived && millis() - lastResponseTime >= 2000) {
//      radio.openReadingPipe(1, beforeModuleIdmodul1);
//      radio.startListening();
//    }
  }

  //else {
  //    memset(&messageData, 0, sizeof(messageData));
  //  }

if (!beforeModuleId2ResponseReceived && millis() - lastResponseTime >= 6000) {
    beforeModuleId2ResponseReceived = false;
    radio.openReadingPipe(2, beforeModuleIdmodul1);
    radio.startListening();


}

  //radio.stopListening();

  lcd();
}

void gabungData() {
  memcpy(datagabung, &messageData, sizeof(messageData));
  // Salin data dari newData ke datagabung setelah messageData
  memcpy(datagabung + sizeof(messageData), &newData2, sizeof(newData2));
}

void kirimnrf12() {
  radio.stopListening();
  radio.openWritingPipe(moduleId);
  uint16_t cek_id;
  memcpy(&cek_id, &datagabung[12], sizeof(cek_id));
  Serial.print("Cek point : ");
  for (int i = 0; i < 18; i++)
    Serial.print(datagabung[i]);
  Serial.print(" => ");
  Serial.println(static_cast<uint16_t>(cek_id));

  Serial.print("sedang kirim data");
  Serial.println();
  radio.write(&datagabung, sizeof(datagabung)); // Kirim data gabungan


  uint16_t customerIdModul1;
  float userTotalUsageModul1;

  memcpy(&customerIdModul1, &datagabung[0], sizeof(customerIdModul1));
  memcpy(&userTotalUsageModul1, &datagabung[2], sizeof(userTotalUsageModul1));

  Serial.print("ID Pelanggan: ");
  Serial.println(customerIdModul1);
  Serial.print("Total Penggunaan Air: ");
  Serial.print(userTotalUsageModul1, 5);
  Serial.println(" m³");

  uint16_t customerIdModul2;
  float userTotalUsageModul2;

  memcpy(&customerIdModul2, &datagabung[6], sizeof(customerIdModul2));
  memcpy(&userTotalUsageModul2, &datagabung[8], sizeof(userTotalUsageModul2));

  Serial.print("ID Pelanggan: ");
  Serial.println(static_cast<uint16_t>(customerIdModul2));
  Serial.print("Total Penggunaan Air: ");
  Serial.print(userTotalUsageModul2, 5);
  Serial.println(" m³");

  uint16_t customerIdModul3;
  float userTotalUsageModul3;

  memcpy(&customerIdModul3, &datagabung[12], sizeof(customerIdModul3));
  memcpy(&userTotalUsageModul3, &datagabung[14], sizeof(userTotalUsageModul3));

  Serial.print("ID Pelanggan: ");
  Serial.println(static_cast<uint16_t>(customerIdModul3));
  Serial.print("Total Penggunaan Air: ");
  Serial.print(userTotalUsageModul3, 5);
  Serial.println(" m³");

}

void sensor() {

  // radio.stopListening();
  pulseDuration = pulseIn(sensorPin, FALLING);
  float currentLiters = pulseDuration * pulseToCubicMeters;
  totalLiters += currentLiters;

  const float newUserTotalUsage = totalLiters / 1000.0;

  memcpy(&newData2[0], &dev_id, sizeof(dev_id));
  memcpy(&newData2[2], &newUserTotalUsage, sizeof(newUserTotalUsage));

}

unsigned long rtimer0 = 0, timer0 = 1000;
unsigned long rtimer1 = 0, timer1 = 1500;
unsigned long previousCT2Millis = 0;
const long intervalCT2 = 50;

void loop() {
  unsigned long ct = millis();
  unsigned long ct2 = millis();
  unsigned long ct3 = millis();
  unsigned long currentMillis2 = millis();
  // unsigned long previousCT2Millis = millis();

  sensor();
  gabungData();
  if (currentMillis2 - previousCT2Millis >= intervalCT2) {
    bacanrf();
    previousCT2Millis = currentMillis2;
  }
  if (ct - rtimer0 >= timer0) {
    kirimnrf12();
    //    for(int i =0; i<18; i++)
    //      datagabung[i] = 0;
    rtimer0 = ct;
  }
  //  if (ct - rtimer0 >= timer0) {
  //    kirimnrf();
  //    rtimer0 = ct;
  //  }
  //  if (ct2 - rtimer1 >= timer1) {
  //    kirimnrf2();
  //    rtimer1 = ct2;
  //  }
  if (dataReceived) {
    digitalWrite(module2Pin, HIGH);
    ledOnMillis = millis();
    dataReceived = false;
  }
  if (millis() - ledOnMillis >= ledDuration) {
    digitalWrite(module2Pin, LOW);
  }
  lcd(); // Update OLED display
}
