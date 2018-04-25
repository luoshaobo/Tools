Attribute VB_Name = "SMCodeGenBase"
Public Const OUTPUT_DIR_BASE As String = "tmp"
Public Const OUTPUT_FILE_HANDLE As Integer = 1
Public Const STENCIL_FILE_NAME As String = "StateMachineStencil.vss"
Public Const MASTER_NAME_STATE As String = "State"
Public Const MASTER_NAME_TRANSITION As String = "Transition"
Public Const PAGE_NAME_PREFIX_TO_BE_IGNORED = "#"
Public Const NEW_LINE = vbCrLf

Public bFileOpened As Boolean
Public sOutPutFilePath As String
Public oCurrentPage As Object

Sub A_000_ProcessAllPagesAndGenerateCppFiles()
    Dim oPage As Object
    Dim nIgnoredPageCount As Integer
    
    nIgnoredPageCount = 0
    
    For Each oPage In Application.ActiveDocument.Pages
        Set oCurrentPage = oPage
        
        If Not IsPageIgnored(oCurrentPage.Name) Then
            PreProcessPage
            ProcessPage
            PostProcessPage
        Else
            nIgnoredPageCount = nIgnoredPageCount + 1
        End If
    Next
    
    GenerateCppCode
    
    MsgBox "" & nIgnoredPageCount & " page(s) ignored." _
        + vbCrLf + "XML files for all of the pages are generated in [" + Application.ActiveDocument.Path + GetOutputDir() + "]." _
        + vbCrLf + "CPP files are generated in [" + Application.ActiveDocument.Path + GetOutputDir() + "\GeneratedCode]." _
        + vbCrLf + "Log files are located in [" + Application.ActiveDocument.Path + GetOutputDir() + "]." _
        , vbOKOnly + vbInformation, "Notification"
End Sub

Sub A_001_ProcessCurrentPage()
    Set oCurrentPage = Application.ActiveWindow.Page
    
    If Not IsPageIgnored(oCurrentPage.Name) Then
        PreProcessPage
        ProcessPage
        PostProcessPage
        
        MsgBox "XML file for the current page is generated in [" + Application.ActiveDocument.Path + GetOutputDir() + "].", _
            vbOKOnly + vbInformation, "Notification"
    Else
        MsgBox "XML file for the current page is ignored to generate.", _
            vbOKOnly + vbExclamation, "Notification"
    End If
End Sub

Sub GenerateCppCode()
    Dim sGenerateCodeIncContent As String
    
    sGenerateCodeIncContent = "set PAGE_NAMES="
    For Each oPage In Application.ActiveDocument.Pages
        If Not IsPageIgnored(oPage.Name) Then
            sGenerateCodeIncContent = sGenerateCodeIncContent + oPage.Name + " "
        End If
    Next
    Shell "cmd /c echo " + sGenerateCodeIncContent + " >" + GetCurDocNameWithoutExt() + "_inc.bat", vbHide

    Shell "cmd /c " + GetCurDocNameWithoutExt() + ".bat >" + GetOutputDir() + "\log.txt 2>" + GetOutputDir() + "\log_err.txt", vbHide
End Sub

Sub PreProcessPage()
    bFileOpened = False
    sOutPutFilePath = GetOutputDir() + "\" + oCurrentPage.Name + ".xml"
    
    OpenFileToWrite
End Sub

Sub PostProcessPage()
    CloseFileToWrite
End Sub

Sub ProcessPage()
    Dim oShapes As Object
    Dim oShape As Object
    Dim oCell As Object
    
    Dim sContentText As String
    
    WriteToFile "<AllShapes>" + NEW_LINE
    
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
        
            WriteToFile "  <Shape Type=""State"">" + NEW_LINE
            WriteToFile "    <PinX>" + sPinX + "</PinX>" + NEW_LINE
            WriteToFile "    <PinY>" + sPinY + "</PinY>" + NEW_LINE
            WriteToFile "    <Width>" + sWidth + "</Width>" + NEW_LINE
            WriteToFile "    <Height>" + sHeight + "</Height>" + NEW_LINE
            WriteToFile "    <Text>" + EscapeXmlText(sContentText) + "</Text>" + NEW_LINE
            WriteToFile "  </Shape>" + NEW_LINE
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
        
            WriteToFile "  <Shape Type=""Transition"">" + NEW_LINE
            WriteToFile "    <BeginX>" + sBeginX + "</BeginX>" + NEW_LINE
            WriteToFile "    <BeginY>" + sBeginY + "</BeginY>" + NEW_LINE
            WriteToFile "    <EndX>" + sEndX + "</EndX>" + NEW_LINE
            WriteToFile "    <EndY>" + sEndY + "</EndY>" + NEW_LINE
            WriteToFile "    <Text>" + EscapeXmlText(sContentText) + "</Text>" + NEW_LINE
            WriteToFile "  </Shape>" + NEW_LINE
        Else
            MsgBox "Unknown shape!", vbOKOnly + vbExclamation, "Warning"
        End If
    Next
    
    WriteToFile "</AllShapes>" + NEW_LINE
End Sub

Function EscapeXmlText(sOrig As String) As String
    Dim sResult As String
    
    Dim sCh As String

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

Function OpenFileToWrite() As Boolean
    Dim fso As Object
    
    Set fso = CreateObject("Scripting.FileSystemObject")
    If fso.FileExists(sOutPutFilePath) Then
        Kill sOutPutFilePath
    End If
    
    Open sOutPutFilePath For Binary As #OUTPUT_FILE_HANDLE
    On Error GoTo ERR
    
    bFileOpened = True
    OpenFileToWrite = True
    Exit Function
ERR:
    OpenFileToWrite = False
    Exit Function
End Function

Function CloseFileToWrite() As Boolean
    bFileOpened = False
    Close #OUTPUT_FILE_HANDLE
    On Error GoTo ERR
    
    CloseFileToWrite = True
    Exit Function
ERR:
    CloseFileToWrite = False
    Exit Function
End Function

Function WriteToFile(sText As String) As Boolean
    If Not bFileOpened Then
        WriteToFile = False
        Exit Function
    End If
    
    Put #OUTPUT_FILE_HANDLE, , sText
    On Error GoTo ERR
    
    WriteToFile = True
    Exit Function
ERR:
    WriteToFile = False
    Exit Function
End Function

Function IsTransition(sTransName As String) As Boolean
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

Function IsPageIgnored(sPageName As String) As Boolean
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

Function GetCurDocNameWithoutExt() As String
    Dim sDocName As String
    Dim sDocNameWithoutExt As String
    Dim nDotPos As Integer
    
    sDocName = Application.ActiveDocument.Name
    nDotPos = InStr(sDocName, ".")
    sDocNameWithoutExt = Mid(sDocName, 1, nDotPos - 1)
    
    GetCurDocNameWithoutExt = sDocNameWithoutExt
    Exit Function
End Function

Function GetOutputDir() As String
    Dim sOutputDir As String
    
    sOutputDir = OUTPUT_DIR_BASE + "\" + GetCurDocNameWithoutExt()
    
    If Dir(sOutputDir, vbDirectory) = "" Then
        MkDir sOutputDir
    End If
    
    GetOutputDir = sOutputDir
    Exit Function
End Function
