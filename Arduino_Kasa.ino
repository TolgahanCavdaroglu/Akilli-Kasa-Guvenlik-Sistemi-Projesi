#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_Fingerprint.h>

HardwareSerial mySerial(1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

bool parmakBekleniyor = false;

const char* ssid = "****";
const char* password = "****";
#define BOTtoken "*****"
#define CHAT_ID "****"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo motor;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { A0, A1, A2, A3 };
byte colPins[COLS] = { A4, A5, D5, D2 };

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS);

String girilen = "";
String sifre = "*****";

int hak = 3;
int yuzHak = 10;
bool yuzBekleniyor = false;
String otpKod = "";
String girilenOtp = "";
bool otpBekleniyor = false;
String aktifKullanici = "";
bool kilitli = false;
bool sistemAcik = false;

unsigned long otpBaslamaZamani = 0;
const unsigned long OTP_SURESI = 30000;  //  30 saniye
void setup() {
  client.setInsecure();
  randomSeed(millis());
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("WiFi Baglaniyor");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  lcd.clear();
  lcd.print("WiFi Baglandi");
  delay(1500);
  client.setInsecure();
  motor.setPeriodHertz(50);
  motor.attach(D3, 1000, 2000);
  // PARMAK IZI SENSOR BASLAT
  mySerial.begin(57600, SERIAL_8N1);

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Parmak izi sensor bulundu");
  } else {
    Serial.println("Parmak izi sensor bulunamadi");
  }

  motor.write(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sifre Gir:");
}

