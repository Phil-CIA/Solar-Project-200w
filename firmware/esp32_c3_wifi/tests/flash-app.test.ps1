param(
    [string]$BuildDir = (Join-Path (Split-Path -Parent $PSScriptRoot) 'build')
)

$ErrorActionPreference = 'Stop'
$helper = Join-Path (Split-Path -Parent $PSScriptRoot) 'flash-app.ps1'
. $helper -Port COM999 -ExpectedMac 'aa:bb:cc:dd:ee:ff' -BuildDir $BuildDir -PlanOnly

$expected = 'aa:bb:cc:dd:ee:ff'
$match = 'MAC: AA:BB:CC:DD:EE:FF'
$other = 'MAC: 00:11:22:33:44:55'

if (-not (Test-ExpectedEspMac -Output @('Chip is ESP32-C3', $match, 'Running stub', $match) -ExpectedMac $expected)) {
    throw 'Matching repeated MAC addresses were rejected.'
}
if (Test-ExpectedEspMac -Output @($match, $other) -ExpectedMac $expected) {
    throw 'Conflicting MAC addresses were accepted.'
}
if (Test-ExpectedEspMac -Output @($other, $other) -ExpectedMac $expected) {
    throw 'Incorrect MAC addresses were accepted.'
}
if (Test-ExpectedEspMac -Output @('Chip is ESP32-C3', 'No MAC reported') -ExpectedMac $expected) {
    throw 'Missing MAC address was accepted.'
}
Write-Host 'PASS: ESP identity output validation'
