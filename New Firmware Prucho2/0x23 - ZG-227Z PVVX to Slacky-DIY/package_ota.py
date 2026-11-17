#!/usr/bin/env python3
"""PVVX OTA envelope for existing Slacky-DIY ZG-227Z board 0x23 (no compilation)."""
import binascii
import hashlib
from pathlib import Path
import struct

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
SOURCE = ROOT / "bin/ts0201_tz3000_0x23_zed_last_version.bin"
OUTPUT = HERE / "1141-0227-10103001-zg227z-pvvx-to-slacky-0x23.zigbee"

def main():
    fw = SOURCE.read_bytes()
    assert len(fw) == 154676, "Unexpected Slacky 0x23 BIN size"
    assert fw[6:8] == bytes.fromhex("5d02") and fw[8:12] == b"KNLT", "Invalid Telink OTA-compatible BIN marker"
    version = int.from_bytes(fw[2:6], "little")
    manufacturer = int.from_bytes(fw[18:20], "little")
    image_type = int.from_bytes(fw[20:22], "little")
    image_size = int.from_bytes(fw[24:28], "little")
    assert (version, manufacturer, image_type, image_size) == (0x10103001, 0x6565, 0x0223, len(fw))
    stored = int.from_bytes(fw[-4:], "little")
    computed = (binascii.crc32(fw[:-4]) ^ 0xFFFFFFFF) & 0xFFFFFFFF
    assert stored == computed == 0xCC80D08F, "Telink CRC failed"
    label = b"Slacky-DIY ZG-227Z from PVVX".ljust(32, bytes(1))
    assert len(label) == 32
    hdr = struct.pack("<I5HIH32sI", 0x0BEEF11E, 0x0100, 56, 0,
        0x1141, 0x0227, 0x10103001, 2, label, 62 + len(fw))
    assert len(hdr) == 56
    ota = hdr + struct.pack("<HI", 0, len(fw)) + fw
    assert ota[62:] == fw and len(ota) == 154738
    assert struct.unpack_from("<H", ota, 10)[0] == 0x1141
    assert struct.unpack_from("<H", ota, 12)[0] == 0x0227
    assert struct.unpack_from("<I", ota, 14)[0] == 0x10103001
    OUTPUT.write_bytes(ota)
    print(f"{OUTPUT.name}: size={len(ota)} bytes, Telink CRC={stored:08X}, SHA256={hashlib.sha256(ota).hexdigest()}")

if __name__ == "__main__":
    main()
