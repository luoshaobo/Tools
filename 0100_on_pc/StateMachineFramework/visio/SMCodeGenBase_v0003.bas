Attribute VB_Name = "SMCodeGenBase"
Option Explicit

Private Const OUTPUT_DIR_BASE As String = "tmp"
Private Const STENCIL_FILE_NAME As String = "StateMachineStencil.vss"
Private Const MASTER_NAME_STATE As String = "State"
Private Const MASTER_NAME_TRANSITION As String = "Transition"
Private Const PAGE_NAME_PREFIX_TO_BE_IGNORED = "#"
Private Const NEW_LINE = vbCrLf

Public Sub A_000_ProcessAllPagesAndGenerateCppFiles()
    Dim oPage As Object
    Dim nIgnoredPageCount As Integer
    Dim sOutputDir As String
    Dim oCurrentPage As Object
    
    sOutputDir = GetOutputDir()
    
    If Dir(OUTPUT_DIR_BASE, vbDirectory) = "" Then
        MsgBox "The base directory " + OUTPUT_DIR_BASE + "does not exist.", vbCritical + vbOKOnly, "Error"
        Exit Sub
    End If
    
    If Dir(sOutputDir, vbDirectory) = "" Then
        MsgBox "Failded to create the directory: " + GetOutputDir(), vbCritical + vbOKOnly, "Error"
        Exit Sub
    End If
    
    nIgnoredPageCount = 0
    For Each oPage In Application.ActiveDocument.Pages
        Set oCurrentPage = oPage
        
        If Not IsPageIgnored(oCurrentPage.Name) Then
            If Not ProcessPage(oCurrentPage) Then
                MsgBox "Failded to generate XML for the page: " + oCurrentPage.Name, vbCritical + vbOKOnly, "Error"
                Exit Sub
            End If
        Else
            nIgnoredPageCount = nIgnoredPageCount + 1
        End If
    Next
    
    If GenerateCppCode() Then
        MsgBox "" & nIgnoredPageCount & " page(s) ignored." _
            + vbCrLf + "XML files for all of the pages are generated in [" + Application.ActiveDocument.Path + GetOutputDir() + "]." _
            + vbCrLf + "CPP files are generated in [" + Application.ActiveDocument.Path + GetOutputDir() + "\GeneratedCode]." _
            + vbCrLf + "Log files are located in [" + Application.ActiveDocument.Path + GetOutputDir() + "]." _
            , vbOKOnly + vbInformation, "Info"
    End If
End Sub

Sub A_001_ProcessCurrentPage()
    Dim sOutputDir As String
    Dim oCurrentPage As Object
    
    sOutputDir = GetOutputDir()

    If Dir(OUTPUT_DIR_BASE, vbDirectory) = "" Then
        MsgBox "The base directory " + OUTPUT_DIR_BASE + "does not exist.", vbCritical + vbOKOnly, "Error"
        Exit Sub
    End If
    
    If Dir(sOutputDir, vbDirectory) = "" Then
        MsgBox "Failded to create directory: " + GetOutputDir(), vbCritical + vbOKOnly, "Error"
        Exit Sub
    End If

    Set oCurrentPage = Application.ActiveWindow.Page
    
    If Not IsPageIgnored(oCurrentPage.Name) Then
        If Not ProcessPage(oCurrentPage) Then
            MsgBox "Failded to generate XML for the page: " + oCurrentPage.Name, vbCritical + vbOKOnly, "Error"
            Exit Sub
        End If
        
        MsgBox "XML file for the current page is generated in [" + Application.ActiveDocument.Path + GetOutputDir() + "].", _
            vbOKOnly + vbInformation, "Info"
    Else
        MsgBox "XML file for the current page is ignored to generate.", _
            vbOKOnly + vbExclamation, "Warn"
    End If
End Sub

