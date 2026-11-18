# ESP32-to-STM32 Firmware Delivery Plan (Issue #24)

Status: draft plan, 2026-11-12. The firmware-delivery path is not implemented or qualified yet. Tracks [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24).

SWD (ST-LINK + OpenOCD) remains the programming and recovery method until every gate below is passed.

## Starting point

ESP32-C3 Wi-Fi application ([firmware/esp32_c3_wifi](../firmware/esp32_c3_wifi/README.md), DEC-069 in the [decisions log](decisions-log.md)):

- Bench-tested on Rev1:
  - The merged image booted and joined the saved router.
  - Retry delays of 1, 2, 4 and 8 seconds were observed on the earlier image.
  - A 4-second BOOT hold opened the WPA2 setup AP; a phone connected and loaded the page.
  - The button-opened AP closed after 10 minutes while the router was connected, with no phone client connected.
  - On the authentication-fix image, missing, wrong and malformed GET credentials and missing or wrong `/save` credentials were rejected. A phone signed in using a temporary test password.
  - On the later status image, the user reported that the phone joined the setup AP using the password shown on the console.
- Not yet bench-tested:
  - Browser sign-in with the normal random setup password, and unauthorized `/clear` rejection.
  - Re-provisioning through the setup AP.
  - `/clear`.
  - The 5-minute outage fallback.
  - The 60-second retry ceiling and sustained indefinite retry.
- The running ESP application emits one-way Wi-Fi status records; connected reports were observed after a PC-over-USB app-only flash. The flashed STM32 status image displayed an RSSI value on the OLED and D11 was observed flashing green; D11 timing and other status patterns remain untested.
- Not implemented: the framed, bidirectional STM32 data link or an ESP-to-STM32 firmware-update path.

STM32 bench firmware ([stage_aht20_display_uart.c](../firmware/bringup/rev1_g473_stages/stage_aht20_display_uart.c), running build `u6q5q6-10s-wifi1`) receives the ESP UART0 console on PB11 (USART3_RX). It relays the console to the PC UART with an `ESP> ` prefix and consumes status records from that relay. PB10 is left high-impedance.

## Rev1 ESP32–STM32 wiring and finding

| ESP32-C3 | Net | STM32G473 | STM32 UART capability |
|---|---|---|---|
| GPIO21 (U0TXD) | `RTS_ESP` | PB11 | USART3_RX (AF7) |
| GPIO20 (U0RXD) | `CTS_ESP` | PB10 | USART3_TX (AF7) |
| GPIO0 | `RXD_ESP` | PA11 | No USART TX/RX: USART1_CTS (AF7), FDCAN1_RX, USB_DM |
| GPIO1 | `TXD_ESP` | PA12 | No USART TX/RX: USART1_RTS_DE (AF7), FDCAN1_TX, USB_DP |

The architecture planned a hardware UART data channel on `TXD_ESP`/`RXD_ESP` ([wifi-coprocessor-architecture.md](wifi-coprocessor-architecture.md)). The Rev1 MCU pins cannot provide one, according to the DS12288 alternate-function table. That table still needs a direct check against the ST PDF; the mapping above came from a secondary copy of DS12288.

The usable Rev1 hardware UART is the "flow control" pair, PB10/PB11 = USART3. That pair is currently shared with the ESP UART0 console.

