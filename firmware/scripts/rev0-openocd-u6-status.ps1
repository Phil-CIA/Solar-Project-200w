$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'openocd-common.ps1')

$commands = @(
    'adapter serial B55B5A1A0000000020AFF501',
    'init',
    'halt',
    'set saved_moder [mrw 0x48000400]',
    'set saved_otyper [mrw 0x48000404]',
    'set saved_pupdr [mrw 0x4800040C]',
    'mww 0x48000400 [expr {($saved_moder & 0xFFFF0FFF) | 0x00005000}]',
    'mww 0x48000404 [expr {$saved_otyper | 0x000000C0}]',
    'mww 0x4800040C [expr {$saved_pupdr & 0xFFFF0FFF}]',
    'proc sda {high} { if {$high} { mww 0x48000418 0x00000040 } else { mww 0x48000418 0x00400000 }; sleep 1 }',
    'proc scl {high} { if {$high} { mww 0x48000418 0x00000080 } else { mww 0x48000418 0x00800000 }; sleep 1 }',
    'proc read_sda {} { return [expr {([mrw 0x48000410] >> 6) & 1}] }',
    'proc i2c_start {} { sda 1; scl 1; sda 0; scl 0 }',
    'proc i2c_stop {} { sda 0; scl 1; sda 1 }',
    'proc i2c_write {value} { for {set bit 7} {$bit >= 0} {incr bit -1} { sda [expr {($value >> $bit) & 1}]; scl 1; scl 0 }; sda 1; scl 1; set ack [expr {![read_sda]}]; scl 0; return $ack }',
    'proc i2c_read_nack {} { set value 0; sda 1; for {set bit 7} {$bit >= 0} {incr bit -1} { scl 1; set value [expr {$value | ([read_sda] << $bit)}]; scl 0 }; sda 1; scl 1; scl 0; return $value }',
    'set result [catch { i2c_start; if {![i2c_write 0xD4]} { error "U6 write address NACK" }; if {![i2c_write 0x78]} { error "U6 register pointer NACK" }; i2c_start; if {![i2c_write 0xD5]} { error "U6 read address NACK" }; set status [i2c_read_nack]; i2c_stop } message]',
    'catch {i2c_stop}',
    'mww 0x48000400 $saved_moder',
    'mww 0x48000404 $saved_otyper',
    'mww 0x4800040C $saved_pupdr',
    'resume',
    'if {$result} { echo "U6 STATUS_BYTE SWD read failed: $message" } else { echo [format "U6 STATUS_BYTE via SWD = 0x%02X" $status] }',
    'shutdown'
)

Invoke-Stm32G4OpenOcd $commands