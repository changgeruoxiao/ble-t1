# nRF52840 Packet Sniffer bring-up

## What the USB sniffer is

The separate `nRF52840 Packet Sniffer` USB stick does **not** connect by wire to the ProMicro application board. It listens to Bluetooth LE packets over the 2.4 GHz air interface and forwards captures to the PC over USB.

If the seller has already programmed compatible nRF Sniffer firmware, the hardware-side first step is simply to plug it into the PC. However, it is not literally zero-setup: Windows must enumerate the device/serial port and the PC needs Nordic's current nRF Sniffer host tooling (or an equivalent compatible host stack).

If it does not enumerate as a usable sniffer, do not immediately erase/program it. First identify the USB device, COM port and existing firmware behavior. Only reprogram the dongle when required.

## Current Nordic host-side workflow

Nordic's current nRF Util workflow is:

```powershell
nrfutil install ble-sniffer
nrfutil ble-sniffer bootstrap
```

The first command installs the `ble-sniffer` nRF Util command and associated sniffer firmware files. The second installs the Wireshark external-capture integration (`extcap`).

After connecting the sniffer, a basic command-line capture can be tested with:

```powershell
nrfutil ble-sniffer sniff --port COMx
```

Replace `COMx` with the actual serial port. The command writes captured packets to a PCAP file in the current directory.

Wireshark should expose an interface similar to:

```text
nRF Sniffer for Bluetooth LE COMx
```

The nRF Sniffer toolbar can then select nearby advertising devices and follow a specific device.

## First-use procedure for this project

### A. Plug in only the sniffer

1. Leave the ProMicro application board disconnected at first if that makes COM-port identification easier.
2. Plug in the USB Packet Sniffer.
3. Check Windows Device Manager under Ports and USB devices.
4. Record the new COM port/device identity.
5. If no new serial/USB interface appears, record the exact device-manager status before changing firmware.

Useful checks:

```powershell
nrfutil --version
nrfutil search
```

Then, if needed:

```powershell
nrfutil install ble-sniffer
nrfutil ble-sniffer bootstrap
```

### B. Verify command-line sniffing

```powershell
nrfutil ble-sniffer sniff --port COMx
```

If packets are being received, stop the command after a short test and locate the generated PCAP file.

### C. Verify Wireshark

1. Open Wireshark.
2. Confirm an `nRF Sniffer for Bluetooth LE COMx` capture interface exists.
3. If the toolbar is hidden, enable `View -> Interface Toolbars -> nRF Sniffer for Bluetooth LE`.
4. Start capture.
5. Initially use the all-advertising-devices view.

### D. Power the `ble-t1` application board

Once Task 002 firmware is flashed and running, look for:

```text
ble-t1
```

Select it in the sniffer device list and confirm repeated legacy advertising traffic.

The Task 002 firmware also carries an easy-to-recognize test payload:

```text
FF FF 42 54 31 02
```

The bytes after `FF FF` contain ASCII `BT1` followed by experiment version `02`.

## Expected packet-level observations

For this first experiment, it is sufficient to observe some of the following:

```text
Advertising PDU
Scan Request
Scan Response
Complete Local Name: ble-t1
Manufacturer Specific Data: FF FF 42 54 31 02
```

The exact Wireshark PDU label can vary with advertising mode and dissector version. Do not treat the textual label alone as the acceptance criterion; the device name/address and payload are stronger identifiers.

## When firmware reprogramming may be needed

Nordic's current sniffer tooling expects a compatible Nordic device programmed with nRF Sniffer firmware. A seller-labeled `nRF52840 Packet Sniffer` may already contain such firmware, but clone hardware/firmware versions vary.

Reprogram only if one of these is true:

- the USB device is detected but current nRF Sniffer tooling cannot communicate with it;
- Wireshark/nrfutil reports a protocol/firmware mismatch;
- the seller firmware is clearly not nRF Sniffer firmware;
- local-agent inspection finds a documented compatible programming path for this exact dongle.

Before reprogramming, record the USB VID/PID, COM port, device-manager name, and any current behavior. Avoid guessing that the stick is electrically identical to Nordic PCA10059 solely because it uses nRF52840.

## Safety / project boundaries

- The sniffer is receive/analysis equipment for this project; it is not the application-board programmer.
- Do not connect it to the application's SWD pads.
- Do not erase the ProMicro UF2 bootloader during sniffer setup.
- Do not commit captures that contain unrelated nearby-device traffic.

## Official references

Nordic documentation used for this workflow:

- nRF Sniffer for Bluetooth LE overview
- nRF Util `ble-sniffer` plugin installation
- nRF Util `ble-sniffer sniff`
- Running nRF Sniffer in Wireshark
- Preparing/programming sniffer hardware

See also `docs/references.md` for the project's board/Nordic references.
