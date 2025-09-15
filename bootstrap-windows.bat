@echo off
if not exist vcpkg\ (
    echo "Please run git submodule init before executing the bootstrap file."
    exit /b 0
)

:: TODO check bootstrap already done (Check VCPKG_ROOT is set ? Could be by an external process).

cd vcpkg 
call bootstrap-vcpkg.bat

set "VCPKG_ROOT=%cd%"
:: Add first here to override potentially available global vcpkg
set "PATH=%VCPKG_ROOT%;%PATH%"

:: TODO need to download overlay port aka and geometry.

echo "Bootstrap done."

cd ..