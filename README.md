# RoomSense - IOT

Embedded dio projekta (prototip) za prikupljanje podataka o temperaturi, vlagi i razini CO2, te slanje podataka preko MQTT protokola.

![wired breadboard esp32c6 + DHT11 + NDIR CO2](https://leoklis.github.io/media/IOT_breadboard_wired.jpeg)

Cilj projekta je napraviti sustav za pracenje podataka o kvaliteti zraka i spojiti sustave za reguliranje istih (klima, prozori, grijanje...).

## Komponente

1. **ESP32 C6 dev board** - entry-level dev plocica bazirana na ESP32-C6-WROOM-1 modulu.
2. **DHT11** - digitalni senzor za prikupljanje informacija o temperaturi i vlazi.
3. **NDIR CO2 click** - sustav senzora plina CO2, koji mjeri apsolutnu koncentraciju CO2, korištenjem integriranog senzora CDM7160.

## Softver

Za razvoj je koristen ESP-IDF razvojni sustav i ESP-IDF FreeRTOS kernel.

## Korisni linkovi
- [NDIR CO2 click](https://www.mikroe.com/ndir-co2-click) - info i dokumentacija za CO2 senzor
- [ESP32-C6-DevKitC-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/index.html) - info i dokumentacija za ESP32 dev kit
- [DHT11 Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf) - dokumentacija za DHT11 senzor