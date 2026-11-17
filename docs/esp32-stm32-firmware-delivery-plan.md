# ESP32-to-STM32 Firmware Delivery Plan (Issue #24)

Status: draft plan, 2026-11-12. The firmware-delivery path is not implemented or qualified yet. Tracks [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24).

SWD (ST-LINK + OpenOCD) remains the programming and recovery method until every gate below is passed.

## Starting point

ESP32-C3 Wi-Fi application ([firmware/esp32_c3_wifi](../firmware/esp32_c3_wifi/README.md), DEC-068 in the [decisions log](decisions-log.md)):

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

Rev-next action: route the data channel to MCU pins that have USART TX/RX. Confirm the result against the [bring-up checklist](test/bringup-checklist.md) UART step.

## Proposed architecture

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

These are candidates for the Rev2 PCB work tracked in the [Rev2 consolidation register](rev-two-consolidation.md), row "ESP32/STM32 link and firmware delivery" ([#11](https://github.com/Phil-CIA/Solar-Project-200w/issues/11), [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24)).

Every row needs a schematic and PCB decision before Rev2 freeze. "Evidence" says where each recommendation comes from.

| # | Recommendation | Evidence | Status |
|---|---|---|---|
| R1 | Route the ESP data UART to STM32 pins that have USART TX/RX: a spare USART pair other than USART1 PA9/PA10, which carries the PC console. STM32 GPIO may be rearranged to free a pair. Candidates to confirm against DS12288: USART2 on PA2/PA3 or PB3/PB4, UART4 on PC10/PC11, or LPUART1 on PA2/PA3. The ESP side can stay on GPIO0/GPIO1, because ESP UART1 is mapped through its GPIO matrix. Optional RTS/CTS must avoid ESP strapping pins GPIO2, GPIO8 and GPIO9. Use direction-explicit net names, for example `ESP_DATA_TX` / `ESP_DATA_RX`. | Rev1 PA11/PA12 have no USART TX/RX (secondary DS12288 source; plan step 1 confirms). The 2026-11-16 Rev2 KiCad file still routes `RXD_ESP`/`TXD_ESP` to PA11/PA12. That file is not the latest design: the user reports PA0–PA2 are now in use, so pin selection must use the current pin map. | Candidate; tracked in [#33](https://github.com/Phil-CIA/Solar-Project-200w/issues/33) |
| R2 | Give the ESP UART0 console (GPIO21/GPIO20) its own STM32 UART receive pin or a test header, separate from the data link, so boot logs stay visible across ESP resets. | Rev1 COM5 relay was essential to debug the ESP boot loop and the Wi-Fi retries. | Candidate |
| R3 | Bring out an accessible ESP setup button (GPIO9/BOOT, or a free GPIO) and ESP EN reset, reachable without opening the enclosure. | A 4 s BOOT hold opened the setup AP and the phone connected on 2026-11-12. | Candidate; enclosure accessibility remains a design decision |
| R4 | Keep the ESP antenna keepout at the board edge, away from the power stage and copper pours; decide on an external-antenna module option if the enclosure is metal. Qualify connectivity with nearby switching loads operating. | Earlier Rev1 bench RSSI was -79 to -86 dBm; on 2026-11-12 it was initially -77 dBm scanning and -75 dBm at association, then -89 dBm scanning during failed reconnect attempts. It reconnected after separate lights were switched off, but interference is not proven. On 2026-11-14 it scanned at -86 dBm and connected after retries; on 2026-11-15 connected reports ranged from -85 to -81 dBm. | Candidate; needs antenna orientation and RSSI at the installed location |
| R5 | Decide whether the STM32 controls ESP EN (reset) through an open-drain pin, so the STM32 can recover a hung ESP; optionally also control GPIO9 (BOOT), left high-impedance by default. Use any spare STM32 GPIO from the current pin map. The ESP must have no control of STM32 NRST/BOOT0, so it cannot bypass the STM32 installer; give PB8-BOOT0 a defined pull state. | Plan item 3 and the #24 safety constraints; Rev1 recovery required manual ESP resets. | Candidate; decide in plan step 2 |
| R6 | Keep the SWD header (SWDIO, SWCLK, NRST, GND, 3V3) accessible in the enclosure as the qualified recovery path. | #24 requires SWD recovery until the update flow is qualified. | Candidate |
| R7 | Show a way to read the setup-AP password without a console (status LED/OLED message, or a printed label if a fixed per-board password is chosen). Clearly explain the separate browser sign-in and make password entry and reset usable without weakening security. | The password is currently logged only on the console. A phone reported repeated wrong-password attempts with the random password but later joined using the console password; the cause of the earlier failures remains unknown. The browser sign-in prompt was initially mistaken for a page failure. | Candidate; firmware or hardware choice |
| R8 | Make setup and heartbeat indicators visible in the enclosure and explain the AP timeout and how to reopen it. Make both indicators diagnosable, for example with independent control or test pads at the first LED input and inter-LED link. | On 2026-11-12 the AP closed after 10 minutes before the phone connected. Rev1 chains D8 into D11; D11 flashed green but D8 was reported continuously green. The cause is not yet established. | Candidate; confirm indicator visibility, control and behavior |
| R9 | Fit series resistors (for example 100 Ω, value to be confirmed) and test pads on each ESP–STM32 signal, and keep each receiver idle-high with pull-ups. This limits current through I/O protection diodes if either side is unpowered or resetting, and avoids spurious frames during ESP boot. | The ESP is powered from `CTRL_5V` through its module regulator, while the STM32 uses `CTRL_3V3`, so the rails may not come up or go down together. Rev1 debugging required probing these nets. | Candidate |

## Open questions

- Does Rev1 have spare STM32 flash for dual slots, and what is the page size of the fitted G473 variant?
- Key storage: where is the public key held in STM32 flash, and is readout/write protection used?
- Should the setup AP be usable without a console (password label or QR code on the board) before field deployment?
- Should the ESP also get OTA for itself in the same image-delivery flow, and in which order?
