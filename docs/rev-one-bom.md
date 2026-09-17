# Solar Project 200W - Rev 1 Bill of Materials (BOM)

**Project:** Solar Project 200W MPPT & Battery-First Power Controller  
**Hardware Revision:** Rev 1.0  
**Status:** Synchronized with `Solar Project Rev1.kicad_sch` and `Solar Project Rev1.net`; C7, C52, and D2 catalog numbers confirmed 2026-09-17 (DEC-058) - C7=YAGEO CC1206KKX7R0BB225 (`C577211`, 2.2uF/100V/X7R/1206), C52=Samsung CL21B224KBFNNNE (`C5378`, 220nF/50V/X7R/0805), D2=Littelfuse SMBJ48CA (`C151264`, Vrwm=48V/VC=77.4V@Ipp=7.8A/DO-214AA) - all three datasheet-verified as exact matches to the DEC-055/056/057 targets. New connector `J8` added 2026-09-17 (DEC-061, Track C battery temperature sensing) - remote I2C sensor part number still open.  
**Total Component Count:** 173 Parts (161 with verified LCSC / JLCPCB Part Numbers, 3 newly split/added TBD)  

---

## Consolidated Rev 1 Component Table

| Item | Designator | Qty | Value | Package / Footprint | LCSC Part # | Description |
| :---: | :--- | :---: | :--- | :--- | :--- | :--- |
| 1 | `10mm1, 10mm2` | 2 |  | `10mm LOGO` | — |  |
| 2 | `C1, C8, C12, C22, C28, C33, C44, C45, C46, C47, C48, C51, C54, C57` | 14 | 100nF | `C_0805` | **C28233** | 100V 100nF X7R ±10% 0805 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 2a | `C52` | 1 | 220nF, 50V, X7R | `C_0805` | **C5378** | Samsung Electro-Mechanics CL21B224KBFNNNE. Bump from 100nF - LM74502H's own `CVCAP>=10x Ciss` guidance needs ~106nF at Q5/Q6's worst-case (max) `Ciss` for the shared gate load; 100nF clears typ-Ciss with ~22% headroom but falls ~6% short of max-Ciss (DEC-042). Voltage rating comfortable (chip-side VCAP-VS abs max is 15V) - confirmed 2026-09-17 (DEC-058) |
| 3 | `C2, C55, C56` | 3 | 1uF | `C_0805` | **C28323** | 50V 1uF X7R ±10% 0805 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 4 | `C3` | 1 | 100uF | `CP_Radial_D6.3mm_P2.50mm` | **C49232871** | Unpolarized capacitor |
| 5 | `C15, C34, C35` | 3 | 2.2uF | `C_1206` | **C50254** | 50V 2.2uF X7R ±10% 1206 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 5a | `C7` | 1 | 2.2uF, 100V, X7R | `C_1206` | **C577211** | YAGEO CC1206KKX7R0BB225. 100V-rated replacement for the 50V `C50254` part - C7 sits on `PV_IN_POS`, whose real operating voltage (up to 57V at STC under 2S, or 28.5V/~34V cold under the DEC-051 2P baseline) exceeds the 50V rating fitted here (DEC-049). Split out of the shared 50V line; confirmed 2026-09-17 (DEC-058) |
| 6 | `C9, C13` | 2 | 2.2nF | `C_0805` | **C28260** | 50V 2.2nF C0G ±5% 0805 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 7 | `C10` | 1 | 47pF | `C_0805` | **C14857** | 50V 47pF C0G ±5% 0805 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 8 | `C14, C53` | 2 | 100uF | `C_1206` | **C15008** | 6.3V 100uF X5R ±20% 1206 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 9 | `C18` | 1 | 560uF | `CP_Elec_10x12.6` | **C264031** | Unpolarized capacitor |
| 10 | `C19, C21, C30` | 3 | 47uF | `C_Radial_D8.0mm_H7.0mm_P3.50mm` | **C44606514** | Unpolarized capacitor |
| 11 | `C20` | 1 | 180pF | `C_0805` | **C1794** | Unpolarized capacitor |
| 12 | `C23` | 1 | 100nF | `C_0805` | **C28233** | Unpolarized capacitor |
| 13 | `C24` | 1 | 22nF | `C_0805` | **C1710** | Unpolarized capacitor |
| 14 | `C25` | 1 | 1.5nF | `C_0805` | **C1590** | Unpolarized capacitor |
| 15 | `C26` | 1 | 33nF | `C_0805` | **C1738** | Unpolarized capacitor |
| 16 | `C27, C29` | 2 | 22uF | `C_0805` | **C45783** | 25V 22uF X5R ±20% 0805 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 17 | `C31` | 1 | 47uF | `CP_Radial_D8.0mm_P3.50mm` | **C44606514** | Unpolarized capacitor |
| 18 | `C49, C50` | 2 | 33pF | `C_0805` | **C1798** | 50V 33pF NP0 ±5% 0805 Multilayer Ceramic Capacitors MLCC - SMD/SMT ROHS |
| 19 | `D1, D5` | 2 | 1SMA4742A | `D_DO-214AC(SMA)` | **C19077480** | Independent Type 11.4V~12.6V 1W 12V SMA(DO-214AC) Zener Diodes ROHS |
| 20 | `D2` | 1 | SMBJ48CA (replaces `SMBJ60CA`) | `D_SMB` | **C151264** | Littelfuse SMBJ48CA. 100V-VDS-margin-corrected TVS: `Vrwm`=48V, `Vbr`=53.3-58.9V, `VC`(clamp)=77.4V @ `Ipp`=7.8A, DO-214AA - replaces the installed `SMBJ60CA` (`Vrwm`=60V, `VC`=96.8V @ `Ipp`=6.20A, only ~3.2% clamp margin vs Q4's 100V VDS) for more comfortable clamp-voltage margin; confirmed 2026-09-17 (DEC-058) |
| 21 | `D3` | 1 | USBLC6-2SC6 | `SOT-23-6` | **C7519** | Very low capacitance ESD protection diode, 2 data-line, SOT-23-6 |
| 22 | `D4, D6, D7, D10` | 4 | 1N5819WS | `D_SOD-323` | **C191023** | 40V 600mV@1A 1A SOD-323 Schottky Diodes ROHS |
| 23 | `D8` | 1 | WS2812B | `LED-SMD_4P-L1.6-W1.5_XL-1615RGBC-WS2812B` | **C5349954** | 1615 RGB LEDs(Built-in IC) ROHS |
| 24 | `D9` | 1 | Green | `D_0805` | **C2297** | Emerald 0805 LED Indication - Discrete ROHS |
| 25 | `D12` | 1 | Yellow | `D_0805` | **C2296** | Yellow 0805 LED Indication - Discrete ROHS |
| 26 | `F1, F2` | 2 | Fuse | `Fuseholder_Littelfuse_100_series_5x25mm` | **C142864** | Fuse |
| 27 | `J1, J3` | 2 | Screw_Terminal_01x02 | `TerminalBlock_Phoenix_MKDS-1,5-2-5.08_1x02_P5.08mm_Horizontal` | **C8439** | Generic screw terminal, single row, 01x02, script generated (kicad-library-utils/schlib/autogen/connector/) |
| 28 | `J2` | 1 | USB4105-GF-A | `GCT_USB4105-GF-A` | **C3020560** |  |
| 29 | `J4` | 1 | Conn_01x04 | `PinHeader_1x04_P2.54mm_Vertical_SMD_Pin1Right` | **C41417361** | Generic connector, single row, 01x04, script generated (kicad-library-utils/schlib/autogen/connector/) |
| 30 | `J5` | 1 | Conn_01x03_Socket | `PinHeader_1x03_P2.54mm_Vertical` | **C2937625** | Generic connector, single row, 01x03, script generated |
| 31 | `J6` | 1 | 20021121-00010C4LF | `PinHeader_2x5_P1.27mm` | — |  |
| 31a | `J8` | 1 | Conn_01x04 | `PinHeader_1x04_P2.54mm_Vertical` | — | DEC-061: new remote battery-pack I2C temperature-sensor probe header, taps existing `I2C CLK`/`I2C DATA`/`CTRL_3V3`/`PWR_NEG` nets (pin1=`I2C CLK`, pin2=`PWR_NEG`, pin3=`CTRL_3V3`, pin4=`I2C DATA`). Exact remote sensor module part/address and mating cable/connector still open |
| 32 | `L2` | 1 | L | `L_APV_APH1240` | **C49308530** | Inductor |
| 33 | `L3` | 1 | L | `L_KOHERelec_MDA7030` | **C49333024** | Inductor |
| 34 | `M1, M2, M3, M4` | 4 | CSD18540Q5B | `TDSON-8-1` | **C86513** | 100A Id, 60V Vds, NexFET N-Channel Power MOSFET, 2.2mOhm Ron, 41nC Qg(typ), SON8 5x6mm |
| 35 | `Q2, Q3, Q7` | 3 | 2N7002K | `Q_SOT-23` | **C22379475** | 60V 340mA 350mW 900mΩ@10V,500mA 2.5V@1mA 1PCSNChannel SOT-23 MOSFETs ROHS |
| 36 | `Q4` | 1 | HSBA0119 | `PQFN-8-EP_6x5mm_P1.27mm_Generic` | **C53244070** | P-MOSFET transistor |
| 37 | `Q5, Q6` | 2 | BSC040N10NS5 | `TDSON-8-1` | **C534334** | 100A Id, 100V Vds, OptiMOS N-Channel Power MOSFET, 4.0mOhm Ron, Qg (typ) 58.0nC, PG-TDSON-8 |
| 38 | `R1, R2, R32, R38, R40, R43, R67, R74` | 8 | 100kΩ | `R_0805` | **C17416** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 100kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 39 | `R3` | 1 | 76.8k | `R_0805` | **C3000581** | ResistorRES 76.8kΩ ±1% 125mW 0805 |
| 40 | `R4, R26, R27, R41, R65, R69, R70, R71, R72, R73` | 10 | 10Ω | `R_0805` | **C17414** | 125mW Thick Film Resistors 150V ±1% ±200ppm/°C 10Ω 0805 Chip Resistor - Surface Mount ROHS |
| 41 | `R5, R44, R45` | 3 | 4.7kΩ | `R_0805` | **C17673** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 4.7kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 42 | `R6, R19, R34, R36, R47, R48, R50, R53, R54, R55, R56, R57, R58, R59` | 14 | 10kΩ | `R_0805` | **C17415** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 10kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 43 | `R7, R10, R61` | 3 | 20kΩ | `R_0805` | **C17523** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 20kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 44 | `R8` | 1 | 27kΩ | `R_0805` | **C17547** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 27kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 45 | `R9` | 1 | 180k | `R_0805` | **C17497** | Resistor |
| 46 | `R11, R39` | 2 | 330Ω | `R_0805` | **C17650** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 330Ω 0805 Chip Resistor - Surface Mount ROHS |
| 47 | `R12` | 1 | 6.8kΩ | `R_0805` | **C17730** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 6.8kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 48 | `R13` | 1 | 1MΩ | `R_0805` | **C17514** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 1MΩ 0805 Chip Resistor - Surface Mount ROHS |
| 49 | `R14` | 1 | 330kΩ | `R_0805` | **C17651** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 330kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 50 | `R15` | 1 | 2kΩ | `R_0805` | **C17537** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 2kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 51 | `R16, R22` | 2 | 0.001ohms | `R_Shunt_Vishay_WSK2512_6332Metric_T2.66mm` | **C41382682** | Shunt resistor with Kelvin connections |
| 52 | `R17` | 1 | 5.62k | `R_0805` | **C2933470** | Resistor RES 5.62kΩ ±1% 125mW 0805 |
| 53 | `R18` | 1 | 76.8k | `R_0805` | **C3000581** | Resistor RES 76.8kΩ ±1% 125mW 0805 |
| 54 | `R20` | 1 | 499k | `R_0805` | **C119857** | Resistor |
| 55 | `R21` | 1 | 102k | `R_0805` | **C119830** | Resistor |
| 56 | `R23` | 1 | 75kΩ | `R_0805` | **C17743** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 75kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 57 | `R24, R62` | 2 | 5.1kΩ | `R_0805` | **C17700** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 5.1kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 58 | `R25` | 1 | 5.62k | `R_0805` | **C2933470** | Resistor  RES 5.62kΩ ±1% 125mW 0805 |
| 59 | `R28` | 1 | 51.7k | `R_0805` | **C119851** | Resistor |
| 60 | `R29` | 1 | 20.5k | `R_0805` | **C119842** | Resistor |
| 61 | `R30` | 1 | 0Ω | `R_0805` | **C17888** | Resistor - CDC pin grounded to disable cable-drop-compensation/current-monitor feature for bring-up (DEC-060); consolidated into row 63's 0Ω group |
| 62 | `R31` | 1 | 2.43k | `R_0805` | **C119853** | Resistor |
| 63 | `R30, R33, R49, R63, R64` | 5 | 0Ω | `R_0805` | **C17888** | 125mW Thick Film Resistors 150V ±800ppm/°C ±1% 0Ω 0805 Chip Resistor - Surface Mount ROHS |
| 64 | `R35` | 1 | 180kΩ | `R_0805` | **C17497** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 180kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 65 | `R37, R66` | 2 | 2.2MΩ | `R_0805` | **C17539** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 2.2MΩ 0805 Chip Resistor - Surface Mount ROHS |
| 66 | `R42, R52` | 2 | 1kΩ | `R_0805` | **C17513** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 1kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 67 | `R46` | 1 | 82kΩ | `R_0805` | **C17840** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 82kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 68 | `R51` | 1 | 2.7kΩ | `R_0805` | **C17546** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 2.7kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 69 | `R60` | 1 | 1.5kΩ | `R_0805` | **C17515** | 125mW Thick Film Resistors 150V ±100ppm/°C ±1% 1.5kΩ 0805 Chip Resistor - Surface Mount ROHS |
| 70 | `R68` | 1 | 1.3k | `R_0805` | **C2933303** | Resistor |
| 71 | `S1` | 1 | Tactile Button, 160gf | `SW_TS-1088-AR02016` | **C720477** | Without 50mA 4mm 100MΩ 100000 Times 12V 160gf 3mm 2mm Round Button Standing paste SPST SMD Tactile Switches ROHS |
| 72 | `SW1, SW2, SW3, SW4, SW5` | 5 | Tactile Button, 160gf | `SW-SMD_4P-L5.1-W5.1-P3.70-LS6.5-TL-2` | **C318884** | None Without 50mA 5.1mm 100000 Times 160gf 12V 5.1mm 1.5mm Round Button Standing paste SPST SMD Tactile Switches ROHS |
| 73 | `TP1` | 1 | TestPoint_Small | `PinHeader_1x01_P2.54mm_Vertical` | — | test point |
| 74 | `TP15, TP16, TP17, TP18` | 4 | 5015 | `TestPoint_Keystone_5015_Micro_Mini` | — |  |
| 75 | `U1` | 1 | STM32G431CBU6 | `QFN-48-1EP_7x7mm_P0.5mm_EP5.6x5.6mm` | **C529356** | Arm Cortex-M4 32-bit MCU+FPU, 170 MHz / 213 DMIPS, up to 128 KB Flash, 32 KB SRAM, rich analog, math accelerator |
| 76 | `U2` | 1 | ESP32_C3_mini | `ESP32 C3 mini` | — |  |
| 77 | `U3` | 1 | CP2102-GMR | `WQFN-28_L5.0-W5.0-P0.50-BL-EP3.2` | **C6568** | 12Mbps Transceiver USB WQFN-28-EP(5x5) USB Converters ROHS |
| 78 | `U4` | 1 | LM5163 | `Texas_R-PDSO-G8_EP2.95x4.9mm_Mask2.4x3.1mm_ThermalVias` | **C2873264** | 100V Input, 0.5A Synchronous Buck DC/DC Converter With Ultra-low IQ |
| 79 | `U5` | 1 | LP5912-3.3DRV | `WSON-6-1EP_2x2mm_P0.65mm_EP1x1.6mm_ThermalVias` | **C114569** | 500-mA Ultra-Low-Noise Low-IQ LDO, 3.3V, WSON-6 |
| 80 | `U6` | 1 | LM51772QRHARQ1 | `VFQFN-40-1EP_6x6mm` | **C41383743** |  |
| 81 | `U7` | 1 | AHT20 | `PSON100P300X300X110-6N` | **C2757850** |  |
| 82 | `U8` | 1 | SSD1306_OLED | `0.96 inOLED display SSD1306` | **C5248080** | 0.96 Inch OLED I2C IIC Display Module 12864 128x64 Pixel SSD1306 |
| 83 | `U9` | 1 | LMV393 | `SOIC-8-1EP_3.9x4.9mm_P1.27mm_EP2.29x3mm_ThermalVias` | **C7984** | Dual General-Purpose Low-Voltage Comparator, SOIC-8/TSSOP-8/VSSOP-8 |
| 84 | `U10` | 1 | INA213AIDCKR | `SOT65P210X110-6N` | **C129346** | Low- or High-Side Measurement, Bidirectional Zero-Drift Current-Shunt Monitors |
| 85 | `U14` | 1 | LM74502HDDFR | `Texas_DDF0008A_SOT-8_1.6x2.9mm_P0.65mm` | **C5219202** | 65V Reverse Battery Protection Ideal Diode Controller TSOT-8 |
| 86 | `X2` | 1 | X49SM8MSD2SC | `CRYSTAL-SMD_L11.5-W4.8-LS12.7` | **C12674** | 8MHz Surface Mount Crystal 20pF ±20ppm ±30ppm HC-49S-SMD Crystals ROHS |
