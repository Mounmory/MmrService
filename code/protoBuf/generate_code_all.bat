@echo off  
setlocal enabledelayedexpansion  

set rootDir=%cd%
echo %rootDir%

for /d %%i in ("%rootDir%\proto\*") do (  
    echo deal proto file in folder: %%~nxi  
	call .\generate_one.bat %rootDir%\proto\%%~nxi %rootDir%\generate\%%~nxi
)

echo Done.  
pause