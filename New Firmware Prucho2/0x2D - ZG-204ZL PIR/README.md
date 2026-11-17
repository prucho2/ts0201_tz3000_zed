# Prucho2 — 0x2D ZG-204ZL PIR (TLSR8253)

**To jest samodzielny katalog źródeł do budowania PIR-a.** Nie musisz szukać zmian w głównych plikach `src/` repozytorium.

**Status v0.1.49: eksperymentalny, PIR (PD4) nadal wymaga testu na sprzęcie; światło było potwierdzone w v0.1.48.** Poprawna kompilacja nie oznacza działającego urządzenia. To nie jest wersja produkcyjna.

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
| `bin/` | **Gotowe obrazy BIN i Zigbee OTA**, automatycznie zapisywane do repozytorium po udanej kompilacji w GitHub Actions; tylko TEST |

## Jak samodzielnie zmieniać kod?

1. Wejdź w **New Firmware Prucho2 → 0x2D - ZG-204ZL PIR → src**.
2. Wybierz plik, np. `src/app_pir_lux_prucho2.c`. Kliknij ikonę ołówka w GitHubie (Edit this file), popraw wartości i kliknij **Commit changes** na gałęzi testowej.
3. Przejdź do **Actions → PIR 0x2D - Build from project folder**. Możesz użyć **Run workflow** (wybierz gałąź `dev/prucho2-pir-pvvx-v141-baseline`), lub poczekać na automatyczną kompilację po zmianie pliku `src/`.
4. Otwórz zakończony run, na dole strony w **Artifacts → PIR-0x2D-test-build** pobierzesz paczkę z plikami BIN i Zigbee OTA.

**Ważne:** po udanym GitHub Actions buildzie trzy obrazy są też widoczne w tym folderze `bin/`, bez osobnego pobierania Artifacts. Nową wersję firmware podnieś w `src/include/version_cfg.h` w sekcji `BOARD_PRUCHO2_PIR_ZG204ZL` (np. po 0x47 kolejne 0x48). `build.py` sam odczytuje numer i nazywa pliki. Nie trzeba zmieniać `makefile` ani workflow.

## Budowanie lokalne

Z komputera z Python 3, GNU Make i kompilatorem Telink TC32 (ścieżka `/opt/tc32/bin/tc32-elf-gcc` na Linuksie) z katalogu repozytorium:

```bash
python3 "New Firmware Prucho2/0x2D - ZG-204ZL PIR/build.py"
```

Skrypt kopiuje projekt do katalogu tymczasowego, nakłada pliki z tego katalogu na wspólne źródła Slacky, kompiluje, sprawdza numer obrazu, zgodność plików, CRC32 i tworzy `bin/` **tylko w katalogu PIR**. GitHub Actions dodatkowo commituje te obrazy do repozytorium; lokalny build zapisuje je tylko lokalnie. Na Windows najprościej korzystać z GitHub Actions bez lokalnego toolchaina.

## Architektura i bezpieczeństwo

- Podstawa: Slacky-DIY, źródła odtworzone z v0.1.41, commit `5c41888e7f85a8888490b4ed1abd6c1ca75b26a2`.
- PIR: adaptacja `task_pir()` / `occupancyTimerCb()` / wake-on-opposite-level z PVVX, nie pełny firmware PVVX.
- Diagnostyka: wszystkie 11 liczników 0xF100–0xF10A, zbiorczy raport przy zdarzeniu.
- Światło: zachowane źródło v0.1.41 (lokalnie 10 s; raport przy zmianie >=10 jednostek ZCL, synchronizacja 3600 s). Brak nowej funkcji raportowania co 60 s.
- Wspólny SDK Telink, biblioteki i narzędzia pozostają w głównym repozytorium i są używane podczas kompilacji; nie ma potrzeby kopiowania 1400 plików SDK do tego katalogu.
- **Główny `src/` nie jest już źródłem zmian dla PIR w nowym workflow.** Stary workflow nadal istnieje dla starej gałęzi i należy go odróżniać od nowego.
- Zmiana kodu w GitHubie lub udana kompilacja nie flashuje urządzenia i nie restartuje HA. HA MAIN bez zmian.

## Diagnostyka światła — build testowy 0.1.49

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


## Test v0.1.49 (17 listopada 2026)

