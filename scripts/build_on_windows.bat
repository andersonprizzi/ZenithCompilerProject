@echo off

if not exist build (
    mkdir build
)

echo Building project.
gcc -I headers -o build/main sources/*.c
echo Build successful.