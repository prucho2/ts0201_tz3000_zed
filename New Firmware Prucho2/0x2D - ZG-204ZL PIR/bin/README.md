# Wynikowe pliki firmware

Po uruchomieniu `build.py` lokalnie tutaj powstaną:
- `prucho2_zg204zl_pir_V0.1.47.bin`
- `prucho2_zg204zl_pir_last_version.bin` (identyczna kopia)
- `6565-022d-01473001-prucho2_zg204zl_pir.zigbee` (OTA)

Po udanej kompilacji GitHub Actions zapisuje również pliki **bezpośrednio w tym folderze `bin/` na gałęzi testowej**, oprócz kopii w **Artifacts → PIR-0x2D-test-build**.

**Nie publikuj testowych obrazów jako sprawdzonych.** Aktualnie 0.1.47 nie działa prawidłowo w części PIR i światła.

Wyniki kompilacji przez GitHub Actions są automatycznie commitowane do `bin/` na gałęzi testowej. Jest to **archiwum eksperymentalnych obrazów**, a nie potwierdzenie poprawnej pracy urządzenia. Oprócz tego paczka w Artifacts jest dostępna przez 30 dni.
