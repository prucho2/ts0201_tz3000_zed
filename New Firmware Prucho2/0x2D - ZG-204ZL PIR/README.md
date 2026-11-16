# Prucho2 — 0x2D ZG-204ZL PIR (TLSR8253)

**To jest samodzielny katalog źródeł do budowania PIR-a.** Nie musisz szukać zmian w głównych plikach `src/` repozytorium.

**Status v0.1.48: eksperymentalny, NIE działa jeszcze PIR (PD4 stale LOW) i światło może pokazywać N/A.** Poprawna kompilacja nie oznacza działającego urządzenia. To nie jest wersja produkcyjna.

## Co gdzie jest?

| Ścieżka w tym folderze | Do czego służy |
|---|---|
| `src/common/board_zg204zl_pir_prucho2.h` | **Piny sprzętowe**: PIR PD4, fotoczujnik PC4, zasilanie światła PD3, LED PD7, przycisk PA1 |
| `src/app_pir_prucho2.c` | **PIR**: odczyt GPIO, Occupancy, podtrzymanie ruchu, wybudzanie, pełne 11 liczników diagnostycznych |
| `src/app_pir_lux_prucho2.c` | **Światło**: pomiar ADC i warunki raportowania; obecnie kod z firmware 0.1.41 |
| `src/app_pm.c` | Zasypianie i wybudzanie |
| `src/app_main.c` | Inicjalizacja i pętla programu |
| `src/app_endpoint_cfg.c` | Klastry i atrybuty Zigbee, w tym Illuminance i Occupancy |
| `src/include/version_cfg.h` | **Numer wersji firmware (PIR `APP_BUILD`)** |
| `src/include/app_pir_prucho2.h` | Deklaracje funkcji i diagnostyki |
| `zigbee2mqtt/prucho2_pir_zg204zl.mjs` | Konwerter Z2M; to nie jest część pliku BIN |
| `build.py` | Samodzielna kompilacja wyłącznie z plików tego projektu |
| `bin/` | Miejsce na wyniki BIN i Zigbee OTA, tworzone przez build; nie publikuje automatycznie na produkcję |

## Jak samodzielnie zmieniać kod?

1. Wejdź w **New Firmware Prucho2 → 0x2D - ZG-204ZL PIR → src**.
2. Wybierz plik, np. `src/app_pir_lux_prucho2.c`. Kliknij ikonę ołówka w GitHubie (Edit this file), popraw wartości i kliknij **Commit changes** na gałęzi testowej.
3. Przejdź do **Actions → PIR 0x2D - Build from project folder**. Możesz użyć **Run workflow** (wybierz gałąź `dev/prucho2-pir-pvvx-v141-baseline`), lub poczekać na automatyczną kompilację po zmianie pliku `src/`.
4. Otwórz zakończony run, na dole strony w **Artifacts → PIR-0x2D-test-build** pobierzesz paczkę z plikami BIN i Zigbee OTA.

**Ważne:** nową wersję firmware podnieś w `src/include/version_cfg.h` w sekcji `BOARD_PRUCHO2_PIR_ZG204ZL` (np. po 0x47 kolejne 0x48). `build.py` sam odczytuje numer i nazywa pliki. Nie trzeba zmieniać `makefile` ani workflow.

## Budowanie lokalne

Z komputera z Python 3, GNU Make i kompilatorem Telink TC32 (ścieżka `/opt/tc32/bin/tc32-elf-gcc` na Linuksie) z katalogu repozytorium:

```bash
python3 "New Firmware Prucho2/0x2D - ZG-204ZL PIR/build.py"
```

Skrypt kopiuje projekt do katalogu tymczasowego, nakłada pliki z tego katalogu na wspólne źródła Slacky, kompiluje, sprawdza numer obrazu, zgodność plików, CRC32 i tworzy `bin/` **tylko w katalogu PIR**. Na Windows najprościej korzystać z GitHub Actions bez lokalnego toolchaina.

