$ErrorActionPreference = 'Stop'

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir '..\..\..')
$outDir = Join-Path $scriptDir 'build'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$gcc = (Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue).Source
$objcopy = (Get-Command arm-none-eabi-objcopy -ErrorAction SilentlyContinue).Source
$size = (Get-Command arm-none-eabi-size -ErrorAction SilentlyContinue).Source

if (-not $gcc) {
    $gcc = Get-ChildItem -Path (Join-Path $repoRoot '.tools'), $env:LOCALAPPDATA, $env:ProgramFiles, 'C:\Program Files (x86)' -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $objcopy) {
    $objcopy = Get-ChildItem -Path (Join-Path $repoRoot '.tools'), $env:LOCALAPPDATA, $env:ProgramFiles, 'C:\Program Files (x86)' -Recurse -Filter arm-none-eabi-objcopy.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $size) {
    $size = Get-ChildItem -Path (Join-Path $repoRoot '.tools'), $env:LOCALAPPDATA, $env:ProgramFiles, 'C:\Program Files (x86)' -Recurse -Filter arm-none-eabi-size.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $gcc -or -not $objcopy) {
    throw 'arm-none-eabi-gcc/objcopy not found. Install GNU Arm Embedded Toolchain, then rerun this script.'
}

$frameworkRoot = 'C:\Users\user\.platformio\packages\framework-stm32cubeg4'
if (-not (Test-Path $frameworkRoot)) {
    throw "Required STM32Cube G4 package not found at $frameworkRoot"
}

$elf = Join-Path $outDir 'solar_rev1_g473_minimal.elf'
$bin = Join-Path $outDir 'solar_rev1_g473_minimal.bin'
$hex = Join-Path $outDir 'solar_rev1_g473_minimal.hex'
$map = Join-Path $outDir 'solar_rev1_g473_minimal.map'

$flags = @(
    '-mcpu=cortex-m4',
    '-mthumb',
    '-Os',
    '-ffunction-sections',
    '-fdata-sections',
    '-Wall',
    '-Wextra'
)

$includes = @(
    '-I', (Join-Path $frameworkRoot 'Drivers\CMSIS\Include'),
    '-I', (Join-Path $frameworkRoot 'Drivers\CMSIS\Device\ST\STM32G4xx\Include'),
    '-DSTM32G473xx'
)

& $gcc @flags @includes `
    (Join-Path $scriptDir 'startup_stm32g473xx.s') `
    (Join-Path $scriptDir 'system_stm32g4xx.c') `
    (Join-Path $scriptDir 'main.c') `
    '-T' (Join-Path $scriptDir 'STM32G473CEUX_FLASH.ld') `
    '-Wl,--gc-sections,--relax' `
    "-Wl,-Map=$map" `
    '--specs=nano.specs' `
    '--specs=nosys.specs' `
    '-o' $elf

& $objcopy -O binary $elf $bin
& $objcopy -O ihex $elf $hex

if ($size) {
    & $size $elf
}

Write-Host "Built $elf"
Write-Host "Built $bin"
Write-Host "Built $hex"
Write-Host "Built $map"