void loop() {
  if (sistemAcik) {

    char key = keypad.getKey();

    if (key == 'A') {
      bot.sendMessage(
        CHAT_ID,
        "A tusuna basildi",
        "");

      motor.write(0);

      sistemAcik = false;
      Serial.println("LOG:KASA_KAPANDI");
      girilen = "";
      girilenOtp = "";

      lcd.clear();
      lcd.print("Kasa Kapandi");

      delay(2000);

      lcd.clear();
      lcd.print("Sifre Gir:");
    }

    return;
  }

  if (kilitli) {

    lcd.clear();
    lcd.print("Sistem Kilitli");

    delay(500);
    return;
  }


  // PARMAK IZI BEKLEME
  if (parmakBekleniyor) {

    uint8_t p = finger.getImage();

    if (p != FINGERPRINT_OK) {
      return;
    }

    p = finger.image2Tz();

    if (p != FINGERPRINT_OK) {
      return;
    }

    p = finger.fingerSearch();

    if (p == FINGERPRINT_NOTFOUND) {
      Serial.println("LOG:PARMAK_HATALI");

      hak--;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Parmak Hatali");

      lcd.setCursor(0, 1);
      lcd.print("Hak:");
      lcd.print(hak);

      bot.sendMessage(
        CHAT_ID,
        "Izinsiz parmak izi denemesi!",
        "");

      delay(2000);

      if (hak <= 0) {
        Serial.println("LOG:SISTEM_KILITLENDI");
        lcd.clear();
        lcd.print("Sistem Kilitli");

        bot.sendMessage(
          CHAT_ID,
          "Sistem KILITLENDI!",
          "");

        kilitli = true;
      }

      return;
    }
    Serial.println("LOG:PARMAK_BASARILI");

    hak = 3;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hosgeldiniz");

    if (finger.fingerID == 1) {
      aktifKullanici = "*****";
      lcd.setCursor(0, 1);
      lcd.print("****");
    }

    else if (finger.fingerID == 2) {
      aktifKullanici = "****";
      lcd.setCursor(0, 1);
      lcd.print("****");
    }



    delay(2000);

    parmakBekleniyor = false;

    otpKod = String(100000 + esp_random() % 900000);

    otpBaslamaZamani = millis();

    bot.sendMessage(
      CHAT_ID,
      "OTP Kodunuz:\n\n" + otpKod,
      "");

    otpBekleniyor = true;

    lcd.clear();
    lcd.print("OTP Giriniz");

    return;
  }

  // OTP BEKLEME
  if (otpBekleniyor) {
    if (millis() - otpBaslamaZamani > OTP_SURESI) {
      Serial.println("LOG:OTP_SURESI_DOLDU");
      lcd.clear();
      lcd.print("OTP Suresi Doldu");

      bot.sendMessage(
        CHAT_ID,
        "OTP suresi doldu!",
        "");

      delay(2000);

      otpBekleniyor = false;
      girilenOtp = "";
      girilen = "";

      lcd.clear();
      lcd.print("Sifre Gir:");

      return;
    }


    char key = keypad.getKey();

    if (key) {

      if (key >= '0' && key <= '9') {

        girilenOtp += key;

        lcd.setCursor(0, 1);
        lcd.print(girilenOtp);
      }

      else if (key == '#') {

        if (girilenOtp == otpKod) {
          Serial.println("LOG:OTP_DOGRU");
          lcd.clear();
          lcd.print("OTP Dogru");

          bot.sendMessage(
            CHAT_ID,
            "Giris Basarili - " + aktifKullanici,
            "");
          Serial.println("LOG:GIRIS_BASARILI:" + aktifKullanici);
          lcd.clear();
          lcd.print("Kasa Acildi");
          delay(1000);
          motor.write(180);
          Serial.println("LOG:KASA_ACILDI");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Hosgeldiniz");

          lcd.setCursor(0, 1);
          lcd.print(aktifKullanici);

          sistemAcik = true;

          otpBekleniyor = false;
        } else {
          Serial.println("LOG:OTP_HATALI");

          lcd.clear();
          lcd.print("OTP Hatali");

          delay(2000);

          girilenOtp = "";

          lcd.clear();
          lcd.print("OTP Giriniz");
        }
      }
    }

    return;
  }

  // YUZ TANIMA BEKLEME
  if (yuzBekleniyor) {

    if (Serial.available()) {

      String veri = Serial.readStringUntil('\n');
      veri.trim();

      if (veri == "FACE_OK") {

        Serial.println("LOG:YUZ_BASARILI");
        yuzHak = 10;
        bot.sendMessage(
          CHAT_ID,
          "Yuz Tanima Basarili!",
          "");

        parmakBekleniyor = true;
        yuzBekleniyor = false;

        lcd.clear();
        lcd.print("Parmak Okut");
      }

      else if (veri == "FACE_FAIL") {
        Serial.println("LOG:YUZ_HATALI");
        yuzHak--;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Yuz Hatali");

        lcd.setCursor(0, 1);
        lcd.print("Hak:");
        lcd.print(yuzHak);
        bot.sendMessage(
          CHAT_ID,
          "Yuz Tanima Hatali! Kalan Hak: " + String(yuzHak),
          "");

        delay(2000);

        if (yuzHak <= 0) {
          Serial.println("LOG:SISTEM_KILITLENDI");
          lcd.clear();
          lcd.print("Sistem Kilitli");

          bot.sendMessage(
            CHAT_ID,
            "Yuz tanima hakki bitti! Sistem kilitlendi.",
            "");
          Serial.println("PHOTO");

          kilitli = true;
          return;
        }

        lcd.clear();
        lcd.print("Yuz Bekleniyor");
      }
    }

    return;
  }

  if (hak == 0) {

    Serial.println("LOG:SISTEM_KILITLENDI");

    lcd.clear();
    lcd.print("Kilitlendi");

    bot.sendMessage(
      CHAT_ID,
      "Sifre hakki bitti! Sistem kilitlendi.",
      "");

    delay(1000);

    while (true)
      ;
  }

  char key = keypad.getKey();

  if (key) {

    if (key == '*') {

      if (girilen.length() > 0) {

        girilen.remove(girilen.length() - 1);

        lcd.setCursor(0, 1);
        lcd.print("                ");

        lcd.setCursor(0, 1);

        for (int i = 0; i < girilen.length(); i++) {
          lcd.print("*");
        }
      }
    }

    else if (key == '#') {

      lcd.clear();

      if (girilen == sifre) {

        Serial.println("LOG:SIFRE_DOGRU");

        yuzBekleniyor = true;

        lcd.print("Yuz Bekleniyor");
      } else {

        Serial.println("LOG:SIFRE_YANLIS");

        hak--;

        lcd.print("Hatali");
        if (hak <= 0) {

          Serial.println("LOG:SISTEM_KILITLENDI");

          bot.sendMessage(
            CHAT_ID,
            "Sifre hakki bitti! Sistem kilitlendi.",
            "");

          kilitli = true;
        }
        lcd.setCursor(0, 1);

        lcd.print("Hak:");
        lcd.print(hak);

        delay(2000);

        girilen = "";

        lcd.clear();
        lcd.print("Sifre Gir:");
      }
    }

    else {

      girilen += key;

      lcd.setCursor(0, 1);

      for (int i = 0; i < girilen.length(); i++) {
        lcd.print("*");
      }
    }
  }
}
