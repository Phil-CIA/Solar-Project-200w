@echo off
setlocal
set "LTSPICE_EXE=C:\Users\forch\AppData\Local\Programs\ADI\LTspice\LTspice.exe"
if not exist "%LTSPICE_EXE%" (
  echo LTspice executable not found at expected path: %LTSPICE_EXE%
  echo Update this script to point at the correct executable if the install location changes.
  exit /b 1
)
start "" "%LTSPICE_EXE%" "%~dp0pv_switch_gate_drive.asc"
