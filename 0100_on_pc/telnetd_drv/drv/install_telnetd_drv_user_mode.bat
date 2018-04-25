set DRV_TYPE=TestDrives
rem set DRV_TYPE=BuiltIn

REM ==============================================================

set DRV_ROOT=\HKEY_LOCAL_MACHINE\Drivers\%DRV_TYPE%\TELNETD_DRIVER_0

regtool.exe add -p %DRV_ROOT%

regtool.exe set -s %DRV_ROOT%\Dll "telnetd_drv.dll"
regtool.exe set -s %DRV_ROOT%\Prefix "TLD"
regtool.exe set -d %DRV_ROOT%\Index 0x0
regtool.exe set -d %DRV_ROOT%\Order 0x10000000
regtool.exe set -s %DRV_ROOT%\FriendlyName "Telnetd Driver"
regtool.exe set -d %DRV_ROOT%\Ioctl 0x0

regtool.exe set -d %DRV_ROOT%\Flags 0x12
regtool.exe set -d %DRV_ROOT%\UserProcGroup 0x0220

REM ==============================================================

set DRV_ROOT=\HKEY_LOCAL_MACHINE\Drivers\%DRV_TYPE%\TELNETD_DRIVER_1

regtool.exe add -p %DRV_ROOT%

regtool.exe set -s %DRV_ROOT%\Dll "telnetd_drv.dll"
regtool.exe set -s %DRV_ROOT%\Prefix "TLD"
regtool.exe set -d %DRV_ROOT%\Index 0x1
regtool.exe set -d %DRV_ROOT%\Order 0x10000000
regtool.exe set -s %DRV_ROOT%\FriendlyName "Telnetd Driver"
regtool.exe set -d %DRV_ROOT%\Ioctl 0x0

regtool.exe set -d %DRV_ROOT%\Flags 0x12
regtool.exe set -d %DRV_ROOT%\UserProcGroup 0x0221

REM ==============================================================

set DRV_ROOT=\HKEY_LOCAL_MACHINE\Drivers\%DRV_TYPE%\TELNETD_DRIVER_2

regtool.exe add -p %DRV_ROOT%

regtool.exe set -s %DRV_ROOT%\Dll "telnetd_drv.dll"
regtool.exe set -s %DRV_ROOT%\Prefix "TLD"
regtool.exe set -d %DRV_ROOT%\Index 0x2
regtool.exe set -d %DRV_ROOT%\Order 0x10000000
regtool.exe set -s %DRV_ROOT%\FriendlyName "Telnetd Driver"
regtool.exe set -d %DRV_ROOT%\Ioctl 0x0

regtool.exe set -d %DRV_ROOT%\Flags 0x12
regtool.exe set -d %DRV_ROOT%\UserProcGroup 0x0222

REM ==============================================================

set DRV_ROOT=\HKEY_LOCAL_MACHINE\Drivers\%DRV_TYPE%\TELNETD_DRIVER_3

regtool.exe add -p %DRV_ROOT%

regtool.exe set -s %DRV_ROOT%\Dll "telnetd_drv.dll"
regtool.exe set -s %DRV_ROOT%\Prefix "TLD"
regtool.exe set -d %DRV_ROOT%\Index 0x3
regtool.exe set -d %DRV_ROOT%\Order 0x10000000
regtool.exe set -s %DRV_ROOT%\FriendlyName "Telnetd Driver"
regtool.exe set -d %DRV_ROOT%\Ioctl 0x0

regtool.exe set -d %DRV_ROOT%\Flags 0x12
regtool.exe set -d %DRV_ROOT%\UserProcGroup 0x0223

REM ==============================================================

set DRV_PROC_GRP_ROOT=\HKEY_LOCAL_MACHINE\Drivers\ProcGroup_0223

regtool.exe add -p %DRV_PROC_GRP_ROOT%

regtool.exe set -s %DRV_PROC_GRP_ROOT%\ProcName "udevice.exe"
regtool.exe set -s %DRV_PROC_GRP_ROOT%\ProcVolPrefix "$udevice"



