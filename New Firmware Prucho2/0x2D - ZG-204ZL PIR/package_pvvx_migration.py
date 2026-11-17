#!/usr/bin/env python3
"""Package the already-built Slacky Prucho2 v0.1.58 BIN for OTA from PVVX ZG-204ZL.

Outer ZCL OTA identity: PVVX 0x1141 / 0x0232 (board ID 50).
Embedded app remains UNCHANGED: Prucho2 0x6565 / 0x022d, v0.1.58.

This is a migration envelope ONLY, not a generic future-update image.
No recompile, header patch inside firmware, or modification to CRC.
"""
import binascii
import hashlib
import pathlib
import struct

HERE = pathlib.Path(__file__).resolve().parent
FW_PATH = HERE / "bin/prucho2_zg204zl_pir_V0.1.58.bin"
OUTPUT = HERE / "ota-from-pvvx/1141-0232-01583001-prucho2-zg204zl-from-pvvx-v0.1.58.zigbee"
VERSION = 0x01583001
INTERNAL_MANUFACTURER = 0x6565
INTERNAL_IMAGE_TYPE = 0x022D
PVVX_MANUFACTURER = 0x1141
PVVX_IMAGE_TYPE = 0x0232
PVVX_040_VERSION = 0x01403001

def main():
    fw = FW_PATH.read_bytes()
    assert 0 < len(fw) < 0x36000, "FW size outside 512-KB PVVX layout safety envelope"
    assert fw[6:8] == b"\x5d\\x02", "Not an OTA-compatible Telink application"
    assert fw[8:12] == b"KNLT", "Missing Telink bootable marker"
    internal_ver = int.from_bytes(fw[2:6], "little")
    internal_mfr = int.from_bytes(fw[18:20], "little")
    internal_type = int.from_bytes(fw[20:22], "little")
    internal_size = int.from_bytes(fw[24:28], "little")
    assert (internal_ver, internal_mfr, internal_type) == (
        VERSION, INTERNAL_MANUFACTURER, INTERNAL_IMAGE_TYPE), (
        f"Unexpected internal identity {internal_ver:08x}/{internal_mfr:04x}/{internal_type:04x}"
    )
    assert internal_size == len(fw), f"Internal length mismatch {internal_size} != {len(fw)}"
    stored_crc = int.from_bytes(fw[-4:], "little")
    computed_crc = binascii.crc32(fw[:-4]) ^ 0xFFFFFFFF
    assert stored_crc == computed_crc, "Telink firmware CRC invalid"
    assert VERSION > PVVX_040_VERSION, "OTA version is not an upgrade from PVVX 0.1.40"

    header_string = b"Prucho2 from PVVX ZG204ZL"
    header_string = header_string.ljust(32, b"\x00")
    header = struct.pack(
        "<I5HIH32sI",
        0x0BEEF11E,      # Zigbee OTA identifier
        0x0100,          # OTA header version
        56,              # OTA header length
        0,               # no optional headers
        PVVX_MANUFACTURER,
        PVVX_IMAGE_TYPE,
        VERSION,
        2,               # Zigbee stack version
        header_string,
        56 + 6 + len(fw)
    )
    assert len(header) == 56
    image = header + struct.pack("<HI", 0, len(fw)) + fw
    assert image[62:] == fw, "Firmware payload was modified"
    assert len(image) == len(fw) + 62
    magic, hdrver, hdrlen, fieldctrl, manu, imtype, version, stackver, label, total = struct.unpack(
        "<I5HIH32sI", image[:56]
    )
    assert (magic, hdrver, hdrlen, fieldctrl, manu, imtype, version, total) == (
        0x0BEEF11E, 0x0100, 56, 0, 0x1141, 0x0232, VERSION, len(image)
    )
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_bytes(image)
    print(f"PVVX-migration OTA ready: {OUTPUT.name}")
    print(f"  Zigbee OTA wrapper manufacturer=0x{manu:04X}, image=0x{imtype:04X}, version=0x{version:08X}")
    print(f"  Embedded Prucho2 manufacturer=0x{internal_mfr:04X}, image=0x{internal_type:04X}")
    print(f"  Firmware bytes={len(fw)}, package bytes={len(image)}, CRC=0x{stored_crc:08X}")
    print(f"  SHA256={hashlib.sha256(image).hexdigest()}")

if __name__ == "__main__":
    main()
