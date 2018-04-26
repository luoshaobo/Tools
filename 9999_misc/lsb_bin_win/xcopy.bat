@echo off

if "%1"=="/?" (
    echo Usage:
    echo     %0 src_dir dst_dir
    echo
    goto :end
)

rem echo ####arg1 %1
rem echo ####arg2 %2

if not exist %1\ (
    echo *** Error: the source direcory %1 does not exist.
    goto :end
)

if not exist %2\ (
    mkdir %2
)

if not exist %2\ (
    echo *** Error: the destination direcory %2 can not be created.
    goto :end
)

copy %1\* %2

FOR /D %%d IN (%1\*) DO (
    rem echo ###item %%d
    rem echo ###dir_name %%~nd
    rem echo ###dst_dir %2\%%~nd
    
    mkdir %2\%%~nd
    if not exist %2\%%~nd\ (
        echo *** Error: the destination direcory %2\%%~nd can not be created.
        goto :end
    )
    
    call %0 %1\%%~nd %2\%%~nd
)

:end
exit /b

