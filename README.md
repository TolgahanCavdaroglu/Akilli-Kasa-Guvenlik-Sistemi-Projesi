# 🔐 Akıllı Kasa Güvenlik Sistemi

Bu proje, geleneksel kasa sistemlerinin güvenliğini artırmak amacıyla geliştirilmiş dört aşamalı kimlik doğrulama mekanizmasına sahip bir akıllı kasa sistemidir.

Sistemde kullanıcı doğrulaması aşağıdaki adımlar ile gerçekleştirilmektedir:

1. Şifre Doğrulama
2. Yüz Tanıma
3. Parmak İzi Doğrulama
4. Tek Kullanımlık Parola (OTP) Doğrulama

Tüm doğrulama adımlarının başarıyla tamamlanması durumunda kasa açılmaktadır.

---

## 🚀 Kullanılan Donanımlar

- Deneyap Kart
- ESP32-S3-CAM
- R307S Parmak İzi Sensörü
- 16x2 I2C LCD Ekran
- 4x4 Matrix Keypad
- SG90 Servo Motor
- Breadboard
- Güç Kartı

---

## 💻 Kullanılan Yazılımlar ve Teknolojiler

- Arduino IDE
- Python
- OpenCV
- InsightFace
- Firebase Realtime Database
- Telegram Bot API
- HTML / CSS / JavaScript

---

## 🔑 Sistem Özellikleri

- Çok katmanlı kimlik doğrulama
- Gerçek zamanlı yüz tanıma
- Parmak izi doğrulama
- OTP üretimi ve doğrulama
- Telegram üzerinden bildirim gönderimi
- Firebase üzerinde olay kayıtlarının tutulması
- Web tabanlı güvenlik izleme paneli
- Hatalı girişlerde sistem kilitleme mekanizması

---

## 📂 Proje Dosyaları

| Dosya | Açıklama |
|---------|---------|
| Arduino_Kasa.ino | Deneyap Kart üzerinde çalışan ana sistem kodu |
| kamera_tanima.py | Yüz tanıma ve doğrulama uygulaması |
| firebase_test.py | Firebase bağlantı testleri |
| indexx.html | Web tabanlı izleme paneli |
| Rapor.pdf | Proje raporu |

---

## 📊 Sistem Mimarisi

Kullanıcı önce şifre doğrulamasından geçer. Ardından ESP32-S3-CAM ile yüz doğrulama gerçekleştirilir. Yüz doğrulaması başarılı olursa parmak izi doğrulama aşamasına geçilir. Son olarak Telegram üzerinden gönderilen OTP kodu doğrulanır. Tüm aşamalar başarıyla tamamlandığında servo motor çalıştırılarak kasa açılır.

---

## 👨‍💻 Geliştiriciler

- Tolgahan Çavdaroğlu
- Buse İrem Yeşiltaş

---

## ⚠️ Güvenlik Notu

Bu repoda güvenlik amacıyla aşağıdaki bilgiler paylaşılmamıştır:

- Firebase servis hesabı anahtarları
- Firebase URL bilgileri
- Telegram Bot Token bilgileri
- Telegram Chat ID bilgileri
- Kullanıcılara ait biyometrik veriler
