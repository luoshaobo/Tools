@echo off

if /I "%1" == "tk" (
    echo "Build code with TK..."
    cd "D:\casdev\WinCE\public\MMP_PROD\_NAV\Main.tk\AccessLib"
    build -c
) else (
    echo "Build original code"
    cd "D:\casdev\WinCE\public\MMP_PROD\_NAV\Main\AccessLib"
    build -c
)