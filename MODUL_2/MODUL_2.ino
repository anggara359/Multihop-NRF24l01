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
const uint64_t beforeModuleId = 0xF0F0F0F0E2LL;
const uint64_t moduleId = 0xF0F0F0F0E3LL;
const uint16_t dev_id = 2;

//Time for Sending millis
unsigned long previousMillis = 0;
const long interval = 2000;

//Data
byte messageData[6];
byte newData2[6];
byte datagabung[12];

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
    display.print("MODUL 2");
    
    // Place "Jumlah Data Modul 1" below "MODUL 2"
    x = 10;
    y = 9;
    
    display.setCursor(x, y);
    display.print("Jumlah Data Modul 1 = ");
    display.println(dataCounterNewCustomer);
    display.display();
  }

void bacanrf() {
  
  //Setting Listening 
  radio.openReadingPipe(1, beforeModuleId);
  radio.startListening();
  
  //Cek Radio 
  if (radio.available()) {
    radio.read(&messageData, sizeof(messageData));
    dataReceived = true;
    dataCounter++;
    uint16_t customerId;
    memcpy(&customerId, &messageData[0], sizeof(customerId));
    if (customerId == 1) {
      dataCounterNewCustomer++;
    }
}
  lcd(); // Update OLED display
}


void gabungData(){
  memcpy(datagabung, &messageData, sizeof(messageData));
  // Salin data dari newData ke datagabung setelah messageData
  memcpy(datagabung + sizeof(messageData), &newData2, sizeof(newData2));
}

void kirimnrf12() {
  radio.stopListening();
  radio.openWritingPipe(moduleId);
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
unsigned long prev_reset_millis = 0;
unsigned long reset_interval = 5000;
unsigned long previousCT2Millis = 0;
const long intervalCT2 = 50; //50

void loop() {
  unsigned long _millis = millis();

 sensor();
  
 if (_millis - previousCT2Millis >= intervalCT2) {
      bacanrf();
     previousCT2Millis = _millis;
 }
  if (_millis - rtimer0 >= timer0) {
    gabungData();
    kirimnrf12();
//    memset(gabungData, 0, sizeof(gabungData)*sizeof(gabungData[0]));
    rtimer0 = _millis;
  }

//  if(_millis - prev_reset_millis > reset_interval)
//  {
//    Serial.println("RESET");
//    prev_reset_millis = _millis;
//  }
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
    ledOnMillis = _millis;
    dataReceived = false;
  }
  if (_millis - ledOnMillis >= ledDuration) {
    digitalWrite(module2Pin, LOW);
  }
  lcd(); // Update OLED display
}
