#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <DFPlayer_Mini_Mp3.h>

// Konstanta dan variabel
const int aktif = 6;
const int daftar = 2;
const int hapus = 3;
const int relay1 = 15;
const int relay2 = 14;
const int alamat = 0;
byte isi;
int k = 0;
boolean b = false;
boolean a = false;

// Objek
SoftwareSerial mySerial(7, 8);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Fungsi setup
void setup() {
    Serial.begin(9600);
    mp3_set_serial(Serial);
    mp3_set_volume(25);

    pinMode(13, OUTPUT);
    pinMode(aktif, INPUT_PULLUP);
    pinMode(daftar, INPUT_PULLUP);
    pinMode(hapus, INPUT_PULLUP);
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);

    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);

    mySerial.begin(9600);
    finger.begin(57600);

    if (!finger.verifyPassword()) {
        while (1);
    }

    delay(100);
    if (!digitalRead(hapus)) {
        EEPROM.write(alamat, 0);
        delay(1);
    }

    isi = EEPROM.read(alamat);
    if (isi > 50) {
        EEPROM.write(alamat, 0);
    }

    isi = EEPROM.read(alamat);
    while (!digitalRead(daftar)) {
        Serial.print("Daftar");
        digitalWrite(13, HIGH);
        getFingerprintEnroll(isi);
        digitalWrite(13, LOW);
        delay(10000);
        
        if (isi < 50) {
            isi++;
        }
        
        EEPROM.write(alamat, isi);
        delay(100);
        isi = EEPROM.read(alamat);
    }
}

// Fungsi loop
void loop() {
    b = digitalRead(relay1);
    a = digitalRead(aktif);

    if (a) {
        Serial.println(b);
        Serial.println("masuk");
        getFingerprintID();
    }
}

// Fungsi pendaftaran sidik jari
uint8_t getFingerprintEnroll(int id) {
    int p = -1;

    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
    }

    p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK) return p;

    delay(2000);
    while (p != FINGERPRINT_NOFINGER) {
        p = finger.getImage();
    }

    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
    }

    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK) return p;

    p = finger.createModel();
    if (p != FINGERPRINT_OK) return p;

    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
        EEPROM.write(alamat, 2);
        delay(1000);
    }

    return p;
}

// Fungsi pencarian sidik jari
uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) return p;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return p;

    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
        // Sidik jari ditemukan
    } else {
        Serial.println("Salah");
        mp3_play(3);
        k++;

        while (p != FINGERPRINT_NOFINGER) {
            p = finger.getImage();
        }

        delay(2000);
        if (k > 2) {
            mp3_play(4);
            k = 0;
            delay(300000); // Kunci selama 5 menit
        }

        return p;
    }

    k = 0;
    if (b) {
        digitalWrite(relay1, LOW);
        mp3_play(1);

        while (p != FINGERPRINT_NOFINGER) {
            p = finger.getImage();
        }

        for (int a = 0; a < 50; a++) {
            p = finger.getImage();
            if (p == FINGERPRINT_OK) {
                p = finger.image2Tz();
                if (p == FINGERPRINT_OK) {
                    p = finger.fingerSearch();
                    if (p == FINGERPRINT_OK) {
                        goto starter;
                    }
                }
            }
            delay(1000);
            if (a >= 25) goto mati;
        }

        starter:
        mp3_play(2);
        delay(2000);
        digitalWrite(relay2, LOW);
        delay(1200);
        digitalWrite(relay2, HIGH);
    } else {
        mati:
        digitalWrite(relay1, HIGH);
        while (p != FINGERPRINT_NOFINGER) {
            p = finger.getImage();
        }
    }

    return finger.fingerID;
}

// Fungsi mendapatkan ID sidik jari dengan cepat
int getFingerprintIDez() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK) {
        Serial.println("Salah");
        mp3_play(3);
        k++;

        while (p != FINGERPRINT_NOFINGER) {
            p = finger.getImage();
        }

        delay(2000);
        if (k > 2) {
            mp3_play(4);
            k = 0;
            delay(300000); // Kunci selama 5 menit
        }

        return -1;
    }

    k = 0;
    if (b) {
        digitalWrite(relay1, LOW);
        mp3_play(1);

        while (p != FINGERPRINT_NOFINGER) {
            p = finger.getImage();
        }

        for (int a = 0; a < 50; a++) {
            p = finger.getImage();
            if (p == FINGERPRINT_OK) {
                p = finger.image2Tz();
                if (p == FINGERPRINT_OK) {
                    p = finger.fingerFastSearch();
                    if (p == FINGERPRINT_OK) {
                        goto starter;
                    }
                }
            }
            delay(1000);
            if (a >= 25) goto mati;
        }

        starter:
        mp3_play(2);
        delay(2000);
        digitalWrite(relay2, LOW);
        delay(1200);
        digitalWrite(relay2, HIGH);
    } else {
        mati:
        digitalWrite(relay1, HIGH);
        while (p != FINGERPRINT_NOFINGER) {
            p = finger.getImage();
        }
    }

    return finger.fingerID;
}
