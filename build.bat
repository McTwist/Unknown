@echo off
:: Call this to build the program

:: Set output variable
set OUTPUT="build/"

:: Create folder
md %OUTPUT%

:: Go to folder
cd %OUTPUT%

:: Make the required files
cmake ..\src

:: Note: Currently not sure how to build automatically, but it is sure possible

:: Good to have if in a shell
cd ..
