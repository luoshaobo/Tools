@echo off
if "%1" == "" (
    set sub_title=
) else (
    set sub_title=%1
)
if not "%1" == "" (
    title Customed Cygwin - %sub_title%
)
chdir /d D:\cygwin_root\bin
bash --login -i
