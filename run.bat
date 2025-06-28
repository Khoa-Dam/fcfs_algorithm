@echo off
g++ fcfs_win.cpp -o fcfs_win.exe -mwindows -lgdi32
if %errorlevel% equ 0 (
    echo Compilation successful! Running program...
    start fcfs_win.exe
) else (
    echo Compilation failed!
    pause
) 