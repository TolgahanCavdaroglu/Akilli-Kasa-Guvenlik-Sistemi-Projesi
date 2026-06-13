import cv2
import urllib.request
import numpy as np
import serial
import requests
import firebase_admin
from firebase_admin import credentials, db
from datetime import datetime
from insightface.app import FaceAnalysis
from numpy.linalg import norm

# Yüz modeli
app = FaceAnalysis()
app.prepare(ctx_id=0)

arduino = serial.Serial("COMPORT", 9600, timeout=1)
while arduino.in_waiting:
    print("TEMIZLENEN:", arduino.readline())
print("COM baglandi")
BOT_TOKEN = "****"
CHAT_ID = "****"
son_durum = ""
frame = None
cred = credentials.Certificate(
    "firebase_credentials.json"
)

firebase_admin.initialize_app(cred, {
    'databaseURL':
    'https://****'
})

ref = db.reference("olaylar")

# Referans fotoğraf
ref_img = cv2.imread("****.jpg")

ref_faces = app.get(ref_img)

if len(ref_faces) == 0:
    print("Referans fotoğrafta yüz bulunamadı!")
    exit()

known_embedding = ref_faces[0].embedding

url = "http://*****/capture"

while True:

    print("Bekleniyor...")

    if arduino.in_waiting:

        mesaj = arduino.readline().decode(errors="ignore").strip()

        print("ARDUINO:", mesaj)

        if mesaj == "LOG:SIFRE_YANLIS":

            ref.push({
                "olay": "SIFRE_YANLIS",
                "zaman": str(datetime.now())
            })

            print("Firebase: SIFRE_YANLIS")

        elif mesaj == "LOG:SIFRE_DOGRU":

            ref.push({
                "olay": "SIFRE_DOGRU",
                "zaman": str(datetime.now())
            })

            print("Firebase: SIFRE_DOGRU")

        elif mesaj == "LOG:YUZ_HATALI":

            ref.push({
                "olay": "YUZ_HATALI",
                "zaman": str(datetime.now())
            })

            print("Firebase: YUZ_HATALI")

        elif mesaj == "LOG:YUZ_BASARILI":

            ref.push({
                "olay": "YUZ_BASARILI",
                "zaman": str(datetime.now())
            })

            print("Firebase: YUZ_BASARILI")

        elif mesaj == "LOG:PARMAK_HATALI":

            ref.push({
                "olay": "PARMAK_HATALI",
                "zaman": str(datetime.now())
            })

            print("Firebase: PARMAK_HATALI")

        elif mesaj == "LOG:PARMAK_BASARILI":

            ref.push({
                "olay": "PARMAK_BASARILI",
                "zaman": str(datetime.now())
            })

            print("Firebase: PARMAK_BASARILI")

        elif mesaj == "LOG:OTP_HATALI":

            ref.push({
                "olay": "OTP_HATALI",
                "zaman": str(datetime.now())
            })

            print("Firebase: OTP_HATALI")
        elif mesaj == "LOG:OTP_DOGRU":

            ref.push({
                "olay": "OTP_DOGRU",
                "zaman": str(datetime.now())
            })

            print("Firebase: OTP_DOGRU")

        elif mesaj == "LOG:SISTEM_KILITLENDI":

            ref.push({
                "olay": "SISTEM_KILITLENDI",
                "zaman": str(datetime.now())
            })

            print("Firebase: SISTEM_KILITLENDI")
        elif mesaj == "LOG:KASA_KAPANDI":

            ref.push({
                "olay": "KASA_KAPANDI",
                "zaman": str(datetime.now())
            })

            requests.post(
                f"https://api.telegram.org/bot{BOT_TOKEN}/sendMessage",
                data={
                    "chat_id": CHAT_ID,
                    "text": "🔒 Kasa kapatildi."
                }
            )

            print("Firebase: KASA_KAPANDI")

        elif mesaj.startswith("LOG:GIRIS_BASARILI:"):

            kullanici = mesaj.split(":")[2]

            ref.push({
                "olay": "GIRIS_BASARILI",
                "kullanici": kullanici,
                "zaman": str(datetime.now())
            })

            print("Firebase: GIRIS_BASARILI ->", kullanici)

        elif mesaj == "PHOTO":

            if frame is not None:

                cv2.imwrite("supheli.jpg", frame)

                files = {
                    "photo": open("supheli.jpg", "rb")
                }

                data = {
                    "chat_id": CHAT_ID,
                    "caption": "⚠️ Yuz tanima hakki bitti!"
                }

                requests.post(
                    f"https://api.telegram.org/bot{BOT_TOKEN}/sendPhoto",
                    files=files,
                    data=data
                )

                print("Foto Telegram'a gonderildi")

    # Kameradan görüntü al
    img_resp = urllib.request.urlopen(url)
    img_np = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    frame = cv2.imdecode(img_np, cv2.IMREAD_COLOR)

    faces = app.get(frame)

    for face in faces:

        box = face.bbox.astype(int)
        embedding = face.embedding

        similarity = np.dot(
            known_embedding,
            embedding
        ) / (norm(known_embedding) * norm(embedding))

        if similarity > 0.55:

            label = f"****{similarity:.2f}"

            if son_durum != "FACE_OK":
                arduino.write(b"FACE_OK\n")
                print("FACE_OK gonderildi")
                son_durum = "FACE_OK"

        else:

            label = f"Bilinmeyen {similarity:.2f}"

            if son_durum != "FACE_FAIL":
                arduino.write(b"FACE_FAIL\n")
                print("FACE_FAIL gonderildi")
                son_durum = "FACE_FAIL"

        cv2.rectangle(
            frame,
            (box[0], box[1]),
            (box[2], box[3]),
            (0, 255, 0),
            2
        )

        cv2.putText(
            frame,
            label,
            (box[0], box[1] - 10),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2
        )

    cv2.imshow("Yuz Tanima", frame)

    if cv2.waitKey(1) == 27:
        break

cv2.destroyAllWindows()
arduino.close()