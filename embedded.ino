#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD dengan alamat I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 karakter LCD

// Pin untuk Ultrasonic A (penampungan sementara)
const int trigPinA = 7;
const int echoPinA = 6;

// Pin untuk Ultrasonic B (penampungan flush)
const int trigPinB = 9;
const int echoPinB = 8;

// Pin untuk relay kontrol pompa dan solenoid
const int pumpRelayPin = 10;   // Relay Channel 1 untuk pompa
const int solenoidRelayPin = 12; // Relay Channel 2 untuk solenoid

// Variabel untuk menyimpan hasil pengukuran jarak
long durationA, durationB;
int distanceA, distanceB;

// Rentang jarak untuk persentase
const int minDistanceA = 6;  // Jarak minimum (penuh) untuk penampungan sementara
const int maxDistanceA = 12; // Jarak maksimum (kosong) untuk penampungan sementara

const int minDistanceB = 6;  // Jarak minimum (penuh) untuk penampungan flush
const int maxDistanceB = 22; // Jarak maksimum (kosong) untuk penampungan flush

void setup() {
  // Inisialisasi LCD
  lcd.begin(16, 2); // 16 kolom, 2 baris
  lcd.backlight();

  // Inisialisasi pin sensor
  pinMode(trigPinA, OUTPUT);
  pinMode(echoPinA, INPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);

  // Inisialisasi pin relay
  pinMode(pumpRelayPin, OUTPUT);
  pinMode(solenoidRelayPin, OUTPUT);

  // Matikan pompa dan solenoid saat awal
  digitalWrite(pumpRelayPin, LOW); // Relay off
  digitalWrite(solenoidRelayPin, LOW); // Relay off

  // Serial monitor untuk debug
  Serial.begin(9600);
}

void loop() {
  // Membaca jarak dari Ultrasonic A (penampungan sementara)
  distanceA = readUltrasonic(trigPinA, echoPinA);

  // Membaca jarak dari Ultrasonic B (penampungan flush)
  distanceB = readUltrasonic(trigPinB, echoPinB);

  // Hitung persentase ketinggian air
  int percentageA = calculatePercentage(distanceA, maxDistanceA, minDistanceA);
  int percentageB = calculatePercentage(distanceB, maxDistanceB, minDistanceB);

  // Kontrol solenoid valve
  if (distanceA <= minDistanceA) { // Penampungan sementara penuh
    digitalWrite(solenoidRelayPin, LOW); // Matikan solenoid (tutup aliran)
    Serial.println("Solenoid: OFF (tertutup)");
  } else {
    digitalWrite(solenoidRelayPin, HIGH); // Hidupkan solenoid (buka aliran)
    Serial.println("Solenoid: ON (terbuka)");
  }

  // Kontrol pompa
  if (distanceA < 9 && distanceB > minDistanceB) {
    digitalWrite(pumpRelayPin, HIGH); // Nyalakan pompa
    Serial.println("Pompa: ON");
  } else {
    digitalWrite(pumpRelayPin, LOW); // Matikan pompa
    Serial.println("Pompa: OFF");
  }

  // Menampilkan persentase di LCD
  lcd.clear(); // Bersihkan layar untuk mencegah teks lama tertinggal

  // Tampilkan persentase Penampungan Sementara di baris pertama
  lcd.setCursor(0, 0); // Baris pertama, kolom pertama
  lcd.print("Sementara: ");
  lcd.print(percentageA);
  lcd.print("%");

  // Tampilkan persentase Penampungan Flush di baris kedua
  lcd.setCursor(0, 1); // Baris kedua, kolom pertama
  lcd.print("Flush: ");
  lcd.print(percentageB);
  lcd.print("%");

  delay(1000); // Tunggu 1 detik sebelum pengukuran berikutnya
}

// Fungsi untuk membaca jarak dari sensor ultrasonic
int readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Membaca durasi pulsa echo
  long duration = pulseIn(echoPin, HIGH);

  // Konversi durasi menjadi jarak (cm)
  int distance = duration * 0.034 / 2;

  return distance;
}

// Fungsi untuk menghitung persentase tinggi air
int calculatePercentage(int sensorReading, int maxDistance, int minDistance) {
  // Pastikan pembacaan sensor berada dalam rentang
  if (sensorReading > maxDistance) sensorReading = maxDistance;
  if (sensorReading < minDistance) sensorReading = minDistance;

  // Hitung persentase berdasarkan rumus: (maxDistance - reading) / (maxDistance - minDistance) * 100
  int percentage = ((maxDistance - sensorReading) * 100) / (maxDistance - minDistance);

  return percentage;
}