Private Function GenerateCppCode() As Boolean
    Dim sGenerateCodeIncContent As String
    Dim sLogErrFilePath As String
    Dim sLogErrFileContent As String
    Dim oPage As Object
    
    sGenerateCodeIncContent = "set PAGE_NAMES="
    For Each oPage In Application.ActiveDocument.Pages
        If Not IsPageIgnored(oPage.Name) Then
            sGenerateCodeIncContent = sGenerateCodeIncContent + oPage.Name + " "
        End If
    Next
    SaveFileContent sGenerateCodeIncContent, GetCurDocNameWithoutExt() + "_inc.bat"
    If Not FileExists(GetCurDocNameWithoutExt() + "_inc.bat") Then
        MsgBox "Failded to create the file: " + GetCurDocNameWithoutExt() + "_inc.bat", vbCritical + vbOKOnly, "Error"
        GenerateCppCode = False
        Exit Function
    End If

    If Not FileExists(GetCurDocNameWithoutExt() + ".bat") Then
        MsgBox "The file: " + GetCurDocNameWithoutExt() + ".bat does no exist.", vbCritical + vbOKOnly, "Error"
        GenerateCppCode = False
        Exit Function
    End If
    RunCmdLine "cmd /c " + GetCurDocNameWithoutExt() + ".bat >" + GetOutputDir() + "\log.txt 2>" + GetOutputDir() + "\log_err.txt"
    
    sLogErrFilePath = GetOutputDir() + "\log_err.txt"
    If FileExists(sLogErrFilePath) Then
        sLogErrFileContent = GetFileContent(sLogErrFilePath)
        If Len(sLogErrFileContent) > 0 Then
            RunGuiExe "notepad " + sLogErrFilePath
        End If
    End If
    
    GenerateCppCode = True
    Exit Function
End Function

Private Function ProcessPage(oCurrentPage As Object) As Boolean
    Dim oShapes As Object
    Dim oShape As Object
    Dim oCell As Object
    Dim sPinX As String
    Dim sPinY As String
    Dim sWidth As String
    Dim sHeight As String
    Dim sBeginX As String
    Dim sBeginY As String
    Dim sEndX As String
    Dim sEndY As String
    Dim sContentText As String
    Dim sOutPutFilePath As String
    Dim fd As Object
    
    sOutPutFilePath = GetOutputDir() + "\" + oCurrentPage.Name + ".xml"
    
    Set fd = OpenFileToWrite(sOutPutFilePath)
    If fd Is Nothing Then
        ProcessPage = False
        Exit Function
    End If
    
    WriteToFile fd, "<AllShapes>" + NEW_LINE
    
    Set oShapes = oCurrentPage.Shapes
    For Each oShape In oShapes
        If oShape.Master.NameU = MASTER_NAME_STATE Then
            Set oCell = oShape.CellsSRC(visSectionObject, visRowXFormOut, visXFormPinX)
            sPinX = oCell.ResultStr("mm")
            Set oCell = oShape.CellsSRC(visSectionObject, visRowXFormOut, visXFormPinY)
            sPinY = oCell.ResultStr("mm")
            Set oCell = oShape.CellsSRC(visSectionObject, visRowXFormOut, visXFormWidth)
            sWidth = oCell.ResultStr("mm")
            Set oCell = oShape.CellsSRC(visSectionObject, visRowXFormOut, visXFormHeight)
            sHeight = oCell.ResultStr("mm")
            sContentText = oShape.Text
        
            WriteToFile fd, "  <Shape Type=""State"">" + NEW_LINE
            WriteToFile fd, "    <PinX>" + sPinX + "</PinX>" + NEW_LINE
            WriteToFile fd, "    <PinY>" + sPinY + "</PinY>" + NEW_LINE
            WriteToFile fd, "    <Width>" + sWidth + "</Width>" + NEW_LINE
            WriteToFile fd, "    <Height>" + sHeight + "</Height>" + NEW_LINE
            WriteToFile fd, "    <Text>" + EscapeXmlText(sContentText) + "</Text>" + NEW_LINE
            WriteToFile fd, "  </Shape>" + NEW_LINE
        ElseIf IsTransition(oShape.Master.NameU) Then
            Set oCell = oShape.CellsU("BeginX")
            sBeginX = oCell.ResultStr("mm")
            Set oCell = oShape.CellsU("BeginY")
            sBeginY = oCell.ResultStr("mm")
            Set oCell = oShape.CellsU("EndX")
            sEndX = oCell.ResultStr("mm")
            Set oCell = oShape.CellsU("EndY")
            sEndY = oCell.ResultStr("mm")
            sContentText = oShape.Text
        
            WriteToFile fd, "  <Shape Type=""Transition"">" + NEW_LINE
            WriteToFile fd, "    <BeginX>" + sBeginX + "</BeginX>" + NEW_LINE
            WriteToFile fd, "    <BeginY>" + sBeginY + "</BeginY>" + NEW_LINE
            WriteToFile fd, "    <EndX>" + sEndX + "</EndX>" + NEW_LINE
            WriteToFile fd, "    <EndY>" + sEndY + "</EndY>" + NEW_LINE
            WriteToFile fd, "    <Text>" + EscapeXmlText(sContentText) + "</Text>" + NEW_LINE
            WriteToFile fd, "  </Shape>" + NEW_LINE
        Else
            MsgBox "Unknown shape!", vbOKOnly + vbExclamation, "Warning"
        End If
    Next
    
    WriteToFile fd, "</AllShapes>" + NEW_LINE
    
    If Not CloseFileToWrite(fd) Then
        ProcessPage = False
        Exit Function
    End If
    
    ProcessPage = True
    Exit Function
