param(
    [string]$SpiceRoot = $PSScriptRoot,
    [bool]$GeneratePlots = $true,
    [bool]$FailOnWarnings = $false,
    [double]$B1CurrentTargetA = 15.0,
    [double]$B1ExpectedPeakMinA = 12.0,
    [double]$B1ExpectedPeakMaxA = 13.0,
    [string[]]$WarningGateTags = @("measure_failed", "pin_avg<=0", "pout_avg<=0", "eff_pct_out_of_range")
)

$ErrorActionPreference = "Stop"

$ngspiceCmd = Get-Command ngspice -ErrorAction Stop
$ngspice = $ngspiceCmd.Source

$logsDir = Join-Path $SpiceRoot "logs"
New-Item -ItemType Directory -Force -Path $logsDir | Out-Null
$waveformsDir = Join-Path $SpiceRoot "waveforms"
New-Item -ItemType Directory -Force -Path $waveformsDir | Out-Null

$decks = @(
    "00_block_b0_pv_input.cir",
    "01_block_b1_power_stage.cir",
    "02_block_b2_battery_boundary.cir",
    "04_block_b4_fault_network.cir",
    "90_system_integration_harness.cir"
)

$results = @()

function Get-ScalarFromLog {
    param(
        [string]$Text,
        [string]$Name
    )

    $lines = $Text -split "`r?`n"
    $linePattern = "(?i)\b$Name\b\s*="

    foreach ($line in $lines) {
        if ($line -match $linePattern) {
            if ($line -match "=\s*([+-]?(?:\d+\.?\d*|\d*\.\d+)(?:e[+-]?\d+)?)") {
                return $Matches[1]
            }
        }
    }

    return ""
}

function Get-ScalarFromFile {
    param(
        [string]$Path,
        [string]$Name
    )

    if (-not (Test-Path $Path)) {
        return ""
    }

    $linePattern = "(?i)\b$Name\b\s*="
    $matchInfo = Select-String -Path $Path -Pattern $linePattern | Select-Object -First 1
    if ($null -ne $matchInfo) {
        $line = $matchInfo.Line
        if ($line -match "=\s*([+-]?(?:\d+\.?\d*|\d*\.\d+)(?:e[+-]?\d+)?)") {
            return $Matches[1]
        }
    }

    return ""
}

function Get-DeckParamNumeric {
    param(
        [string]$DeckPath,
        [string]$ParamName
    )

    if (-not (Test-Path $DeckPath)) {
        return $null
    }

    $paramPattern = "(?i)^\s*\.param\s+" + [regex]::Escape($ParamName) + "\s*=\s*([+-]?(?:\d+\.?\d*|\d*\.\d+)(?:e[+-]?\d+)?)"
    $m = Select-String -Path $DeckPath -Pattern $paramPattern | Select-Object -First 1
    if ($null -eq $m) {
        return $null
    }

    [double]$value = 0
    $ok = [double]::TryParse(
        $m.Matches[0].Groups[1].Value,
        [System.Globalization.NumberStyles]::Float,
        [System.Globalization.CultureInfo]::InvariantCulture,
        [ref]$value
    )
    if (-not $ok) {
        return $null
    }
    return $value
}

foreach ($deck in $decks) {
    $deckPath = Join-Path $SpiceRoot $deck
    if (-not (Test-Path $deckPath)) {
        $results += [pscustomobject]@{
            deck = $deck
            status = "missing"
            vout_avg = ""
            iin_avg = ""
            iout_avg = ""
            pin_avg = ""
            pout_avg = ""
            eff_pct = ""
            req_chg_001 = ""
            req_chg_003 = ""
            bat_trip = ""
            ocp_trip = ""
            notes = "deck file not found"
        }
        continue
    }

    $logPath = Join-Path $logsDir ($deck + ".log")
    if (Test-Path $logPath) {
        Remove-Item $logPath -Force
    }

    $proc = Start-Process -FilePath $ngspice -ArgumentList @("-b", "-o", $logPath, $deckPath) -WorkingDirectory $SpiceRoot -Wait -NoNewWindow -PassThru
    $status = if ($proc.ExitCode -eq 0) { "ok" } else { "fail" }

    $text = ""
    if (Test-Path $logPath) {
        $text = Get-Content -Path $logPath -Raw

        $hasProblem = Select-String -Path $logPath -Pattern "fatal error|undefined parameter|no such function|simulation\(s\) aborted|timestep too small|cannot compute substitute|no such device" -Quiet
        if ($hasProblem) {
            $status = "fail"
        }
    }

    $voutAvg = Get-ScalarFromFile -Path $logPath -Name "vout_avg"
    $iinAvg = Get-ScalarFromFile -Path $logPath -Name "iin_avg"
    $batTrip = Get-ScalarFromFile -Path $logPath -Name "bat_trip"
    $ocpTrip = Get-ScalarFromFile -Path $logPath -Name "ocp_trip"
    $ioutAvg = Get-ScalarFromFile -Path $logPath -Name "iout_avg"
    $pinAvg = Get-ScalarFromFile -Path $logPath -Name "pin_avg"
    $poutAvg = Get-ScalarFromFile -Path $logPath -Name "pout_avg"
    $effPct = Get-ScalarFromFile -Path $logPath -Name "eff_pct"

    $results += [pscustomobject]@{
        deck = $deck
        status = $status
        vout_avg = $voutAvg
        iin_avg = $iinAvg
        iout_avg = $ioutAvg
        pin_avg = $pinAvg
        pout_avg = $poutAvg
        eff_pct = $effPct
        req_chg_001 = ""
        req_chg_003 = ""
        bat_trip = $batTrip
        ocp_trip = $ocpTrip
        notes = "log: logs/$deck.log"
    }
}

