Option Explicit
        
Dim wshShell, Shortcut
Dim strDir, strName, strTargetDir, strTarget

Sub usage()
    WScript.echo "Usage:"
    WScript.echo "    create_win_shortcut.vbs <shortcut_dir> <shortcut_name> <target_dir> <target_name>"
end Sub

if WScript.Arguments.Count < 4 then
    usage
    WScript.Quit(1)
end if

'strName = "build_DPCA_module_dev__xxx.bat"
'strDir = "D:\lsb\links"
'strTargetDir = "R:\casdev\DPCA_C1.B4.08\C1.B4.08\DPCA_iMX\Delivery\MMP_GENERIC\tools2"
'strTarget = strTargetDir & "\build_DPCA_module_dev__xxx.bat"

strDir = WScript.Arguments(0)
strName = WScript.Arguments(1)
strTargetDir = WScript.Arguments(2)
strTarget = strTargetDir & "\" & WScript.Arguments(3)

REM WScript.echo strDir
REM WScript.echo strName
REM WScript.echo strTargetDir
REM WScript.echo strTarget

Set wshShell = WSH.CreateObject("WScript.Shell")
Set Shortcut = wshShell.CreateShortcut(strDir & "\" & strName & ".lnk")
Shortcut.TargetPath = strTarget
Shortcut.WindowStyle = 1
Shortcut.WorkingDirectory = strTargetDir
Shortcut.Save
Set Shortcut = Nothing
Set wshShell = Nothing
