#include <ChipNorm_VL53L0X.h>

#define SCL 22 //SCL mit D22 verbinden
#define SDA 21 //SDA mit D21 verbinden
#define I2C_Address 0x50 //I2C Adresse

ChipNorm_VL53L0X VL53L0X(SCL, SDA, I2C_Address); //Initialisiert den Sensor (Objekt "VL53L0X" der Klasse "ChipNorm_VL53L0X" erstellt) 

void setup() {
  Serial.begin(9600); // Serielle Kommunikation initialisieren - 9600 bits pro Sekunde
  
  // Initialisiert den VL53L0X-Sensor
  if (!VL53L0X.begin()) {
    Serial.println("Fehler beim Initialisieren des VL53L0X-Sensors!");
    delay(1000);
    while (true); // Stoppt das Programm bei einem Fehler.
  }
  Serial.println("VL53L0X Sensor initialisiert.");
}

void loop() {
  float distance = VL53L0X.readDistance(); // Auslesen der Distanz

  // Prüft auf Lesefehler (falls NAN zurückgegeben wurde).
  if (isnan(distance)) {
    Serial.println("Fehler beim Lesen der Sensorwerte!");
  } else {
    // Ausgabe der Messwerte
    Serial.print("Distanz: ");
    Serial.println(distance, 2); // Ausgabe Distanz mit zwei Dezimalstellen
  }
  
  delay(1000);  // Wartet 1 Sekunde
}