End Function

Private Function EscapeXmlText(sOrig As String) As String
    Dim sResult As String
    Dim sCh As String
    Dim i As Integer

    For i = 1 To Len(sOrig)
        sCh = Mid(sOrig, i, 1)
        If sCh = "<" Then
            sResult = sResult & "&lt;"
        ElseIf sCh = ">" Then
            sResult = sResult & "&gt;"
        Else
            sResult = sResult & sCh
        End If
    Next
    
    EscapeXmlText = sResult
    Exit Function
End Function

Private Function FileExists(sFilePath As String) As Boolean
    Dim fso As Object
    
    Set fso = CreateObject("Scripting.FileSystemObject")
    If fso Is Nothing Then
        FileExists = False
        Exit Function
    End If
    
    FileExists = fso.FileExists(sFilePath)
    Exit Function
End Function

Private Function GetFileContent(sFilePath As String) As String
    Dim sFileContent As String
    Dim fso As Object
    Dim fd As Object
    
    Set fso = CreateObject("Scripting.FileSystemObject")
    If fso Is Nothing Then
        GetFileContent = ""
        Exit Function
    End If
    
    Set fd = fso.OpenTextFile(sFilePath, 1)
    If fd Is Nothing Then
        GetFileContent = ""
        Exit Function
    End If
    
    If fd.AtEndOfStream Then
        GetFileContent = ""
        Exit Function
    End If
    
    sFileContent = fd.ReadAll()
    
    GetFileContent = sFileContent
    Exit Function
End Function

Private Function SaveFileContent(sFileContent As String, sOutPutFilePath As String) As Boolean
    Dim fso As Object
    Dim fd As Object
    
    Set fso = CreateObject("Scripting.FileSystemObject")
    If fso Is Nothing Then
        SaveFileContent = False
        Exit Function
    End If
    
    If fso.FileExists(sOutPutFilePath) Then
        fso.DeleteFile sOutPutFilePath
    End If
    
    Set fd = fso.OpenTextFile(sOutPutFilePath, 2, True)
    If fd Is Nothing Then
        SaveFileContent = False
        Exit Function
    End If
    
    fd.write sFileContent
    fd.Close
    
    SaveFileContent = True
    Exit Function
