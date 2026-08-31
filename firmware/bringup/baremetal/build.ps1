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

$elf = Join-Path $outDir 'solar_rev0_stage1.elf'
$bin = Join-Path $outDir 'solar_rev0_stage1.bin'
$map = Join-Path $outDir 'solar_rev0_stage1.map'

$flags = @(
    '-mcpu=cortex-m4',
    '-mthumb',
    '-mfpu=fpv4-sp-d16',
    '-mfloat-abi=hard',
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Os',
    '-ffunction-sections',
    '-fdata-sections',
    '-fno-builtin',
    '-nostdlib'
)

& $gcc @flags `
    (Join-Path $scriptDir 'startup_stm32g4xx.s') `
    (Join-Path $scriptDir 'stm32g4_rev0_stage1.c') `
    '-T' (Join-Path $scriptDir 'STM32G47x_512K_FLASH.ld') `
    '-Wl,--gc-sections' `
    "-Wl,-Map=$map" `
    '-o' $elf

& $objcopy -O binary $elf $bin

if ($size) {
    & $size $elf
}

Write-Host "Built $elf"
Write-Host "Built $bin"
