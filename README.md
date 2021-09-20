# Data Logger
### A. Spesifikasi:

1. Data Logger bisa diakses lewat internet baik lewat HP ataupun komputer.
2. Data Logger tidak tergantung dengan server cloud sehingga bisa mandiri.
3. Dapat menampilkan nilai pengukuran temperature/suhu dan kelembaban/humidity secara real time (aktual) baik secara lokal maupun on-line (lewat internet).
4. Display lokal berupa OLED 64x48 pixel, dan hanya menampilkan nilai dalam angka berikut satuan.
5. Nilai pengukuran yang ditampilkan melalui web browser dalam gambar ber-angka/gauge.
6. Memberikan peringatan dalam bentuk visual saat nilai pengukuran melebihi nilai alarm.
7. Memberikan grafik trending selama satu jam secara real time.
8. Grafik trending bisa juga digunakan untuk menampilkan rerata harian selama satu bulan.
9. Terdapat fasilitas memberikan laporan setiap hari berupa file pdf.
10. File laporan (pdf) dapat di-download serta dapat di-share by WA atau email terpisah.
11. Sebelum di-download data ditampilkan dalam bentuk tabel berdasar urutan jam selama 24 jam.
12. Data setting Alarm untuk temperature dan humidity bisa dilakukan perubahan (setting ulang) untuk menyesuaikan kondisi aktual.
13. Perubahan setting itu juga berlaku pada parameter sensor seperti range low dan high, unit dll, dan harus letakkan dalam satu display.
14. Hanya petugas tertentu saja yang bisa melakukan perubahan itu.
15. Petugas yang melakukan perubahan harus masuk/login dulu (username & password) agar bisa melakukan perubahan parameter.
### B. Hardware : [hardware-dwg](https://github.com/slametsampon/dataLogger/blob/main/images/dataLogger.png)

#### 
1. Menggunakan Wemos D1 R1 sebagai dasar module [shield](https://github.com/slametsampon/dataLogger/blob/main/images/esp8266-wemos-d1-mini-nodemcu-pinout.png)
2. DHT 11 - sensor Temperature dan Humidity[DHT11](https://github.com/slametsampon/dataLogger/blob/main/images/dht-temperature-humudity-sensor-pinout.jpg)
3. OLED 64 x 48 Pixel - Display lokal [OLED](https://github.com/slametsampon/dataLogger/blob/main/images/oled-shield-v200-for-wemos-d1-mini-066-inch-64x48-iic-i2c-two-button-1-1-1024x1024.jpg)
4. 3-Module di atas disusun dalam bentuk stack atas - bawah
### C. Software : [software-Cfg](https://github.com/slametsampon/dataLogger/blob/main/config/dataLogger.md)