End Function

Private Function OpenFileToWrite(sOutPutFilePath As String) As Object
    Dim fso As Object
    Dim fd As Object
    
    Set fso = CreateObject("Scripting.FileSystemObject")
    If fso Is Nothing Then
        Set OpenFileToWrite = Null
        Exit Function
    End If
    
    If fso.FileExists(sOutPutFilePath) Then
        fso.DeleteFile sOutPutFilePath
    End If
    
    Set fd = fso.OpenTextFile(sOutPutFilePath, 2, True)
    If fd Is Nothing Then
        Set OpenFileToWrite = Null
        Exit Function
    End If
    
    Set OpenFileToWrite = fd
    Exit Function
End Function

Private Function CloseFileToWrite(fd As Object) As Boolean
    If fd Is Nothing Then
        CloseFileToWrite = False
        Exit Function
    End If
    
    fd.Close

    CloseFileToWrite = True
    Exit Function
End Function

Private Function WriteToFile(fd As Object, sText As String) As Boolean
    If fd Is Nothing Then
        WriteToFile = False
        Exit Function
    End If
    
    fd.write sText

    WriteToFile = True
    Exit Function
End Function

Private Function IsTransition(sTransName As String) As Boolean
    Dim nTransPrefixLen As Integer
    Dim sTransPrefix As String
    
    nTransPrefixLen = Len(MASTER_NAME_TRANSITION)
    If Len(sTransName) < nTransPrefixLen Then
        IsTransition = False
        Exit Function
    End If
    
    sTransPrefix = Mid(sTransName, 1, nTransPrefixLen)
    If sTransPrefix = MASTER_NAME_TRANSITION Then
        IsTransition = True
        Exit Function
    End If
    
    IsTransition = False
    Exit Function
End Function

Private Function IsPageIgnored(sPageName As String) As Boolean
    Dim nIgnorePrefixLen As Integer
    Dim sFirstStr As String
    
    nIgnorePrefixLen = Len(PAGE_NAME_PREFIX_TO_BE_IGNORED)
    sFirstStr = Mid(sPageName, 1, nIgnorePrefixLen)
    
    If sFirstStr = PAGE_NAME_PREFIX_TO_BE_IGNORED Then
        IsPageIgnored = True
    Else
        IsPageIgnored = False
    End If
    
    Exit Function
End Function

Private Function GetCurDocNameWithoutExt() As String
    Dim sDocName As String
    Dim sDocNameWithoutExt As String
    Dim nDotPos As Integer
    
    sDocName = Application.ActiveDocument.Name
    nDotPos = InStr(sDocName, ".")
    sDocNameWithoutExt = Mid(sDocName, 1, nDotPos - 1)
    
    GetCurDocNameWithoutExt = sDocNameWithoutExt
    Exit Function
End Function

Private Function GetOutputDir() As String
    Dim sOutputDir As String
    
    If Dir(OUTPUT_DIR_BASE, vbDirectory) = "" Then
        MkDir OUTPUT_DIR_BASE
    End If
    
    sOutputDir = OUTPUT_DIR_BASE + "\" + GetCurDocNameWithoutExt()
    
    If Dir(sOutputDir, vbDirectory) = "" Then
        MkDir sOutputDir
    End If
    
    GetOutputDir = sOutputDir
    Exit Function
End Function

Private Function RunCmdLine(sCmdLine As String) As Boolean
    Dim objShell As Object
    Set objShell = CreateObject("WScript.Shell")
    objShell.Run sCmdLine, 0, True
    
    RunCmdLine = True
    Exit Function
End Function

Private Function RunGuiExe(sCmdLine As String) As Boolean
    Dim objShell As Object
    Set objShell = CreateObject("WScript.Shell")
    objShell.Run sCmdLine, 1, False
    
    RunGuiExe = True
    Exit Function
End Function
