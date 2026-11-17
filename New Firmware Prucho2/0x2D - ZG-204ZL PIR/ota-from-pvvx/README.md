# OTA migration: PVVX ZG-204ZL → Prucho2 v0.1.58

This package is only for ZG-204ZL (TLSR8253, PVVX image type 0x0232,
manufacturer 0x1141) currently running PVVX v0.1.40.

The OTA OUTER header uses 1141/0232/01583001 to be recognized by PVVX.
The EMBEDDED unmodified binary uses Prucho2 6565/022D/01583001.
It is the exact 0.1.58 Telink firmware image; internal CRC verified.

This follows the OTA migration method described by Slacky-DIY: an OTA
container has the old vendor's Zigbee OTA identity, but its firmware
payload contains the new application's identity.

**Physical migration confirmed on TEST:** the user installed the migration OTA
on the PVVX PIR and after Zigbee2MQTT Interview the device was identified as
Prucho2. The original PVVX identity persisted in the Z2M UI until Interview.
Keep the 512-KB Flash backup and verify the incoming device identity on
any additional PCB variant; individual update success does not prove
compatibility with all PVVX revisions.

After installation, Zigbee2MQTT may need device re-pairing and the
Prucho2 external converter. Subsequent Prucho2 OTA updates use the
normal 6565/022D identity, not this one-time migration package.

Script: `../package_pvvx_migration.py`.