- PIR: według Slacky TS0207 czujnika zalania aktualizacja polaryzacji wybudzania PAD obu pinów PA1 i PD4 tuż przed wejściem w sleep; przy zmianie PD4 krótkie podtrzymanie aktywnego CPU na 3 kontrole po 5 ms. Zdarzenie occupancy nie czeka na zakończenie tych kontroli. 11 istniejących pól diagnostycznych pozostaje bez zmian. To test, nie potwierdzona naprawa.
- Lux: pomiar ADC co 2 sekundy; raport gdy zmiana o >=20 **rzeczywistych luksów** względem ostatnio zgłoszonego stanu; raport wymuszony po 30 sekundach od ostatniego zgłoszenia; każde zaakceptowane przez stos zgłoszenie resetuje 30-sekundowe odliczanie. Wysyłka 30-sekundowa dotyczy zwykłego illuminance, niezależna telemetria diagnostyczna pozostaje co 60 sekund.
- Skorygowano arytmetykę stałoprzecinkową pomiaru lux do bezpiecznej 32-bitowej skali Q12, kompatybilnej z kompilatorem TC32 (nie wymaga 64-bitowych funkcji runtime). To może zmienić wartości lux; porównać z rzeczywistymi warunkami.
- Przed testem potwierdzić poprawną kompilację pliku bin i CRC32 w GitHub Actions; nie restartować HA MAIN.


## Test v0.1.50 — diagnostyka okresowa

- W Z2M automatycznie pojawia się pełny 11-polowy snapshot diagnostyki PIR co 10 s, nawet jeśli PD4 nigdy nie zmienia poziomu. Liczniki nadal zwiększają się lokalnie na bieżąco; raport pokazuje stan na chwilę wysłania. `pir_sleep_returns` oznacza powroty z funkcji uśpienia, a nie jednoznacznie wybudzenia od PIR.
- Pełny snapshot diagnostyczny światła jest wysyłany co 15 s. Normalne raporty `illuminance` pozostają niezależne: ADC co 2 s, próg 20 lx, synchronizacja co 30 s od ostatniego raportu.
- Bez zmian w wykrywaniu PIR, mapowaniu GPIO, atrybutach/konwerterze Z2M i obsłudze energii względem v0.1.49.
- Wersja tylko do diagnostyki, zwiększa ruch Zigbee i zużycie energii. Po rozwiązaniu problemu przywrócić oszczędne raportowanie.


## Test v0.1.51 — tylko PC5

- Jedyna zmiana funkcjonalna względem v0.1.50: definicje startupowe GPIO PC5 ustawione zgodnie z oryginalnym board header PVVX dla ZG-204ZL (`PC5_FUNC=AS_GPIO`, `PC5_INPUT_ENABLE=OFF`, `PC5_OUTPUT_ENABLE=ON`, `PC5_DATA_OUT=1`).
- Nie zmieniono kodu PIR PD4, zarządzania energią, pomiaru światła i okresowych snapshotów diagnostycznych. Sterownik ADC może ponownie konfigurować PC5 w czasie pracy; test nie dowodzi ciągłego zasilania PIR przez PC5 ani stałego utrzymywania stanu PC5 po pomiarach ADC.
- Bezpieczny eksperyment tylko na egzemplarzu testowym z możliwością przywrócenia v0.1.50.


## Test v0.1.52 — porównanie ustawień platformy z PVVX 0.1.40

- Oddzielny tylko dla PIR `src/include/app_cfg.h` (nie zmienia wspólnego `src/`) ustawia taktowanie 24 MHz zamiast 48 MHz oraz `VOLTAGE_DETECT_ENABLE=OFF` zamiast ON; zgodnie ze źródłami PVVX 0.1.40.
- Ponieważ wyłączono platformową kontrolę napięcia, `src/app_main.c` jawnie inicjalizuje ADC przed pierwszym odczytem baterii (`drv_adc_init`, `drv_adc_mode_pin_set` VBAT PC5, `drv_adc_enable`) i nadal pozwala na pomiary światła PC4.
- Ustawienie PC5 HIGH z v0.1.51 pozostaje; odczyt PD4, mechanizm PM, światło 2s/20lx/30s i diagnostyka PIR 10s / lux 15s niezmienione.
- Obie zmiany platformowe są w jednym eksperymencie na wyraźne żądanie; ewentualny pozytywny wynik nie rozstrzygnie, która zmiana pomogła. Monitorować `pir_high_samples`, `pir_rising_edges`, `occupancy`, `lux_adc_vbat`, `lux_adc_pc4`, `illuminance`.
