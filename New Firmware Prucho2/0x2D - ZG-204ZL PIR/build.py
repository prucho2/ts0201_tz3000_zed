#!/usr/bin/env python3
"""Build ZG-204ZL PIR from THIS project's src/ without altering shared root src/.

Requirements: Python 3, GNU make, Telink tc32-elf toolchain (the GitHub
Actions workflow installs those automatically).
"""
import binascii
import pathlib
import shutil
import subprocess
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent.parent
SOURCE = HERE / "src"
OUTPUT = HERE / "bin"
TARGET = "prucho2_zg204zl_pir"
VERSION = "0.1.47"
VERSION_HEADER = 0x01473001
FILES = (
    "app_pir_prucho2.c",
    "app_pir_lux_prucho2.c",
    "app_main.c",
    "app_pm.c",
    "app_endpoint_cfg.c",
    "common/board_zg204zl_pir_prucho2.h",
    "include/version_cfg.h",
    "include/app_pir_prucho2.h",
)

def main():
    with tempfile.TemporaryDirectory(prefix="pir-build-") as tmp:
        work = pathlib.Path(tmp) / "firmware"
        # Never edit the shared root sources, even during a local build.
        shutil.copytree(ROOT, work, ignore=shutil.ignore_patterns(
            ".git", "out", "__pycache__", ".venv", "*.pyc"
        ))
        for filename in FILES:
            source = SOURCE / filename
            dest = work / "src" / filename
            assert source.is_file(), f"Missing project source: {source}"
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, dest)
        # Override stale hardcoded name in the common Slacky makefile.
        subprocess.run(
            ["make", "PROJECT_NAME=" + TARGET,
             "PROJECT_DEF=-DBOARD=BOARD_PRUCHO2_PIR_ZG204ZL",
             "DEBUG=-DUART_PRINTF_MODE=OFF",
             "VERSION_RELEASE=V0.1", "VERSION_BUILD=47"],
            cwd=work, check=True
        )
        bin_dir = work / "bin"
        binary = bin_dir / (TARGET + "_V0.1.47.bin")
        current = bin_dir / (TARGET + "_last_version.bin")
        ota = bin_dir / ("6565-022d-01473001-" + TARGET + ".zigbee")
        b = binary.read_bytes()
        assert b == current.read_bytes(), "versioned and last_version images differ"
        assert int.from_bytes(b[2:6], "little") == VERSION_HEADER, "wrong version header"
        assert int.from_bytes(b[18:20], "little") == 0x6565, "wrong manufacturer"
        assert int.from_bytes(b[20:22], "little") == 0x022D, "wrong image type"
        assert b[6:8] == bytes([0x5D, 0x02]), "OTA magic missing"
        expected_crc = int.from_bytes(b[-4:], "little")
        actual_crc = binascii.crc32(b[:-4]) ^ 0xFFFFFFFF
        assert actual_crc == expected_crc, (
            f"CRC mismatch: stored {expected_crc:08X}, actual {actual_crc:08X}"
        )
        assert ota.is_file(), "OTA image missing"
        OUTPUT.mkdir(parents=True, exist_ok=True)
        for file in (binary, current, ota):
            shutil.copy2(file, OUTPUT / file.name)
        print(f"SUCCESS: v{VERSION}, CRC32 0x{actual_crc:08X}; files in {OUTPUT}")

if __name__ == "__main__":
    main()
