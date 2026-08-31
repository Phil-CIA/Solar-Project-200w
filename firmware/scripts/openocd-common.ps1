$ErrorActionPreference = 'Stop'

function Get-OpenOcdPath {
    $command = Get-Command openocd -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    $candidate = Get-ChildItem -Path $env:LOCALAPPDATA, $env:ProgramFiles -Recurse -Filter openocd.exe -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -like '*xpack-openocd*' } |
        Select-Object -First 1 -ExpandProperty FullName

    if ($candidate) {
        return $candidate
    }

    throw 'OpenOCD was not found. Install xPack OpenOCD or add openocd.exe to PATH.'
}

function Invoke-Stm32G4OpenOcd {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Commands
    )

    $openocd = Get-OpenOcdPath
    $arguments = @('-f', 'interface/stlink.cfg', '-f', 'target/stm32g4x.cfg')

    foreach ($command in $Commands) {
        $arguments += @('-c', $command)
    }

    & $openocd @arguments
}
