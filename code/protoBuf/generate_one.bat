rem %1:proto文件路径
rem %2:生成源码路径

@echo off  
setlocal enabledelayedexpansion  

rem 设置源文件夹和目标文件夹  
set SOURCE_FOLDER=%1
set OUTPUT_FOLDER=%2

if not exist "%OUTPUT_FOLDER%" mkdir "%OUTPUT_FOLDER%" 

echo process proto file in folder %SOURCE_FOLDER%
echo generate source file to %OUTPUT_FOLDER%

for /R "%SOURCE_FOLDER%" %%F in (*.proto) do (
    echo process file: %%~nxF
    rem 使用proto生成源码
	protoc -I %SOURCE_FOLDER% %%~nxF --cpp_out=%OUTPUT_FOLDER%
	rem protoc --java_out=./java --java_multiple_files ./example.proto
)

