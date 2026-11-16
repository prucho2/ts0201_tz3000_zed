# Wynikowe pliki firmware

Po uruchomieniu `build.py` lokalnie tutaj powstaną:
- `prucho2_zg204zl_pir_V0.1.47.bin`
- `prucho2_zg204zl_pir_last_version.bin` (identyczna kopia)
- `6565-022d-01473001-prucho2_zg204zl_pir.zigbee` (OTA)

Na GitHub Actions są pobierane w **Artifacts → PIR-0x2D-test-build** po kompilacji.

**Nie publikuj testowych obrazów jako sprawdzonych.** Aktualnie 0.1.47 nie działa prawidłowo w części PIR i światła.

Skrypt nie zapisuje plików BIN automatycznie do historii GitHub: do repozytorium trafia **kod źródłowy**, natomiast wyniki kompilacji są przechowywane jako **Artifacts** (30 dni). Gdy firmware zostanie sprawdzone na prawdziwym urządzeniu, można ręcznie zarchiwizować ostateczne BIN/OTA w tym katalogu, tak jak w projektach 0x2A–0x2C.
