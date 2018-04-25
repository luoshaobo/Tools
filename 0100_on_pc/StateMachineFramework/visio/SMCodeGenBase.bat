REM
REM NOTE: set the variable PAGE_NAMES with the page names.
REM
call %VSD_FILE_NAME_WITHOUT_EXT%_inc.bat

REM
REM NOTE: set the paths to output.
REM
set OUTPUT_ROOT_DIR=%PARENT_OF_OUTPUT_ROOT_DIR%\%VSD_FILE_NAME_WITHOUT_EXT%
set OUTPUT_XML_DIR=%OUTPUT_ROOT_DIR%
set OUTPUT_CPP_DIR=%OUTPUT_ROOT_DIR%\GeneratedCode
set OUTPUT_LOG_DIR=%OUTPUT_ROOT_DIR%

REM
REM NOTE: calculate all of the generated XML file paths. 
REM
set ALL_PAGE_XML_OUTPUT_FILES=
call :MAKE_ALL_PAGE_XML_OUTPUT_FILES %PAGE_NAMES%

REM
REM NOTE: clean and make directories.
REM
if not exist %OUTPUT_ROOT_DIR% (
    mkdir %OUTPUT_ROOT_DIR%
)
if not exist %OUTPUT_XML_DIR% (
    mkdir %OUTPUT_XML_DIR%
)
if exist %OUTPUT_CPP_DIR% (
    del /F /S /Q %OUTPUT_CPP_DIR%\*.*
)
if not exist %OUTPUT_CPP_DIR% (
    mkdir %OUTPUT_CPP_DIR%
)
if not exist %OUTPUT_LOG_DIR% (
    mkdir %OUTPUT_LOG_DIR%
)

REM
REM NOTE: generate CPP files.
REM
SMCodeGen %INSTANCE_COUNT% %APP_PREFIX% %OUTPUT_CPP_DIR% %OUTPUT_XML_DIR%\dump.xml %ALL_PAGE_XML_OUTPUT_FILES%

goto :eof

:MAKE_ALL_PAGE_XML_OUTPUT_FILES
    shift
    if /I "%0"=="" exit /b 0
    set ALL_PAGE_XML_OUTPUT_FILES=%ALL_PAGE_XML_OUTPUT_FILES% %OUTPUT_XML_DIR%\%0.xml
    goto :MAKE_ALL_PAGE_XML_OUTPUT_FILES
