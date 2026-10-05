@echo off
chcp 65001
g++ -std=c++17 -o AIVocab AIVocab.cpp
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b
)
echo Running Math NLP Bot...
AIVocab.exe
pause
