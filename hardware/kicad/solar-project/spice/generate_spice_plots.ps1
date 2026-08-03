param(
	[string]$SpiceRoot = $PSScriptRoot,
	[int]$MaxPointsPerChart = 4000
)

$ErrorActionPreference = "Stop"

$waveformsDir = Join-Path $SpiceRoot "waveforms"
$plotsDir = Join-Path $SpiceRoot "plots"
New-Item -ItemType Directory -Force -Path $plotsDir | Out-Null

$specs = @(
		@{
				File = "01_block_b1_power_stage_tran.csv"
				Title = "B1 Power Stage Transient"
				TraceNames = @("V_out", "CHG_OUT_POS", "SW_1", "SW_2")
		},
		@{
				File = "90_system_integration_harness_tran.csv"
				Title = "System Integration Harness Transient"
				TraceNames = @("PV_IN_POS_RAW", "PV_IN_POS_FUSED", "PV_IN_POS", "V_out", "CHG_OUT_POS", "BAT_BUS_POS", "SW_1", "SW_2")
		}
)

function Get-WrdataParts {
		param(
				[string]$Line,
				[int]$ExpectedColumns
		)

		$t = $Line.Trim()
		if ([string]::IsNullOrWhiteSpace($t)) {
				return $null
		}

		# Skip non-data lines (headers/comments)
		if ($t -match "[A-Za-z]" -and $t -notmatch "[eE][+-]?\d") {
				return $null
		}

		$parts = $t -split "\s+" | Where-Object { -not [string]::IsNullOrWhiteSpace($_) }
		if ($parts.Count -eq ($ExpectedColumns + 1)) {
				# ngspice wrdata may emit a duplicated scale column (time,time,...)
				$parts = @($parts[0]) + $parts[2..($parts.Count - 1)]
		}
		if ($parts.Count -lt $ExpectedColumns) {
				return $null
		}

		return $parts
}

