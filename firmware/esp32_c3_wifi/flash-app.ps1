[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^COM[1-9][0-9]*$')]
    [string]$Port,

    [Parameter(Mandatory = $true)]
    [ValidatePattern('^([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}$')]
    [string]$ExpectedMac,

    [string]$BuildDir = (Join-Path $PSScriptRoot 'build'),
    [switch]$PlanOnly
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Invoke-Esptool {
    param([string[]]$Arguments)

    $result = @(& $python -m esptool @Arguments 2>&1)
    $exitCode = $LASTEXITCODE
    foreach ($line in $result) {
        Write-Host "$line"
    }
    if ($exitCode -ne 0) {
        throw "esptool failed with exit code $exitCode."
    }
    return @($result | ForEach-Object { "$_" })
}

function Convert-PartitionSize {
    param([string]$Text)

    $text = $Text.Trim()
    if ($text -match '^0x[0-9a-fA-F]+$') {
        return [Convert]::ToUInt64($text, 16)
    }
    if ($text -notmatch '^([0-9]+)([KkMm]?)$') {
        throw "Invalid partition size: $text"
    }
    $size = [uint64]$Matches[1]
    switch ($Matches[2].ToUpperInvariant()) {
        'K' { $size *= 1024 }
        'M' { $size *= 1024 * 1024 }
    }
    return $size
}

function Test-ExpectedEspMac {
    param([string[]]$Output, [string]$ExpectedMac)

    $found = $false
    foreach ($line in $Output) {
        if ($line -match '^\s*MAC:\s*((?:[0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})\s*$') {
            $found = $true
            if ($Matches[1] -ine $ExpectedMac) {
                return $false
            }
        }
    }
    return $found
}

if (-not $env:IDF_PATH) {
    throw 'Activate the ESP-IDF environment before running this helper.'
}
$partitionTool = Join-Path $env:IDF_PATH 'components\partition_table\gen_esp32part.py'
if (-not (Test-Path -LiteralPath $partitionTool -PathType Leaf)) {
    throw "ESP-IDF partition tool not found: $partitionTool"
}
$python = (Get-Command python -CommandType Application -ErrorAction Stop | Select-Object -First 1).Source
$build = (Resolve-Path -LiteralPath $BuildDir -ErrorAction Stop).Path
$description = Get-Content -LiteralPath (Join-Path $build 'project_description.json') -Raw | ConvertFrom-Json
$flasher = Get-Content -LiteralPath (Join-Path $build 'flasher_args.json') -Raw | ConvertFrom-Json

if ($description.target -cne 'esp32c3' -or $description.project_name -cne 'solar_esp32_wifi' -or
    $description.app_bin -cne 'solar_esp32_wifi.bin' -or
    $flasher.extra_esptool_args.chip -cne 'esp32c3' -or
    $flasher.app.file -cne $description.app_bin -or $flasher.app.encrypted -ne 'false') {
    throw 'Build metadata is not an unencrypted solar_esp32_wifi ESP32-C3 application.'
}
if ($flasher.app.offset -notmatch '^0x[0-9a-fA-F]+$' -or
    $flasher.'partition-table'.offset -ine '0x8000' -or
    ($flasher.'partition-table'.file -replace '/', '\') -cne 'partition_table\partition-table.bin') {
    throw 'Unsupported app or partition-table offset in build metadata.'
}
$appOffset = [Convert]::ToUInt64($flasher.app.offset, 16)
$image = Get-Item -LiteralPath (Join-Path $build $description.app_bin)
$partition = Get-Item -LiteralPath (Join-Path $build 'partition_table\partition-table.bin')
if ($image.PSIsContainer -or $image.Length -le 0 -or $partition.PSIsContainer -or
    $partition.Length -le 0 -or $partition.Length -gt 4096) {
    throw 'Invalid application image or partition table.'
}

$partitionOutput = @(& $python $partitionTool $partition.FullName 2>&1)
if ($LASTEXITCODE -ne 0) {
    throw "Invalid build partition table: $($partitionOutput -join ' ')"
}
$entries = @($partitionOutput |
    Where-Object { "$_" -match '^\s*[^#,\s][^,]*,\s*(app|data)\s*,' } |
    ConvertFrom-Csv -Header Name, Type, SubType, Offset, Size, Flags)
$appEntries = @($entries | Where-Object {
    $_.Type.Trim() -eq 'app' -and $_.SubType.Trim() -eq 'factory' -and
    $_.Offset.Trim() -match '^0x[0-9a-fA-F]+$' -and
    [Convert]::ToUInt64($_.Offset.Trim(), 16) -eq $appOffset
})
if ($appEntries.Count -ne 1 -or $appOffset % 4096 -ne 0) {
    throw 'App offset does not match exactly one sector-aligned factory partition.'
}
$appSize = Convert-PartitionSize $appEntries[0].Size
$eraseSize = [uint64]([Math]::Ceiling($image.Length / 4096.0) * 4096)
if ($appSize -eq 0 -or $eraseSize -gt $appSize) {
    throw 'The application would extend past its factory partition.'
}

$null = Invoke-Esptool -Arguments @('version')
$info = @(Invoke-Esptool -Arguments @('--chip', 'esp32c3', 'image_info', $image.FullName))
$infoText = $info -join "`n"
if ($infoText -notmatch 'Checksum:\s+[0-9a-fA-F]+\s+\(valid\)' -or
    $infoText -notmatch 'Validation Hash:\s+[0-9a-fA-F]{64}\s+\(valid\)') {
    throw 'Application image checksum or validation hash is missing or invalid.'
}

$imageHash = (Get-FileHash -LiteralPath $image.FullName -Algorithm SHA256).Hash
$partitionHash = (Get-FileHash -LiteralPath $partition.FullName -Algorithm SHA256).Hash
Write-Host "Application: $($image.FullName)"
Write-Host "SHA-256: $imageHash"
Write-Host "Factory offset: $($flasher.app.offset); image: $($image.Length) bytes; partition: $appSize bytes"
Write-Host "Target: ESP32-C3 on $Port, expected MAC $ExpectedMac"
Write-Host 'Only the application partition will be written; no full erase is requested.'
if ($PlanOnly) {
    Write-Host 'Preview only: no device was accessed or flashed.'
    return
}

Write-Warning 'Use a safe bench state: converter inhibited, no PV, battery or load connected.'
Write-Warning 'The ESP will reset. The existing bootloader must be compatible. Do not change the USB connection during this operation.'
if ((Read-Host 'Type CHECK to reset and identify the ESP, or anything else to cancel') -cne 'CHECK') {
    Write-Warning 'Cancelled before accessing the ESP.'
    return
}

$identity = @(Invoke-Esptool -Arguments @('--chip', 'esp32c3', '--port', $Port, 'read_mac'))
if (-not (Test-ExpectedEspMac -Output $identity -ExpectedMac $ExpectedMac)) {
    throw 'ESP MAC does not match the expected target; no flash was written.'
}

$devicePartition = Join-Path ([IO.Path]::GetTempPath()) "solar-partition-$([guid]::NewGuid().ToString('N')).bin"
try {
    $null = Invoke-Esptool -Arguments @(
        '--chip', 'esp32c3', '--port', $Port, 'read_flash',
        '0x8000', ("0x{0:x}" -f $partition.Length), $devicePartition
    )
    $deviceHash = (Get-FileHash -LiteralPath $devicePartition -Algorithm SHA256).Hash
    if ($deviceHash -cne $partitionHash) {
        throw 'ESP partition table does not match the build; no flash was written.'
    }
    Write-Host 'ESP identity and partition table match.'

    if ((Read-Host 'Type FLASH to write the application, or anything else to cancel') -cne 'FLASH') {
        Write-Warning 'Cancelled; no flash was written.'
        return
    }
    if ((Get-FileHash -LiteralPath $image.FullName -Algorithm SHA256).Hash -cne $imageHash -or
        (Get-FileHash -LiteralPath $partition.FullName -Algorithm SHA256).Hash -cne $partitionHash) {
        throw 'Build files changed after verification; no flash was written.'
    }
    $null = Invoke-Esptool -Arguments @(
        '--chip', 'esp32c3', '--port', $Port, 'write_flash', '--verify',
        '--flash_mode', 'keep', '--flash_freq', 'keep', '--flash_size', 'keep',
        $flasher.app.offset, $image.FullName
    )
    Write-Host 'Application flash completed and verified.'
} finally {
    if (Test-Path -LiteralPath $devicePartition) {
        Remove-Item -LiteralPath $devicePartition -Force
    }
}
