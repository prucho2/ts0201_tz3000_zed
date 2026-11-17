# OTA migration: PVVX ZG-204ZL → Prucho2 v0.1.58

This package is only for ZG-204ZL (TLSR8253, PVVX image type 0x0232,
manufacturer 0x1141) currently running PVVX v0.1.40.

The OTA OUTER header uses 1141/0232/01583001 to be recognized by PVVX.
The EMBEDDED unmodified binary uses Prucho2 6565/022D/01583001.
It is the exact 0.1.58 Telink firmware image; internal CRC verified.

This follows the OTA migration method described by Slacky-DIY: an OTA
container has the old vendor's Zigbee OTA identity, but its firmware
payload contains the new application's identity.

**Experimental migration, not yet tested on physical PVVX hardware.**
Start with the disassembled TEST sensor only; preserve full 512-KB
flash backup and confirm that the device currently announces
manufacturer 0x1141/image type 0x0232 in OTA identity. No guarantee
that a matching OTA envelope proves the running PVVX OTA implementation
will accept and successfully boot the target binary.

After installation, Zigbee2MQTT may need device re-pairing and the
Prucho2 external converter. Subsequent Prucho2 OTA updates use the
normal 6565/022D identity, not this one-time migration package.

Script: `../package_pvvx_migration.py`.
