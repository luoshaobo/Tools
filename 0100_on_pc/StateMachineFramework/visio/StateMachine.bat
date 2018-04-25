REM
REM NOTE: set the path to load SMCodeGen.exe
REM
set PATH=..\SMCodeGen\bin;%PATH%

REM
REM NOTE: set the vsd file without extension name.
REM
set VSD_FILE_NAME_WITHOUT_EXT=%~n0

REM
REM NOTE: set the parent path of the output root.
REM
set PARENT_OF_OUTPUT_ROOT_DIR=tmp

REM
REM NOTE: set the application prefix.
REM
set APP_PREFIX=APP

REM
REM NOTE: set the instance count of the state machines.
REM
set INSTANCE_COUNT=1

REM
REM NOTE: call the general bat file.
REM
call SMCodeGenBase.bat

goto :eof
