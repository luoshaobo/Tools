set DRV_ROOT=\HKEY_LOCAL_MACHINE\Drivers\TestDrives\STD_DRIVER

regtool.exe add -p %DRV_ROOT%

regtool.exe set -s %DRV_ROOT%\Dll "STD_DRV.dll"
regtool.exe set -s %DRV_ROOT%\Prefix "STD"
regtool.exe set -d %DRV_ROOT%\Index 0x1
regtool.exe set -d %DRV_ROOT%\Order 0x0
regtool.exe set -s %DRV_ROOT%\FriendlyName "Standard Driver"
regtool.exe set -d %DRV_ROOT%\Ioctl 0x0

regtool.exe set -d %DRV_ROOT%\Flags 0x12
regtool.exe set -d %DRV_ROOT%\UserProcGroup 0x0222

REM ==============================================================

set DRV_PROC_GRP_ROOT=\HKEY_LOCAL_MACHINE\Drivers\ProcGroup_0222

regtool.exe add -p %DRV_PROC_GRP_ROOT%

regtool.exe set -s %DRV_PROC_GRP_ROOT%\ProcName "STD_DRV.exe"
regtool.exe set -s %DRV_PROC_GRP_ROOT%\ProcVolPrefix "$udevice"