Rev2 now uses a separate USART2 pair, as documented in the [schematic-only handoff below](#rev2-schematic-only-handoff-issue-33). The official ST package/AF data verifies the PA11/PA12 limitation without relying on the earlier secondary DS12288 citation. Rev1 wiring and firmware are unchanged.

## Proposed architecture

The transport-sharing alternatives below describe the Rev1 constraint, not the selected Rev2 wiring. Rev2 retains UART0/USART3 exclusively for the console and adds UART1/USART2 for data. The installer, protocol and update qualification remain future work.

1. **Transport.** On Rev1, use USART3 (PB10 TX / PB11 RX) to ESP GPIO20/GPIO21 at 115200 baud. The ESP data path then needs its own UART:
   - Either move the ESP console off UART0 (USB-only), losing the COM5 boot-log relay.
   - Or carry the console inside the framed protocol as a log message type.

   Decide this before writing the link code.
2. **Framing.** Use a versioned frame: sync, type, sequence, length, payload, CRC-32. This is the [DEC-015](decisions-log.md) protocol extended with update message types: `UPD_BEGIN`, `UPD_CHUNK`, `UPD_END`, `UPD_STATUS`, `UPD_ABORT`.
3. **STM32 installer, not the ROM bootloader.** A small resident STM32 bootloader in the first flash pages validates and installs images. It also keeps the application slot bootable or recoverable. The ROM system bootloader (AN3155) is not proposed as the field path:
   - Entering it needs BOOT0 or option-byte control that the ESP does not have on Rev1.
   - It cannot check authenticity or roll back.
   - Whether it supports USART3 on PB10/PB11 is unconfirmed (check AN2606).

   It may still be used as a bench tool.
4. **Image format.** Use a header carrying:
   - magic
   - header version
   - board/hardware ID
   - firmware version and minimum accepted version
   - image size
   - SHA-256
   - signature

   The STM32 checks the header before erasing anything, and checks the full image before marking it bootable.
5. **Slots and recovery.** If flash allows, use dual slots: a staging slot, then copy or swap, with a boot-confirm flag. Otherwise use a staging slot plus a bootloader-only recovery mode that waits for a new image. An interrupted transfer must leave the previous image bootable or the bootloader waiting. In either case, the charger stays inhibited.
6. **Safety gating.** The STM32 accepts `UPD_BEGIN` only when:
   - charging is disabled;
   - U6 is inhibited and the output path (PA8/U14) is off;
   - no bench test is armed or running.

   The STM32 latches this inhibit for the whole update and recovery. The ESP cannot override it.
7. **Image delivery to the ESP.** The first stage is local only: an upload page on the ESP's LAN address, protected by a password, that streams to the STM32. Remote or cloud delivery stays out of scope until authentication is designed.

## Plan

| Step | Work | Exit check |
|---|---|---|
| 0 | Close the remaining Wi-Fi bench tests: portal authentication, re-provisioning, `/clear`, and 5-minute fallback. | Logged results on the COM5 relay, including rejected unauthenticated requests. |
| 1 | Verify PA11/PA12 and PB10/PB11 alternate functions against the ST DS12288 PDF; confirm the AN2606 G47x USART3 pins; read the G473 flash size and page layout for the slot plan. | Facts recorded in this document. |
| 2 | Decide the console-versus-data-link sharing of ESP UART0 and STM32 USART3 (item 1 above). | Decision logged in the decisions log. |
| 3 | Bring up the framed link: STM32 USART3 TX enabled, ESP UART driver, ping/version/status frames, CRC error counters. | Thousands of frames with zero undetected errors; ESP reset mid-stream leaves the STM32 unaffected. |
| 4 | Define and document the image header, signing, and slot layout; choose the signature scheme (for example Ed25519 or ECDSA P-256) and key handling. | Design reviewed; protocol section added here. |
| 5 | STM32 bootloader plus installer, flashed by SWD; app split to link at the slot address. | Valid image installs and boots; SWD recovery documented and tested. |
| 6 | ESP upload page and transfer client. | End-to-end update on the bench, charger inhibited throughout. |
| 7 | Fault-injection tests from the issue: corrupted, incomplete, wrong-version, wrong-board, unsigned, and interrupted transfers, plus ESP reset, STM32 reset and power loss mid-transfer. | Every case rejected or recovered with an explicit status; the STM32 never boots an unaccepted image; charge control stays inhibited. |

## Step 0 bench results

Record each Wi-Fi test here, with the date, the ESP image commit, and the COM5 capture file. Each result should also say whether it changes a Rev2 recommendation below.

2026-11-12: flashed the merged app built from commit `57ec9b0` (reported ELF SHA-256 prefix `f00ef85a8`). Evidence is in the session-local COM5 captures `com5-step0-20261112-050933.txt` and `com5-step0-retry-20261112-053038.txt`; they contain the setup-AP password and must not be committed.

The subsequent authentication-fix image (uncommitted source, ELF SHA-256 prefix `8cd7be23d`) was flashed without erasing NVS and rejoined the saved router. Its session-local capture is `com5-auth-20261112-080618.txt`.

A temporary bench-only copy of that image (ELF SHA-256 prefix `127b1e7a6`) used a simple fixed test password instead of the normal random password. The ESP reported that the configured AP password matched, and the user reported that the phone connected and passed the browser sign-in. The COM5 capture disconnected during the browser attempt, so it did not independently record the phone's connection. With approval, the normal image was restored with an app-only flash and booted; it later reconnected to the router. Relevant session-local captures include `com5-simple-ap-20261112-094548.txt`, `com5-normal-restore-20261112-102216.txt`, and `com5-lights-test-20261112-102946.txt`; they must not be committed.

2026-11-14: after the board had been left idle, the user confirmed the safe state and reset the ESP. The normal image (`8cd7be23d`) booted, saw the router at -86 dBm, and connected on attempt 5 after authentication-expiry and connection-failure events. With the separate LEDs on, the phone forgot the old setup network, but reported an incorrect password for the newly opened setup AP; the ESP logged repeated reason 15 handshake timeouts. Evidence is in session-local `com5-long-boot-20261114-072529.txt`, `com5-random-phone-20261114-072829.txt`, and `com5-random-phone-join-20261114-072942.txt`; the cause of the failures is not established, and the captures must not be committed.

2026-11-15: with the bench supply off and the ESP connected by USB, the user approved an app-only flash. The helper identified the expected ESP32-C3, matched the installed partition table to the build, and verified the application write. A subsequent passive COM5 capture (`com5-wifi-status-20261115-044009.txt`) recorded 35 connected status reports with RSSI from -85 to -81 dBm. It did not capture boot or exercise the other status states; the STM32 status display and LEDs remain untested. The capture is session-local and must not be committed.

Later on 2026-11-15, the user approved the STM32 flash and confirmed the fitted MCU as STM32G473. ST-LINK reported 3.25 V at the target, an STM32G47/G48 device, and 512 KiB flash. OpenOCD verified the `u6q5q6-10s-wifi1` image (ELF SHA-256 prefix `29823bca`); the live COM5 capture showed its BUILD line and connected status reports at -83 and -84 dBm. No capture file was saved. The user subsequently observed -87 on the OLED RSSI line; this was not compared with a simultaneous COM5 reading. The user reported D11 flashing green and D8 continuously green over a 30-second observation, rather than the expected red heartbeat. The D8 discrepancy is unexplained and its diagnosis was deferred at the user's request; the Wi-Fi state was not checked simultaneously with the LED observations.

The phone later reported "Failed to Join" when using a previously saved setup password. Because the password is regenerated after an ESP restart, it may have been stale. The user then found the setup password on the console, reported that the phone joined the AP on the normal ESP status image (application binary SHA-256 prefix `8c248013`), and reached the browser sign-in prompt at `192.168.4.1`. No COM5 capture was made for this step; browser sign-in with the normal random password remains to be verified.

A separate session-local bench image with a weak fixed website-only password was built (application SHA-256 prefix `cb5e647f`). The setup Wi-Fi password remains random. Offline tests accepted the requested browser credentials and rejected missing, wrong, truncated and unreadable credentials. After the user confirmed safe idle and approved flashing, the app-only helper matched the ESP identity and partition table, then wrote and verified the image. The user confirmed the `BENCH-ADMIN-1` runtime marker after holding BOOT, connected the phone using the new setup Wi-Fi password, and reported that the setup page opened after the temporary website sign-in. No COM5 capture was made for this step. This does not verify browser sign-in with the normal random password. The weak website login is also exposed to the router network while setup is open; restoring the normal image afterward requires separate approval.

| Test | Result | Notes / Rev2 impact |
|---|---|---|
| Boot on the merged image; setup-AP password logged on COM5 | Pass | Booted after flashing; password appeared when BOOT opened the AP. Supports R2 and R7. |
| Router joins with the existing `wifi_cfg` credentials (no re-provisioning) | Pass | Connected on the first attempt, without re-provisioning. |
| 3 s BOOT hold opens the setup AP; phone joins with the logged password | Pass | A 4 s hold opened the AP; on retry the phone connected and obtained a DHCP address. Wrong-password rejection is untested. Supports R3. |
| Setup page opens on the phone | Pass | User confirmed the page appeared at `192.168.4.1`. |
| Unauthenticated access from router network | Partial | On the earlier image, a workstation loaded the setup page without a portal password. On the authentication-fix image, GET with missing, wrong or malformed credentials and POST `/save` with missing or wrong credentials returned 401; the POST used an invalid empty form. Correct phone login passed with the temporary test password; rejected `/clear` remains untested. |
| Phone connection and browser sign-in | Partial | With the normal random password, the phone reported a wrong password and the ESP logged reason 15 handshake timeouts, including on 2026-11-14 after forgetting the saved setup network. With a temporary fixed test password, the user reported that the phone connected and the signed-in setup form appeared; the COM5 capture missed that connection. On the later normal status image, the user reported joining with the console password; browser sign-in with that password remains untested. On a subsequent bench image with random Wi-Fi and a temporary website-only password, the user reported that the phone joined and the signed-in setup page opened. The cause of the earlier failures is unknown. Supports R7. |
| Router reconnect after the temporary test | Partial | The restored normal image booted and found the router at -89 dBm, but logged authentication expiry and connection failures, as it had just before restoration. After the user switched off separate lights (reported as about 150 rapidly switching LEDs), attempt 13 connected and the setup AP closed. This single observation does not establish that the lights caused the failures. Supports R4. |
| STM32 status image and connected relay | Partial | Flash verification and the new BUILD line passed; connected reports appeared on COM5. D8 is reportedly solid green rather than blinking red, and D11 flashes green; the Wi-Fi state was not checked simultaneously. Other Wi-Fi states remain to be checked. Supports R2; R8 awaits indicator validation. |
| Connected RSSI on OLED | Pass (initial) | User observed -87; a simultaneous COM5 comparison and other Wi-Fi states remain untested. |
| Re-provisioning through the setup AP; ESP joins the router | Pending | |
| `/clear`, then setup AP from the no-credentials state | Pending | |
| Router off for 5 min opens the setup AP; router back on closes it | Pending | |
| BOOT-opened AP closes after 10 min while connected | Pass | AP closed 600.020 s after the open log; the router had connected and no station disconnect was logged. No phone client joined. Supports R8. |
| RSSI at the bench location (module antenna orientation noted) | Partial | Scan: -77 dBm; association: -75 dBm. Antenna orientation and installed-enclosure performance not recorded. Supports R4. |

## Rev2 PCB recommendations

Tracked in the [Rev2 consolidation register](rev-two-consolidation.md), [#33](https://github.com/Phil-CIA/Solar-Project-200w/issues/33), [#11](https://github.com/Phil-CIA/Solar-Project-200w/issues/11) and [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24). The 2026-11-17 decisions below supersede the earlier candidate pin/EN recommendations. Schematic implementation is not PCB or hardware qualification.

Every row needs a schematic and PCB decision before Rev2 freeze. "Evidence" says where each recommendation comes from.

| # | Recommendation | Evidence | Status |
|---|---|---|---|
| R1 | Two-wire data UART only: STM32 USART2 AF7, PB3/pad41 TX and PB4/pad42 RX; ESP GPIO0/pad9 RX and GPIO1/pad10 TX. Move SW2/SW4 to PA11/PA12. No RTS/CTS, PA0-PA3 allocation, analog/safety pin move or PC-console change. | Confirmed G473CEU6 UFQFPN48; official ST package/AF XML, current netlist and user-approved SWD-only decision. | Implemented in schematic; PCB/firmware deferred |
| R2 | Retain GPIO21 to PB11 RX and GPIO20 to PB10 TX as a separate buffered UART0/USART3 console pair. STM32 console TX must remain high-impedance until firmware intentionally uses it. | Rev1 boot-log relay retained logically; no data frames on the console pair. | Implemented in schematic; PCB/firmware deferred |
| R3 | Retain the selected SuperMini's onboard RST and GPIO9/BOOT buttons; no EN header pin or extra reset-pad wire. Preserve manual recovery access. | User product photo shows both buttons; user confirmed direct fit in the existing Rev1 footprint. | Manual-only reset selected; enclosure reachability deferred |
| R4 | Keep the ESP antenna keepout at the board edge, away from the power stage and copper pours; decide on an external-antenna module option if the enclosure is metal. Qualify connectivity with nearby switching loads operating. | Earlier Rev1 bench RSSI was -79 to -86 dBm; on 2026-11-12 it was initially -77 dBm scanning and -75 dBm at association, then -89 dBm scanning during failed reconnect attempts. It reconnected after separate lights were switched off, but interference is not proven. On 2026-11-14 it scanned at -86 dBm and connected after retries; on 2026-11-15 connected reports ranged from -85 to -81 dBm. | Candidate; needs antenna orientation and RSSI at the installed location |
| R5 | Manual ESP reset/BOOT only; automatic STM32-controlled EN recovery is explicitly deferred. ESP has no connection to STM32 NRST/BOOT0; retain R86's BOOT0 pull-down. | User superseded the earlier controlled-EN preference after confirming the module has no EN header pin. | Decision complete; no automatic recovery claimed |
| R6 | Add J9 SWDIO/SWCLK/NRST/GND/VTref access. No full JTAG or SWO: PB3/PB4 are allocated to USART2. | User selected SWD-only; #24 retains SWD recovery. | Schematic/footprint assigned; keyed cable/enclosure fit and PCB deferred; #17 remains open |
| R7 | Show a way to read the setup-AP password without a console (status LED/OLED message, or a printed label if a fixed per-board password is chosen). Clearly explain the separate browser sign-in and make password entry and reset usable without weakening security. | The password is currently logged only on the console. A phone reported repeated wrong-password attempts with the random password but later joined using the console password; the cause of the earlier failures remains unknown. The browser sign-in prompt was initially mistaken for a page failure. | Candidate; firmware or hardware choice |
| R8 | Retain the D8/D11 chain; add TP27 at the first input and TP28 between the LEDs so each stage can be diagnosed separately without another GPIO. | D8's Rev1 steady-green cause remains unknown; test access does not prove timing or LED health. | Schematic implemented; PCB visibility/access and diagnostic firmware deferred |
| R9 | U15 TXU0204PWR provides power-off-safe buffering for all four wires, with R87-R90 = 100 Ohm at buffer outputs, R91-R98 = 10 kOhm local-domain pull-ups, C62/C63 = 100 nF, and TP19-TP26 at both endpoint sets. Keep module 3V3 separate from CTRL_3V3. | TI SCES936A pinout, Ioff and VCC-isolation specifications. Series resistors alone are not powered-off isolation or proof against false frames. User approved the buffer topology. | Schematic/footprints implemented; PCB and powered qualification deferred |

## Rev2 schematic-only handoff (issue #33)

### Source and boundary

The current schematic on main `80a6a0d` was the electrical authority. Recovery commit `f6ed6fb` selectively ports saved layout from `f0ccffb` onto that lineage, retaining #22: D4 absent, D6.1/U4.2=`CTRL_SUPPLY_IN`, D6.2=`CHG_OUT_POS`, D7.1=`CTRL_5V`, D7.2=`VBUS`. It does not import the saved schematic or revert the control supply.

The recovered PCB is frozen at Git blob **`97bad31abc292edf088c00e9710441cf21ff1331`**. Subsequent work changes only the Rev2 schematic and its footprint/library assignments. No Update PCB from Schematic, placement, routing, keepout edit or board-saving zone refill follows recovery. Rev1 and firmware are unchanged.

### Pin and component map

`ESP_*_TX/RX` names are from the ESP perspective; `_STM` identifies the STM32-side electrical segment. After moving the interface onto page 1, each buffer output is wired directly to its series resistor; these local nets use KiCad's `Net-(U15-*)` names. Global labels on the receiver sides must match the endpoint names (`ESP_DATA_RX`, `ESP_CONSOLE_RX`, `ESP_DATA_TX_STM`, `ESP_CONSOLE_TX_STM`), without `_BUF`. R96 biases `ESP_CONSOLE_RX`, not ESP GPIO1/data TX.

| Function | Transmitter | Buffer path | Receiver | Endpoint test pads |
|---|---|---|---|---|
| Data to ESP | U1 PB3/pad41, USART2_TX AF7 | U15 A1/2 -> B1Y/13 -> R87 | U2 GPIO0/pad9 | TP19 / TP23 |
| Data to STM32 | U2 GPIO1/pad10 | U15 B3/11 -> A3Y/4 -> R89 | U1 PB4/pad42, USART2_RX AF7 | TP21 / TP25 |
| Console to ESP | U1 PB10/pad22, USART3_TX AF7 | U15 A2/3 -> B2Y/12 -> R88 | U2 GPIO20/pad7, U0RXD | TP20 / TP24 |
| Console to STM32 | U2 GPIO21/pad8, U0TXD | U15 B4/10 -> A4Y/5 -> R90 | U1 PB11/pad24, USART3_RX AF7 | TP22 / TP26 |

SW2 `BTN_DN_N` moves from PB3 to PA11/pad33; SW4 `BTN_LT_N` moves from PB4 to PA12/pad34. USART1 PA9/PA10, SWD PA13/PA14 and all other sensing/control/fault assignments are unchanged. PA0/PA1 are sensing inputs. PA2 is no-connect in the authoritative exported schematic despite issue #33's occupied-pin note; PA2/PA3 are not consumed or advertised as spare. BMS pin allocation remains its separate unresolved design task.

U1 is explicitly STM32G473CEU6 with all 48 numbered package pins plus exposed VSS pad49 checked. Its existing `Package_DFN_QFN:QFN-48-1EP_7x7mm_P0.5mm_EP5.6x5.6mm` assignment is retained. The corrected MCU, 16-pin SuperMini and TXU0204 symbols live in the separate portable `Rev2_Interfaces` library; existing Rev1 library definitions are not replaced.

U2 remains the user's 16-pin SuperMini, not the falsely attributed Seeed C6 part. The user supplied [ASIN B0D47G24W3](https://www.amazon.com/dp/B0D47G24W3), a product image, and confirmed direct physical fit in the existing Rev1 footprint without adapters or pin modifications. The retained footprint/pad ordering is therefore used without inventing a new module outline. The image does not establish a regulator schematic or quantified antenna clearance.

J9 uses `Connector_PinHeader_1.27mm:PinHeader_2x05_P1.27mm_Vertical`: 1=VTref/CTRL_3V3, 2=SWDIO, 3/5/9=GND, 4=SWCLK, 10=STM_NRST. Pins 6/7/8 are no-connect (no SWO/key contact/TDI). This generic unshrouded header is not a qualified keyed IDC connector; verify pin-1 orientation, cable keying and enclosure access under #17.

### Rail/reset states and component rationale

U2.16 remains `CTRL_5V`; U2.14 is regulator-output domain `ESP_3V3_U2`, **not** joined to `CTRL_3V3`. U15 VCCA/1 and OE/8 use CTRL_3V3; VCCB/14 uses ESP_3V3_U2; GND/7 uses PWR_NEG; pins6/9 are NC. This is powered-off isolation, not galvanic isolation or control-supply qualification.

| Condition | Intended electrical behavior / remaining qualification |
|---|---|
| Both 3V3 domains valid | Four fixed-direction channels enabled. Local pull-ups hold undriven inputs/receivers idle-high; UART0 boot logs remain console traffic. |
| STM32 rail absent | TI VCC-isolation/Ioff disables outputs; ESP-side bias comes only from ESP_3V3_U2. No pull bridges the supply domains. |
| ESP regulator output absent | Same isolation; STM32-side bias comes only from CTRL_3V3. Do not bypass U15 with external test wiring. |
| Rail ramps / USB-only / power-down | TI permits arbitrary supply sequencing; below 100 mV or disconnected on either supply all outputs are high-impedance. The intermediate rail transition is not a valid data interval: firmware must discard partial frames and re-establish a valid link. #22's USB-only power path remains unqualified. |
| MCU reset / watchdog | PB10 must remain high-impedance until intentionally configured. Hardware pulls make its buffer input idle-high; no automatic ESP reset is implemented. |
| ESP RST / BOOT | On-module buttons only, existing module strap circuitry retained. Console reboot text cannot be interpreted as a data command. ESP cannot drive STM_NRST or STM32 BOOT0. |

The 100 Ohm resistors are output edge damping, **not** the Ioff mechanism. At a provisional 50 pF total load, their ideal RC contribution is 5 ns (about 11 ns 10-90%), versus 8.68 us per bit at 115200 baud. Actual trace, pad, probe and receiver capacitances plus buffer propagation/edges must be checked after layout. Each 10 kOhm pull draws about 0.33 mA when low at 3.3 V; even 2.5 uA leakage causes only 25 mV drop. Check actual device thresholds, low-level margin and rail tolerances during qualification rather than treating nominal calculations as measurements. C62/C63 provide local 100 nF bypassing.

TP27 observes U1.PB2 -> D8.DIN; TP28 observes D8.DOUT -> D11.DIN. Future diagnostic firmware must send distinct patterns to the two LEDs and compare those two points before attributing a fault to firmware or an LED. The existing Rev1 D8 observation is not explained by this change.

### Evidence and checks

- ST primary package data: [STM32G473C(B-C-E)Ux.xml](https://github.com/STMicroelectronics/STM32_open_pin_data/blob/45c04599e2e0a788c5ed9b6218dc5a28a36041f7/mcu/STM32G473C%28B-C-E%29Ux.xml); AF data: [GPIO-STM32G47x_gpio_v1_0_Modes.xml](https://github.com/STMicroelectronics/STM32_open_pin_data/blob/45c04599e2e0a788c5ed9b6218dc5a28a36041f7/mcu/IP/GPIO-STM32G47x_gpio_v1_0_Modes.xml), both pinned to commit `45c04599`. These verify USART2 PB3/PB4 and USART3 PB10/PB11 AF7. Direct ST PDF retrieval failed; this does not claim verification of the earlier DS12288 reference. G473 search results identify DS12712, so reconcile the document number before later PDF-specific claims.
- TI `txu0204`, **SCES936A (May 2021, revised September 2021)**: [vendor PDF](https://www.ti.com/lit/ds/symlink/txu0204.pdf), PW pin diagram/pin functions, electrical characteristics, VCC isolation and power-supply recommendations. PDF downloaded and inspected in session storage; not vendored, and the datasheet manifest was not changed. ST XML was also downloaded; no ST PDF was obtained.
- KiCad **10.0.6**: final exported netlist verifies all four directed channels, 16 exact signal/LED/button nets, local supply/bias endpoints, all 49 MCU pads, unchanged safety nets and added footprint pad coverage. Final ERC: **52 inherited warnings, zero errors**, same category/count distribution as the pre-edit schematic.
- Read-only PCB DRC remains **62 violations / 347 unconnected items** from recovery. Final schematic parity deliberately rises **4 -> 53**: 28 missing footprints (26 added here plus inherited JP1/JP2), 20 pad-net conflicts, 3 value mismatches and 2 datasheet-field differences. These are not waived fabrication checks. The board still carries legacy U1/U2 metadata.

### Later layout and firmware work; issue remains open

1. Review and apply the schematic-to-PCB update in a separate layout step: add U15, R87-R98, C62/C63, TP19-TP28 and J9; reconcile U1/U2 metadata, button/UART reassignment, ESP regulator-output net, LED-chain labels and STM_NRST. Preserve #22 and the recovered shunt/Kelvin layout.
2. Resolve inherited JP1/JP2, feedback-net and U10-value parity. Fix the saved-source short between NT8.1 (`Net-(J3-Pin_2)`) and the `I_BATT_-` track, UUID `4dc76fe0-a257-4ab2-b935-738aea01b59e`, near (146.32, 130.60) mm; see [recovery record](../hardware/kicad/solar-project/REVISION-INDEX.md). Do not manufacture the frozen recovery board.
3. Place bypass capacitors at both U15 supplies; place series parts at buffer outputs and make endpoint pads probeable with a nearby ground. Keep UART/test stubs away from switching and sense loops. Verify UART load/edges before increasing baud.
4. Provide physical RST/BOOT, SWD and independently identifiable LED access. Reserve antenna clearance at the board edge away from copper, switching loops and metal; **exact all-layer keepout dimensions remain blocked on verified guidance for the fitted module**, not inferred from its product image. Do not silently substitute another SuperMini variant.
5. Before closing #33: zero schematic/PCB parity issues, zero changed-net unconnected items, no unresolved interface ERC/DRC violations, and review/disposition relevant inherited findings. Antenna/service-access checks need the actual PCB/enclosure, not just schematic labels.
6. Firmware handoff only: configure USART2 AF7 PB3/PB4 and ESP UART1 GPIO1/GPIO0 at the initial 115200 baud; move button definitions to PA11/PA12; retain separate USART3 console with PB10 high-impedance until intentional use. Validate framing/link recovery and STM32 command authorization after resets before allowing remote requests. No protocol/reset firmware, flashing or powered tests are included here.

## Open questions

- Does Rev1 have spare STM32 flash for dual slots, and what is the page size of the fitted G473 variant?
- Key storage: where is the public key held in STM32 flash, and is readout/write protection used?
- Should the setup AP be usable without a console (password label or QR code on the board) before field deployment?
- Should the ESP also get OTA for itself in the same image-delivery flow, and in which order?