# Post-pass: rebuild summary deterministically from final logs.
$results = @()
foreach ($deck in $decks) {
    $deckPath = Join-Path $SpiceRoot $deck
    $rowLogPath = Join-Path $logsDir ($deck + ".log")

    if (-not (Test-Path $deckPath)) {
        $results += [pscustomobject]@{
            deck = $deck
            status = "missing"
            vout_avg = ""
            iin_avg = ""
            iout_avg = ""
            pin_avg = ""
            pout_avg = ""
            eff_pct = ""
            req_chg_001 = ""
            req_chg_003 = ""
            bat_trip = ""
            ocp_trip = ""
            notes = "deck file not found"
        }
        continue
    }

    $finalStatus = "ok"
    if (-not (Test-Path $rowLogPath)) {
        $finalStatus = "fail"
    }

    $rowText = ""
    if (Test-Path $rowLogPath) {
        $rowText = Get-Content -Path $rowLogPath -Raw
        if ($null -eq $rowText) {
            $rowText = ""
        }
        if ($rowText.ToLowerInvariant() -match "fatal error|undefined parameter|no such function|simulation\(s\) aborted|timestep too small|cannot compute substitute|no such device") {
            $finalStatus = "fail"
        }
    }

    $voutVal = Get-ScalarFromFile -Path $rowLogPath -Name "vout_avg"
    $iinVal = Get-ScalarFromFile -Path $rowLogPath -Name "iin_avg"
    $ioutVal = Get-ScalarFromFile -Path $rowLogPath -Name "iout_avg"
    $pinVal = Get-ScalarFromFile -Path $rowLogPath -Name "pin_avg"
    $poutVal = Get-ScalarFromFile -Path $rowLogPath -Name "pout_avg"
    $effVal = Get-ScalarFromFile -Path $rowLogPath -Name "eff_pct"
    $batVal = Get-ScalarFromFile -Path $rowLogPath -Name "bat_trip"
    $ocpVal = Get-ScalarFromFile -Path $rowLogPath -Name "ocp_trip"

    $notes = "log: logs/$deck.log"
    $warningList = @()
    $reqChg001 = ""
    $reqChg003 = ""
    if ($deck -eq "01_block_b1_power_stage.cir") {
        $deckRload = Get-DeckParamNumeric -DeckPath $deckPath -ParamName "RLOAD"
        $deckVin = Get-DeckParamNumeric -DeckPath $deckPath -ParamName "VIN"

        [double]$pinNum = 0
        [double]$poutNum = 0
        [double]$effNum = 0
        [double]$iinNum = 0
        [double]$voutNum = 0
        [double]$ioutNum = 0

        if ([string]::IsNullOrWhiteSpace($ioutVal) -and
            -not [string]::IsNullOrWhiteSpace($voutVal) -and
            $null -ne $deckRload -and $deckRload -gt 0 -and
            [double]::TryParse($voutVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$voutNum)) {
            $ioutVal = ($voutNum / $deckRload).ToString("0.00000e+00")
        }

        if ([string]::IsNullOrWhiteSpace($pinVal) -and
            -not [string]::IsNullOrWhiteSpace($iinVal) -and
            $null -ne $deckVin -and
            [double]::TryParse($iinVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$iinNum)) {
            $pinVal = ((-$deckVin) * $iinNum).ToString("0.00000e+00")
        }

        if ([string]::IsNullOrWhiteSpace($poutVal) -and
            -not [string]::IsNullOrWhiteSpace($voutVal) -and
            $null -ne $deckRload -and $deckRload -gt 0 -and
            [double]::TryParse($voutVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$voutNum)) {
            $poutVal = (($voutNum * $voutNum) / $deckRload).ToString("0.00000e+00")
        }

        if (-not [string]::IsNullOrWhiteSpace($pinVal) -and
            [double]::TryParse($pinVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$pinNum) -and
            $pinNum -le 0) {
            $warningList += "pin_avg<=0"
        }
        if (-not [string]::IsNullOrWhiteSpace($poutVal) -and
            [double]::TryParse($poutVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$poutNum) -and
            $poutNum -le 0) {
            $warningList += "pout_avg<=0"
        }

        if ([string]::IsNullOrWhiteSpace($effVal) -and
            -not [string]::IsNullOrWhiteSpace($pinVal) -and
            -not [string]::IsNullOrWhiteSpace($poutVal) -and
            [double]::TryParse($pinVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$pinNum) -and
            [double]::TryParse($poutVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$poutNum) -and
            [math]::Abs($pinNum) -gt 1e-12) {
            $effVal = (100.0 * $poutNum / $pinNum).ToString("0.00000e+00")
        }

        if (-not [string]::IsNullOrWhiteSpace($effVal) -and
            [double]::TryParse($effVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$effNum)) {
            if ($effNum -le 0 -or $effNum -gt 100) {
                $warningList += "eff_pct_out_of_range"
            }
        }
        if (-not [string]::IsNullOrWhiteSpace($iinVal) -and
            [double]::TryParse($iinVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$iinNum) -and
            $iinNum -lt 0) {
            $warningList += "iin_avg_negative"
        }

        if (-not [string]::IsNullOrWhiteSpace($ioutVal) -and
            [double]::TryParse($ioutVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$ioutNum) -and
            $ioutNum -lt $B1CurrentTargetA) {
            $warningList += "iout_below_${B1CurrentTargetA}A"
        }

        if (-not [string]::IsNullOrWhiteSpace($ioutVal) -and
            [double]::TryParse($ioutVal, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$ioutNum)) {
            $reqChg001 = if ($ioutNum -ge $B1CurrentTargetA) { "PASS" } else { "FAIL" }
            $reqChg003 = if ($ioutNum -ge $B1ExpectedPeakMinA -and $ioutNum -le $B1ExpectedPeakMaxA) { "PASS" } else { "FAIL" }
        }
        else {
            $reqChg001 = "UNKNOWN"
            $reqChg003 = "UNKNOWN"
        }

        if (Select-String -Path $rowLogPath -Pattern "(?i)measure\s+.*failed" -Quiet) {
            $warningList += "measure_failed"
        }
    }
    if ($warningList.Count -gt 0) {
        $notes = "$notes; warn=" + ($warningList -join "|")
    }

    $results += [pscustomobject]@{
        deck = $deck
        status = $finalStatus
        vout_avg = $voutVal
        iin_avg = $iinVal
        iout_avg = $ioutVal
        pin_avg = $pinVal
        pout_avg = $poutVal
        eff_pct = $effVal
        req_chg_001 = $reqChg001
        req_chg_003 = $reqChg003
        bat_trip = $batVal
        ocp_trip = $ocpVal
        notes = $notes
    }
}

