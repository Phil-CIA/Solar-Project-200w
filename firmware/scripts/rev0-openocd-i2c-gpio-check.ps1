$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'openocd-common.ps1')

$gpiobModer = 0xAAFFF6BF
$gpiobModer = ($gpiobModer -band (-bnot (0xF -shl 12))) -bor (0x5 -shl 12)
$gpiobPupdr = 0xAA000100
$gpiobPupdr = ($gpiobPupdr -band (-bnot (0xF -shl 12))) -bor (0x5 -shl 12)

Invoke-Stm32G4OpenOcd @(
    'init',
    'halt',
    ('mww 0x48000400 0x{0:X8}' -f $gpiobModer),
    'mww 0x48000404 0x000000C0',
    ('mww 0x4800040C 0x{0:X8}' -f $gpiobPupdr),
    'mww 0x48000418 0x000000C0',
    'mdw 0x48000410 1',
    'mww 0x48000418 0x00800000',
    'mdw 0x48000410 1',
    'mww 0x48000418 0x000000C0',
    'mdw 0x48000410 1',
    'mdw 0x48000400 6',
    'shutdown'
)
