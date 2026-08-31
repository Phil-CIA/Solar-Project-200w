$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'openocd-common.ps1')

Invoke-Stm32G4OpenOcd @(
    'init',
    'halt',
    'reg msp 0x20018000',
    'reg pc 0x08000040',
    'resume',
    'sleep 500',
    'halt',
    'reg pc',
    'mdw 0x20000000 8',
    'shutdown'
)
