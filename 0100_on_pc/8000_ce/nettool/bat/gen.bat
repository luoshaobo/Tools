INPUT_XML_DIR=d:

set INPUT_XML_FILES=
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Root.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type0sSession.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type0cSession.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type1sSession.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type1cSession.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type2sSession.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type2cSession.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type2sSession_Main.xml
set INPUT_XML_FILES=%INPUT_XML_FILES% %INPUT_XML_DIR%\Type2cSession_Main.xml

REM ..\SMCodeGen\Debug\SMCodeGen NT .\tmp\ d:\dump.xml %INPUT_XML_FILES%

del /F /S /Q .\tmp\*.*

..\SMCodeGen\bin\SMCodeGen NT .\tmp\ d:\dump.xml %INPUT_XML_FILES%