## Architektura i bezpieczeństwo

- Podstawa: Slacky-DIY, źródła odtworzone z v0.1.41, commit `5c41888e7f85a8888490b4ed1abd6c1ca75b26a2`.
- PIR: adaptacja `task_pir()` / `occupancyTimerCb()` / wake-on-opposite-level z PVVX, nie pełny firmware PVVX.
- Diagnostyka: wszystkie 11 liczników 0xF100–0xF10A, zbiorczy raport przy zdarzeniu.
- Światło: zachowane źródło v0.1.41 (lokalnie 10 s; raport przy zmianie >=10 jednostek ZCL, synchronizacja 3600 s). Brak nowej funkcji raportowania co 60 s.
- Wspólny SDK Telink, biblioteki i narzędzia pozostają w głównym repozytorium i są używane podczas kompilacji; nie ma potrzeby kopiowania 1400 plików SDK do tego katalogu.
- **Główny `src/` nie jest już źródłem zmian dla PIR w nowym workflow.** Stary workflow nadal istnieje dla starej gałęzi i należy go odróżniać od nowego.
- Zmiana kodu w GitHubie lub udana kompilacja nie flashuje urządzenia i nie restartuje HA. HA MAIN bez zmian.

## Diagnostyka światła — build testowy 0.1.48

W module `src/app_pir_lux_prucho2.c` dodano **10 atrybutów ZCL (0xF200–0xF209)**. Są wysyłane automatycznym raportem diagnostycznym około raz na 60 sekund po dołączeniu do sieci, niezależnie od zwykłego `illuminance`. Z2M wymaga konwertera z podkatalogu `zigbee2mqtt`. Nie zmieniono 10-sekundowego cyklu prób ani progu 10 jednostek ZCL / 1h synchronizacji w rzeczywistym raporcie światła.

| Pole Z2M | Co oznacza |
|---|---|
| `lux_timer_ticks` | Ile razy wywołał się timer zaplanowanego pomiaru |
| `lux_samples` | Ile razy wykonano odczyt ADC |
| `lux_adc_vbat` | Surowy odczyt ADC zasilania przed włączeniem PD3 |
| `lux_adc_pc4` | Surowy odczyt ADC fotoczujnika na PC4 |
| `lux_zcl_value` | Wartość standardowego atrybutu MeasuredValue (nie liniowe lux) |
| `lux_report_attempts` | Ile prób wysłania normalnego raportu światła |
| `lux_reports_enqueued` | Ile raportów przyjął stos Zigbee (nie gwarancja dostarczenia do Z2M) |
| `lux_reports_failed` | Ile wywołań raportowania odrzucił stos |
| `lux_diag_snapshots` | Ile zbiorczych snapshotów przyjął stos (aktualny snapshot może pokazać wartość sprzed zwiększenia) |
| `lux_status` | Maska bitowa: 1=był pomiar; 2=któryś ADC zwrócił zero; 4=joined; 8=czeka normalny raport |

Interpretacja: jeśli `lux_timer_ticks` przyrasta, lecz `lux_samples` nie — problem w pętli aplikacji; jeśli oba przyrastają, lecz ADC PC4 lub VBAT jest zerowe — sprawdzić konfigurację ADC i pinów; jeśli ADC i `lux_zcl_value` są prawidłowe, a `lux_report_attempts` stoi — próg raportowania lub brak startowego raportu; jeśli stos przyjmuje raporty, a `illuminance` pozostaje `N/A` — potrzebna jest diagnostyka transmisji/konwertera. Uwaga: wartości surowe ADC nie są miliwoltami i nie należy ich interpretować jako luksów.

**Niepotwierdzone na urządzeniu.** Zmiana dodaje telemetrię, nie jest deklaracją naprawienia światła ani PIR. Do ustawień PVVX na PC4 i źródła 1.41 wrócimy na podstawie tych wyników.
