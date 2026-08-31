$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'openocd-common.ps1')

Invoke-Stm32G4OpenOcd @(
    'init',
    'halt',
    'mww 0x48000018 0x01200000',
    'mww 0x48000418 0x002000C0',
    'mdw 0x48000000 6',
    'mdw 0x48000400 6',
    'shutdown'
)
