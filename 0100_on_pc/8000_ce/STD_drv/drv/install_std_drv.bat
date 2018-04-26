set DRV_ROOT=\HKEY_LOCAL_MACHINE\Drivers\TestDrives\STD_DRIVER

regtool.exe add -p %DRV_ROOT%

regtool.exe set -s %DRV_ROOT%\Dll "STD_DRV.dll"
regtool.exe set -s %DRV_ROOT%\Prefix "STD"
regtool.exe set -d %DRV_ROOT%\Index 0x1
regtool.exe set -d %DRV_ROOT%\Order 0x0
regtool.exe set -s %DRV_ROOT%\FriendlyName "Standard Driver"
regtool.exe set -d %DRV_ROOT%\Ioctl 0x0