function Get-SampledWaveform {
		param(
				[string]$Path,
				[int]$ExpectedColumns,
				[int]$MaxPoints,
				[int]$TraceCount
		)

		if (-not (Test-Path $Path)) {
				return $null
		}

		$totalRows = 0
		foreach ($line in Get-Content -Path $Path) {
				$parts = Get-WrdataParts -Line $line -ExpectedColumns $ExpectedColumns
				if ($null -ne $parts) {
						$totalRows++
				}
		}

		if ($totalRows -eq 0) {
				return $null
		}

		$step = [Math]::Max(1, [int][Math]::Ceiling($totalRows / [double]$MaxPoints))

		$x = New-Object 'System.Collections.Generic.List[double]'
		$yLists = @()
		for ($i = 0; $i -lt $TraceCount; $i++) {
				$yLists += ,(New-Object 'System.Collections.Generic.List[double]')
		}

		$rowIndex = 0
		foreach ($line in Get-Content -Path $Path) {
				$parts = Get-WrdataParts -Line $line -ExpectedColumns $ExpectedColumns
				if ($null -eq $parts) {
						continue
				}

				if (($rowIndex % $step) -ne 0) {
						$rowIndex++
						continue
				}

				[double]$timeSec = 0
				if (-not [double]::TryParse($parts[0], [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$timeSec)) {
						$rowIndex++
						continue
				}
				$x.Add([math]::Round(($timeSec * 1000.0), 9))

				for ($i = 0; $i -lt $TraceCount; $i++) {
						[double]$val = 0
						if ([double]::TryParse($parts[$i + 1], [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$val)) {
								$yLists[$i].Add([math]::Round($val, 9))
						}
						else {
								$yLists[$i].Add([double]::NaN)
						}
				}

				$rowIndex++
		}

		return @{
				x = $x.ToArray()
				y = $yLists
		}
}

$chartPayload = @()
foreach ($spec in $specs) {
		$csvPath = Join-Path $waveformsDir $spec.File
		$expectedColumns = 1 + $spec.TraceNames.Count
		$sampled = Get-SampledWaveform -Path $csvPath -ExpectedColumns $expectedColumns -MaxPoints $MaxPointsPerChart -TraceCount $spec.TraceNames.Count
		if ($null -eq $sampled) {
				continue
		}

		$timeMs = $sampled.x

		$traces = @()
		for ($i = 0; $i -lt $spec.TraceNames.Count; $i++) {
				$y = $sampled.y[$i].ToArray()
				$traces += @{
						name = $spec.TraceNames[$i]
						y = $y
				}
		}

		$chartPayload += @{
				id = [System.IO.Path]::GetFileNameWithoutExtension($spec.File)
				title = $spec.Title
				x = $timeMs
				traces = $traces
		}
}

$json = $chartPayload | ConvertTo-Json -Depth 8 -Compress
$htmlPath = Join-Path $plotsDir "index.html"

$html = @"
<!doctype html>
<html lang="en">
<head>
	<meta charset="utf-8" />
	<meta name="viewport" content="width=device-width, initial-scale=1" />
	<title>SPICE Waveform Plots</title>
	<script src="https://cdn.plot.ly/plotly-2.35.2.min.js"></script>
	<style>
		:root {
			--bg: #f6f7fb;
			--panel: #ffffff;
			--ink: #172033;
			--accent: #1f5eff;
			--grid: #d8dcea;
		}
		body {
			margin: 0;
			font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
			background: radial-gradient(circle at top left, #eef2ff 0%, var(--bg) 45%, #f9fbff 100%);
			color: var(--ink);
		}
		.wrap {
			max-width: 1200px;
			margin: 24px auto;
			padding: 0 16px 24px;
		}
		h1 {
			margin: 0 0 8px;
			font-size: 28px;
			font-weight: 700;
			color: #111a2e;
		}
		p {
			margin: 0 0 18px;
			color: #394868;
		}
		.card {
			background: var(--panel);
			border: 1px solid #e4e7f2;
			border-radius: 14px;
			padding: 10px 12px;
			margin-bottom: 16px;
			box-shadow: 0 8px 24px rgba(27, 40, 78, 0.08);
		}
		.plot {
			width: 100%;
			height: 460px;
		}
	</style>
</head>
<body>
	<div class="wrap">
		<h1>SPICE Waveform Dashboard</h1>
		<p>Generated from ngspice <code>wrdata</code> files in <code>waveforms/</code>. X-axis is time in ms.</p>
		<div id="plots"></div>
	</div>

	<script>
		const payload = $json;
		const host = document.getElementById('plots');

		if (!payload || payload.length === 0) {
			const msg = document.createElement('p');
			msg.textContent = 'No waveform files were found. Run run_spice_corner_sweeps.ps1 first.';
			host.appendChild(msg);
		}

		payload.forEach((chart) => {
			const card = document.createElement('section');
			card.className = 'card';
			const div = document.createElement('div');
			div.id = chart.id;
			div.className = 'plot';
			card.appendChild(div);
			host.appendChild(card);

			const traces = chart.traces.map((t) => ({
				x: chart.x,
				y: t.y,
				name: t.name,
				mode: 'lines',
				line: { width: 1.8 }
			}));

			const layout = {
				title: { text: chart.title, x: 0.02, xanchor: 'left' },
				margin: { l: 65, r: 20, t: 50, b: 50 },
				legend: { orientation: 'h' },
				paper_bgcolor: '#ffffff',
				plot_bgcolor: '#ffffff',
				xaxis: { title: 'Time (ms)', gridcolor: '#d8dcea' },
				yaxis: { title: 'Voltage (V)', gridcolor: '#d8dcea' }
			};

			Plotly.newPlot(div, traces, layout, { responsive: true, displaylogo: false });
		});
	</script>
</body>
</html>
"@

Set-Content -Path $htmlPath -Value $html -Encoding ascii
Write-Output $htmlPath
