$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'openocd-common.ps1')

Invoke-Stm32G4OpenOcd @(
    'init',
    'halt',
    'mww 0x48000418 0x00000020',
    'mdw 0x48000410 1',
    'mww 0x48000418 0x00200000',
    'mdw 0x48000410 1',
    'mww 0x48000018 0x00000020',
    'mdw 0x48000010 1',
    'mww 0x48000018 0x00200000',
    'mdw 0x48000010 1',
    'mdw 0x48000000 6',
    'mdw 0x48000400 6',
    'shutdown'
)
