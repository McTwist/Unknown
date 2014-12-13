@echo off

:: Move to the correct folder
cd src

:: Set up some variables
set ZIP_NAME=Unknown.zip

:: Run the command
7z a -mx0 -tzip -r ..\%ZIP_NAME% . -x!*.txt

:: Move back to avoid confusion
cd ..
