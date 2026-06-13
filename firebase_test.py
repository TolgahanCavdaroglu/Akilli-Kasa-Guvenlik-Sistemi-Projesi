import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

cred = credentials.Certificate(
        "firebase-adminsdk.json"
)

firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://****'
})

ref = db.reference('olaylar')

ref.push({
    'kullanici': '****',
    'olay': 'Test',
    'durum': 'Firebase Baglandi'
})

print("Firebase'e veri gonderildi.")