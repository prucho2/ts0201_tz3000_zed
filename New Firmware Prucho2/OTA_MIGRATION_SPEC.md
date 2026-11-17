# Specyfikacja migracyjnych pakietów OTA — Prucho2 / Slacky-DIY

## Format i walidacja

- Format Zigbee OTA: **56-bajtowy nagłówek** (`0x0BEEF11E`,
  header version `0x0100`, field control 0, stack version 2)
  + **6-bajtowy nagłówek subelementu** (tag `0x0000`, length BIN)
  + kompletny obraz Telink .bin.
- Pole total image size to `56 + 6 + len(BIN)`.
- BIN musi zachować wewnętrzne `KNLT`, długość zadeklarowaną
  w offset `0x18` oraz CRC32 Telink na końcu obrazu:
  `stored_crc = binascii.crc32(bin[:-4]) ^ 0xffffffff`.
- **Zewnętrzne** manufacturer/image type identyfikują urządzenie, które ma
  *przyjąć* upgrade; **wewnętrzne** manufacturer/image type są własnością
  firmware, które będzie działać po restarcie.
- Przygotowanie migracji nie zmienia ani jednego bajtu embedded BIN.

## Potwierdzone parametry

| Źródło | Zewnętrzne OTA | Wewnętrzny firmware | Rozmiar BIN/OTA |
|---|---|---|---|
| ZG-204ZL PVVX 0.1.40 | `1141-0232-01583001` | Prucho2 `6565-022D-01583001` | 152 324 / 152 386 B |
| ZG-227Z PVVX 0.1.40 | `1141-0227-10103001` | Slacky-DIY `6565-0223-10103001` | 154 676 / 154 738 B |

## Warunki uruchomienia

1. Sprawdzić **rzeczywiste OTA manufacturer i image type** urządzenia (mogą
   różnić się od pola manufacturerName z klastra Basic).
2. Potwierdzić wariant PCB, układ Flash (TLSR8253 512 KB), GPIO i
   rodzaj sensora. W ZG-227Z Slacky 0x23 oczekuje sterownika AHT20.
3. Zachować 512-KB backup Flash, by mieć możliwość odzyskania przez Telink SWS.
4. W Zigbee2MQTT wybrać `Custom firmware` i odpowiedni jednorazowy obraz.
   Wybudzić urządzenie, przeprowadzić OTA.
5. Po migracji wykonać Interview (może być konieczne wybudzenie przyciskiem).
   Sam restart Z2M nie zawsze odświeża model.
6. Sprawdzić działanie sensorów oraz zidentyfikować nowy model. Dalsze
   aktualizacje stosują *normalne* OTA dla nowego firmware.
7. Nie używać obrazu od PIR na ZG-227Z, ani odwrotnie.

## Rezultaty testów

- **ZG-204ZL → Prucho2 0x2D 0.1.58:** użytkownik potwierdził przejście OTA;
  początkowo Z2M widział dawną definicję PVVX, po Interview przełączył się.
- **ZG-227Z → Slacky 0x23 1.0.10:** wygenerowany i zweryfikowany
  pakiet OTA; brak testu fizycznego i potwierdzenia typu sensora T/H.

## Obraz i konwerter PIR

- Konwerter zewnętrzny:
  [`prucho2_pir_zg204zl.mjs`](./0x2D%20-%20ZG-204ZL%20PIR/zigbee2mqtt/prucho2_pir_zg204zl.mjs)
- Obrazek urządzenia:
  `https://www.zigbee2mqtt.io/images/devices/ZG-204ZL-z.png`.
  Obrazek jest ustawieniem ikony urządzenia w Z2M, nie właściwością
  Zigbee OTA ani częścią pliku .bin.
