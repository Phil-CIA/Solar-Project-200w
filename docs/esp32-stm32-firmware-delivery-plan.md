# ESP32-to-STM32 Firmware Delivery Plan (Issue #24)

Status: draft plan, 2026-11-12. Nothing in this document is implemented or qualified yet. Tracks [#24](https://github.com/Phil-CIA/Solar-Project-200w/issues/24).

SWD (ST-LINK + OpenOCD) remains the programming and recovery method until every gate below is passed.

## Starting point

ESP32-C3 Wi-Fi application ([firmware/esp32_c3_wifi](../firmware/esp32_c3_wifi/README.md), DEC-068 in the [decisions log](decisions-log.md)):

- Bench-tested on Rev1:
  - It boots cleanly.
  - It joins the router (`192.168.0.98`, RSSI around -80 dBm).
  - Router retries never stop: the delay between attempts backs off from 1 s to 60 s.
  - Holding BOOT for 3 s opens the setup AP. An outage-opened setup AP closes itself once the router reconnects.
- Merged but not yet bench-tested:
  - The setup AP now uses WPA2 with a random password, logged on the console.
  - Re-provisioning through the setup AP.
  - `/clear`.
  - The 5-minute outage fallback.
  - The 10-minute AP auto-close.
- Not implemented: the STM32 data link, telemetry, and any firmware-update path.

STM32 bench firmware ([stage_aht20_display_uart.c](../firmware/bringup/rev1_g473_stages/stage_aht20_display_uart.c), build `u6q5q6-10s-esplog`) receives the ESP UART0 console on PB11 (USART3_RX). It relays the console to the PC UART with an `ESP> ` prefix. PB10 is left high-impedance.

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
| 0 | Close the remaining Wi-Fi bench tests: WPA2 setup AP, re-provisioning, `/clear`, 5-minute fallback, 10-minute auto-close. | Logged results on the COM5 relay. |
| 1 | Verify PA11/PA12 and PB10/PB11 alternate functions against the ST DS12288 PDF; confirm the AN2606 G47x USART3 pins; read the G473 flash size and page layout for the slot plan. | Facts recorded in this document. |
| 2 | Decide the console-versus-data-link sharing of ESP UART0 and STM32 USART3 (item 1 above). | Decision logged in the decisions log. |
| 3 | Bring up the framed link: STM32 USART3 TX enabled, ESP UART driver, ping/version/status frames, CRC error counters. | Thousands of frames with zero undetected errors; ESP reset mid-stream leaves the STM32 unaffected. |
| 4 | Define and document the image header, signing, and slot layout; choose the signature scheme (for example Ed25519 or ECDSA P-256) and key handling. | Design reviewed; protocol section added here. |
| 5 | STM32 bootloader plus installer, flashed by SWD; app split to link at the slot address. | Valid image installs and boots; SWD recovery documented and tested. |
| 6 | ESP upload page and transfer client. | End-to-end update on the bench, charger inhibited throughout. |
| 7 | Fault-injection tests from the issue: corrupted, incomplete, wrong-version, wrong-board, unsigned, and interrupted transfers, plus ESP reset, STM32 reset and power loss mid-transfer. | Every case rejected or recovered with an explicit status; the STM32 never boots an unaccepted image; charge control stays inhibited. |

## Open questions

- Does Rev1 have spare STM32 flash for dual slots, and what is the page size of the fitted G473 variant?
- Key storage: where is the public key held in STM32 flash, and is readout/write protection used?
- Should the setup AP be usable without a console (password label or QR code on the board) before field deployment?
- Should the ESP also get OTA for itself in the same image-delivery flow, and in which order?
