$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'openocd-common.ps1')

Invoke-Stm32G4OpenOcd @(
    'init',
    'halt',
    'targets',
    'mdw 0x4002104C 1',
    'mdw 0x48000000 6',
    'mdw 0x48000400 6',
    'shutdown'
)
