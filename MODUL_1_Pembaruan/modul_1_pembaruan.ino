#include <RF24.h>
#define PACKET_SIZE 6

// Konfigurasi Alamat
RF24 radio(10, 9);
const uint64_t moduleId = 0xF0F0F0F0E2LL;
const uint16_t dev_id = 1;

// Time for Sending millis
unsigned long previousMillis = 0;
const long interval = 2000;

// Konfigurasi Flow sensor
unsigned long pulseDuration = 0;
const float pulseToCubicMeters = 0.00000247;  //data set 
const int sensorPin = A0;
float totalLiters = 0;
bool dataReceived = false;

void kirimnrf() {
  
  // Read flow sensor data
  pulseDuration = pulseIn(sensorPin, FALLING);
  if (pulseDuration > 0) {
    float currentLiters = pulseDuration * pulseToCubicMeters;
    totalLiters += currentLiters;
    dataReceived = true;
  } else {
    totalLiters += 0.50;  // Menggunakan data dummy
    dataReceived = false;
  }

  // Data that will be sent
  const float newUserTotalUsage = totalLiters / 1000.0;

  byte newData[6];

  // Struktur data
  memcpy(&newData[0], &dev_id, sizeof(dev_id));
  memcpy(&newData[2], &newUserTotalUsage, sizeof(newUserTotalUsage));

  Serial.print("ID Pelanggan Baru: ");
  Serial.println(static_cast<uint16_t>(dev_id));
  Serial.print("Total Penggunaan Air Baru: ");
  Serial.print(newUserTotalUsage, 5); // Display 5 decimal places for precision
  Serial.println(" m³");
  Serial.print("Cek point : ");
  uint16_t cek_id;
  memcpy(&cek_id, &newData[0], sizeof(cek_id));
  for (int i = 0; i < 6; i++) {
    Serial.print(newData[i]);
  }
  Serial.print(" => ");
  Serial.println(cek_id);
  radio.write(&newData, PACKET_SIZE); // Send new data to Module 1
}

void setup() {
  // Set up radio Frekuensi 
  radio.begin();
  radio.openWritingPipe(moduleId);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(85);
  Serial.begin(115200);
}

void loop() {
  // Write data
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    kirimnrf();
    previousMillis = currentMillis;
  }
}