$summaryCsv = Join-Path $SpiceRoot "corner_summary.csv"
$results | Export-Csv -NoTypeInformation -Path $summaryCsv -Encoding ascii

if ($GeneratePlots) {
    $plotScript = Join-Path $SpiceRoot "generate_spice_plots.ps1"
    if (Test-Path $plotScript) {
        & $plotScript -SpiceRoot $SpiceRoot | Out-Null
    }
}

$hasFail = ($results | Where-Object { $_.status -eq "fail" }).Count -gt 0
$hasWarnGateHit = $false
if ($FailOnWarnings) {
    foreach ($r in $results) {
        if ([string]::IsNullOrWhiteSpace($r.notes)) {
            continue
        }
        foreach ($tag in $WarningGateTags) {
            if ($r.notes -match [regex]::Escape($tag)) {
                $hasWarnGateHit = $true
                break
            }
        }
        if ($hasWarnGateHit) {
            break
        }
    }
}

Write-Host "Wrote: $summaryCsv"
Write-Host "Logs:  $logsDir"
if ($GeneratePlots) {
    Write-Host "Plots: $(Join-Path $SpiceRoot 'plots')"
}
$results | Format-Table -AutoSize

if ($hasFail -or $hasWarnGateHit) {
    Write-Host "GateResult: FAIL"
    exit 2
}

Write-Host "GateResult: PASS"
exit 0
