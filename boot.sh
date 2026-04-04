#!/bin/bash

#Clean old bin and logs
echo "Cleaning old build..."
rm -rf bin/ 
rm -rf logs/

#Create new folders
echo "Creating new directories..."
mkdir bin/ 
mkdir logs/

#Compile
echo "Compiling logger..."
gcc src/logger.c -o bin/logger
if [ $? -eq 0 ]; then
    echo "Logger compiled successfully!"
else
    echo "Logger compilation failed!"
    exit 1
fi

echo "Compiling File Manager Module..."
gcc src/file_management.c -o bin/file_management
if [ $? -eq 0 ]; then
    echo "File manager compiled successfully!"
else
    echo "File manager compilation failed!"
    exit 1
fi

echo "Compiling Peterson's Solution Module..."
gcc src/peterson.c -o bin/peterson
if [ $? -eq 0 ]; then
    echo "Peterson's Solution compiled successfully!"
else
    echo "Peterson's Solution compilation failed!"
    exit 1
fi

echo "Compiling Memory Allocation Module..."
gcc src/memory.c -o bin/memory
if [ $? -eq 0 ]; then
    echo "Memory Allocation compiled successfully!"
else
    echo "Memory Allocation compilation failed!"
    exit 1
fi

echo "Compiling Amdahl's Law Module..."
gcc src/amdahl.c -o bin/amdahl
if [ $? -eq 0 ]; then
    echo "Amdahl's Law compiled successfully!"
else
    echo "Amdahl's Law compilation failed!"
    exit 1
fi

echo "Compiling Main Module..."
gcc src/main_menu.c -o bin/main_menu
if [ $? -eq 0 ]; then
    echo "Main Module compiled successfully!"
else
    echo "Main Module compilation failed!"
    exit 1
fi

echo "Starting Demonstration..."
cd bin 
./main_menu




