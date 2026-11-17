# New Firmware Prucho2 — katalog sprzętu i migracji OTA

Własne wersje firmware bazujące na [Slacky-DIY](https://github.com/slacky1965/ts0201_tz3000_zed)
oraz migracje oryginalnych urządzeń PVVX. **Wersja kodu, wersja BIN i OTA
to trzy różne rzeczy**; nagłówek OTA dla migracji PVVX może mieć inne
identyfikatory niż wewnętrzny obraz nowego firmware.

## Projekty

| Profil | Urządzenie | Firmware / sterownik | Dokumentacja |
|---|---|---|---|
| `0x2A` | SONOFF SNZB-02 oraz eWeLink 7014 | Prucho2 v7.0.02, CHT8305 | [0x2A](./0x2A%20-%20SONOFF-SNZB-02-_TZ3000_fllyghyj%20%2B%20eWeLink%207014/README.md) |
| `0x2B` | ZTH01 PCB 30201 0025-V2.0 | Prucho2 v7.0.02, SHT30 | [0x2B](./0x2B%20-%20ZTH01%2030201%200025-V2.0/README.md) |
| `0x2C` | TS0201 AoYan | Prucho2 v7.0.04, GXHTV4/SHT4x | [0x2C](./0x2C%20-%20TS0201%20AoYan/README.md) |
| `0x2D` | ZG-204ZL PIR (TLSR8253) | **Prucho2 v0.1.58**, PIR PD4 + opcjonalny lux PC4 | [Specyfikacja 0x2D](./0x2D%20-%20ZG-204ZL%20PIR/README.md) |
| `0x23` (oryginalny Slacky) | ZG-227Z-z PVVX, termometr/wilgotnościomierz | **Slacky-DIY v1.0.10**, AHT20; bez nowych źródeł firmware | [Migracja PVVX → Slacky 0x23](./0x23%20-%20ZG-227Z%20PVVX%20to%20Slacky-DIY/README.md) |

Profil 0x23 nie jest nową płytką Prucho2; używamy **istniejącego upstream Slacky-DIY**.
Dla 0x23 mamy w repo oryginalny [BIN](../bin/ts0201_tz3000_0x23_zed_last_version.bin)
i [OTA Slacky](../bin/6565-0223-10103001-ts0201_tz3000_0x23_zed.zigbee).

## Aktualizacje PVVX — ważne rozróżnienie

| Źródło | Docelowy firmware | Nagłówek migracyjny OTA | Status |
|---|---|---|---|
| ZG-204ZL z PVVX 0.1.40 | Prucho2 0x2D v0.1.58 | `1141/0232/01583001` | **OTA przeszło na czujniku TEST; po Interview Z2M rozpoznał nowy model** |
| ZG-227Z-z z PVVX 0.1.40 | Oryginalny Slacky-DIY 0x23 v1.0.10 | `1141/0227/10103001` | **Pakiet przygotowany, fizycznie nieprzetestowany** |

- [PIR: pakiet i instrukcja migracji](./0x2D%20-%20ZG-204ZL%20PIR/ota-from-pvvx/README.md)
- [ZG-227Z: pakiet i instrukcja migracji](./0x23%20-%20ZG-227Z%20PVVX%20to%20Slacky-DIY/README.md)
- [Specyfikacja i bezpieczeństwo migracji](./OTA_MIGRATION_SPEC.md)

**Przed wgrywaniem**: potwierdzić obraz OTA (manufacturer, image type i file version),
wariant PCB, typ czujnika I²C, zapisać pełny backup Flash. Nie przenosić
po prostu nagłówka OTA pomiędzy różnymi modelami.

## Budowanie i bezpieczeństwo

- PIR 0x2D jest oddzielnym projektem z własnym `src/`, `build.py`, `bin/`,
  konwerterem Z2M i automatyczną kompilacją przez GitHub Actions.
- Firmware 0x2D v0.1.58 ma CPU 48 MHz; domyślny occupancy timeout **3 s**,
  lux co **20 s**, raport przy zmianie **20 lx** lub po **300 s**,
  bez dodatkowego pomiaru przy ruchu; diagnostyka programowa została usunięta.
- Kontener OTA **nie jest** programem do uruchamiania: zawiera kompletny BIN,
  którego nagłówka Telink/CRC nie wolno zmieniać.
- Obecne firmware 0x2A, 0x2B, 0x2C i 0x2D nie są przebudowywane
  przy generowaniu jednorazowych obrazów migracyjnych.
- Nie restartować HA MAIN bez wyraźnej zgody użytkownika.
