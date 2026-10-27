param(
    [string]$OutputDir = ".\build"
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "..\..\..")
$gcc = (Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue).Source
$objcopy = (Get-Command arm-none-eabi-objcopy -ErrorAction SilentlyContinue).Source
$sizeTool = (Get-Command arm-none-eabi-size -ErrorAction SilentlyContinue).Source
$objdump = (Get-Command arm-none-eabi-objdump -ErrorAction SilentlyContinue).Source

if (-not $gcc) {
    $gcc = Get-ChildItem -Path (Join-Path $repoRoot ".tools"), $env:LOCALAPPDATA, $env:ProgramFiles, "C:\Program Files (x86)" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $objcopy) {
    $objcopy = Get-ChildItem -Path (Join-Path $repoRoot ".tools"), $env:LOCALAPPDATA, $env:ProgramFiles, "C:\Program Files (x86)" -Recurse -Filter arm-none-eabi-objcopy.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $sizeTool) {
    $sizeTool = Get-ChildItem -Path (Join-Path $repoRoot ".tools"), $env:LOCALAPPDATA, $env:ProgramFiles, "C:\Program Files (x86)" -Recurse -Filter arm-none-eabi-size.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $objdump) {
    $objdump = Get-ChildItem -Path (Join-Path $repoRoot ".tools"), $env:LOCALAPPDATA, $env:ProgramFiles, "C:\Program Files (x86)" -Recurse -Filter arm-none-eabi-objdump.exe -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $gcc -or -not $objcopy) {
    throw "arm-none-eabi-gcc/objcopy not found. Install GNU Arm Embedded Toolchain, then rerun this script."
}

$outputPath = Join-Path $scriptDir $OutputDir
New-Item -ItemType Directory -Force -Path $outputPath | Out-Null

$frameworkRoot = "C:\Users\user\.platformio\packages\framework-stm32cubeg4"
if (-not (Test-Path $frameworkRoot)) {
    throw "Required STM32Cube G4 package not found at $frameworkRoot"
}

$commonSources = @(
    (Join-Path $scriptDir "startup_stm32g473xx.s"),
    (Join-Path $scriptDir "system_stm32g4xx.c")
)
$linkerScript = Join-Path $scriptDir "STM32G473CEUX_FLASH.ld"

$commonCompile = @(
    "-mcpu=cortex-m4",
    "-mthumb",
    "-O2",
    "-ffunction-sections",
    "-fdata-sections",
    "-fno-common",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-std=c11",
    "-I", (Join-Path $frameworkRoot "Drivers\CMSIS\Include"),
    "-I", (Join-Path $frameworkRoot "Drivers\CMSIS\Device\ST\STM32G4xx\Include"),
    "-DSTM32G473xx"
)

$targets = @(
    @{
        Name = "solar_rev1_stage_d8_heartbeat"
        Source = (Join-Path $scriptDir "stage_d8_heartbeat.c")
    },
    @{
        Name = "solar_rev1_stage_i2c_scan"
        Source = (Join-Path $scriptDir "stage_i2c_scan.c")
    },
    @{
        Name = "solar_rev1_stage_uart_bootlog"
        Source = (Join-Path $scriptDir "stage_uart_bootlog.c")
    }
)

foreach ($target in $targets) {
    $name = $target.Name
    $elf = Join-Path $outputPath "$name.elf"
    $bin = Join-Path $outputPath "$name.bin"
    $hex = Join-Path $outputPath "$name.hex"
    $map = Join-Path $outputPath "$name.map"
    $lss = Join-Path $outputPath "$name.lss"

    $linkArgs = @()
    $linkArgs += $commonCompile
    $linkArgs += $commonSources
    $linkArgs += $target.Source
    $linkArgs += @(
        "-Wl,--gc-sections",
        "-Wl,--build-id=none",
        "-Wl,-Map=$map",
        "-Wl,--print-memory-usage",
        "-T", $linkerScript,
        "--specs=nano.specs",
        "--specs=nosys.specs"
    )
    $linkArgs += @("-o", $elf)

    & $gcc @linkArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed for $name"
    }

    & $objcopy -O binary $elf $bin
    & $objcopy -O ihex $elf $hex
    & $objdump -d $elf > $lss
    & $sizeTool $elf
}

Write-Host "Build complete in: $outputPath"
