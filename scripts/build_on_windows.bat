@echo off

if not exist build (
    mkdir build
)

echo Building project.
gcc -I headers -o build/main sources/*.c

if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)

echo Build successful.