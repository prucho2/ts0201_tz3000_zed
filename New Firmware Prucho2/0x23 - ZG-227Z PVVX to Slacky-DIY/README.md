# ZG-227Z-z — migracja PVVX 0.1.40 → Slacky-DIY 0x23

**Status:** pakiet OTA przygotowany i statycznie zweryfikowany; **nieprzetestowany jeszcze na tym fizycznym ZG-227Z**.
To nie jest nowy build firmware. Payload to istniejący plik Slacky-DIY:
[`bin/ts0201_tz3000_0x23_zed_last_version.bin`](../../bin/ts0201_tz3000_0x23_zed_last_version.bin).

## Konkretny egzemplarz (identyfikacja z Zigbee2MQTT)

| Pole | Wartość |
|---|---|
| IEEE | `0xa4c138d71e23d70a` |
| NWK | `0xA9CC` / 43468; **adres dynamiczny**, nie identyfikuje sprzętu na stałe |
| Model aktualny | `ZG-227Z-z` |
| Manufacturer Zigbee (Basic) | `Tuya` |
| Firmware aktualne | PVVX 0.1.40, ID `3001-0140`, liczba 20983809 = `0x01403001` |
| Data firmware zgłoszona przez urządzenie | 2026-10-19 |
| Docelowy model Slacky-DIY | `TS0201-z23-SlD` |
| Docelowe firmware | Slacky-DIY v1.0.10, `0x10103001` |
| Układ | TLSR8253, 512 KB Flash (wg profilu sprzętowego PVVX) |

**IEEE i NWK są dokumentacją tego egzemplarza; nie zostały zapisane do OTA ani do obrazu firmware.**
OTA jest przeznaczone dla zgodnego **modelu i profilu**, a nie przypisane kryptograficznie do jednego IEEE.

## Identyfikatory OTA: zewnętrzny nagłówek a docelowy program

| Pole | Aktualne PVVX | Specjalny pakiet migracyjny | Normalne OTA Slacky |
|---|---:|---:|---:|
| Manufacturer Code | `0x1141` | **`0x1141`** | `0x6565` |
| Image Type | `0x0227` | **`0x0227`** | `0x0223` |
| File Version | `0x01403001` | **`0x10103001`** | `0x10103001` |

Pakiet `1141-0227-10103001-zg227z-pvvx-to-slacky-0x23.zigbee` ma **62-bajtowy** nagłówek/opakowanie Zigbee OTA
(56-bajtowy nagłówek + 6-bajtowy subelement) i zawiera **identyczny, niezmodyfikowany** BIN Slacky.
Firmware ma własny nagłówek Telink: `6565/0223/10103001`.
Rozmiar BIN: **154 676 B**; rozmiar OTA: **154 738 B**;
wewnętrzny Telink CRC32: **`0xCC80D08F`**.

Nie zmieniamy producenta ani Image Type wewnątrz BIN.
Zmiana następuje tylko w **nagłówku zewnętrznego pakietu**, by klient PVVX
mógł zaakceptować aktualizację. Po pierwszym uruchomieniu docelowy Slacky
będzie raportował własne identyfikatory.

## Zgodność sprzętowa i ograniczenia

- PVVX: [`board_zg_227z.h`](https://github.com/pvvx/ZigbeeTLc/blob/master/src/board_zg_227z.h):
  PC3 SDA, PC4 SCL, PC2 LED, PD4 przycisk, PC5 VBAT.
- Slacky: [`board_zg_227zs.h`](../../src/common/board_zg_227zs.h):
  **te same piny**, lecz **`SENSOR_AHT20`** jako sterownik, I²C 200 kHz.
- PVVX dla tej płytki może korzystać z CHT8215, AHT20/30 lub SHT4x.
  **Nie mamy potwierdzenia, jaki układ jest na wskazanej płytce**;
  typ należy sprawdzić przed OTA. Jeśli nie jest AHT20-kompatybilny,
  pomiar temperatury/wilgotności w Slacky może nie działać.
- README oryginalnego Slacky zaznacza dla ZG-227Z aktualizację **BIN/programatorem**
  z firmware fabrycznego. Ten pakiet jest **niestandardową migracją z już zainstalowanego PVVX**
  obsługującego OTA i nie jest potwierdzoną przez Slacky procedurą dla ZG-227Z.
- Podstawą przygotowania OTA jest PVVX `1141/0227`. Przed aktualizacją
  **odczytaj rzeczywisty identyfikator OTA urządzenia** (nie tylko model ID w Z2M).
  W repo PVVX występuje też alternatywny kontener `1286/0203` do wcześniejszych firmware.
- Zgodność kontenera i CRC **nie gwarantuje** poprawnego zapisu, restartu lub działania nowego firmware.
  Zalecany backup pełnych 512 KB i fizyczny dostęp do SWS do odzyskania czujnika.

## Procedura testowa (bez usuwania urządzenia na początku)

1. Sprawdź rodzaj układu T/H i potwierdź, że PVVX OTA podaje `0x1141/0x0227`.
2. Zrób backup flash (512 KB) i zachowaj kopię IEEE / danych kalibracyjnych.
3. W Zigbee2MQTT w urządzeniu wybierz firmware OTA → **Custom firmware** i plik
   `1141-0227-10103001-zg227z-pvvx-to-slacky-0x23.zigbee`.
4. Wybudź czujnik przyciskiem i wykonaj aktualizację. Zwróć uwagę na log oraz status flashowania.
5. Po restarcie zrób **Interview** (wybudzając przyciskiem). Z2M potrafi trzymać dawną identyfikację,
   dopóki nie przeprowadzi ponownego odczytu.
6. Sprawdź docelowy model `TS0201-z23-SlD`, temperaturę, wilgotność, baterię oraz aktualizacje przez kilka cykli.
7. Dalsze aktualizacje Slacky wykonuj standardowym OTA dla `0x6565/0x0223`,
   nie tym jednorazowym kontenerem migracyjnym.

## Pliki i odtwarzalność

- [Pakiet przejściowy `1141-0227-10103001-zg227z-pvvx-to-slacky-0x23.zigbee`](./1141-0227-10103001-zg227z-pvvx-to-slacky-0x23.zigbee).
- [Generator `package_ota.py`](./package_ota.py) tworzy ten kontener z BIN, bez kompilacji.
- [Oryginalne OTA Slacky 0x23](../../bin/6565-0223-10103001-ts0201_tz3000_0x23_zed.zigbee)
  jest **inne** i nie jest przeznaczone na pierwszą aktualizację PVVX.
- [Oryginalna instrukcja Slacky-DIY](https://github.com/slacky1965/ts0201_tz3000_zed#как-обновить).
