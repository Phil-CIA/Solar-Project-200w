$ErrorActionPreference = 'Stop'

Write-Host 'Serial ports:'
Get-CimInstance Win32_SerialPort |
    Select-Object DeviceID, Name, Description, Manufacturer, PNPDeviceID |
    Format-List

Write-Host 'Present USB / serial-like PnP devices:'
Get-PnpDevice -PresentOnly |
    Where-Object {
        $_.InstanceId -match 'USB|VID_|CP210|Silicon|UART|Serial' -or
        $_.FriendlyName -match 'USB|CP210|Silicon|UART|Serial'
    } |
    Select-Object Class, FriendlyName, InstanceId, Status |
    Format-List
