#!/bin/bash

# Color codes
GREEN='\033[0;32m'
CYAN='\033[0;36m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

echo -e "${CYAN}================================${RESET}"
echo -e "${CYAN}   CSCI 3431 - OS Project Boot  ${RESET}"
echo -e "${CYAN}================================${RESET}\n"

#Clean old bin and logs and shared memory resources
echo -e "${YELLOW}Cleaning old build...${RESET}"
rm -rf bin/ 
rm -rf logs/
rm -f /tmp/os_log_pipe
ipcrm -M 0x4C4F4720 2>/dev/null

#Create new folders
echo -e "${YELLOW}Creating new directories...${RESET}\n"
mkdir bin/ 
mkdir logs/

echo -e "${YELLOW}Compiling modules...${RESET}"

#Compile
echo "Compiling logger..."
gcc src/logger.c -o bin/logger
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓ Logger compiled successfully!${RESET}"
else
    echo -e "  ${RED}✗ Logger failed${RESET}"
    exit 1
fi


echo "Compiling File Manager Module..."
gcc src/file_management.c -o bin/file_management
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓ File manager compiled successfully!${RESET}"
else
    echo -e "  ${RED}✗ File manager compilation failed!${RESET}"
    exit 1
fi


echo "Compiling Peterson's Solution Module..."
gcc -w src/peterson.c -o bin/peterson
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓ Peterson's Solution compiled successfully!${RESET}"
else
    echo -e "  ${RED}✗ Peterson's Solution compilation failed!${RESET}"
    exit 1
fi


echo "Compiling Memory Allocation Module..."
gcc -w src/memory.c -o bin/memory
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓ Memory Allocation compiled successfully!${RESET}"
else
    echo -e "  ${RED}✗ Memory Allocation compilation failed!${RESET}"
    exit 1
fi


echo "Compiling Amdahl's Law Module..."
gcc src/amdahl.c -o bin/amdahl
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓ Amdahl's Law compiled successfully!${RESET}"
else
    echo -e "  ${RED}✗ Amdahl's Law compilation failed!${RESET}"
    exit 1
fi


echo "Compiling Main Module..."
gcc src/main_menu.c -o bin/main_menu
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓ Main Module compiled successfully!${RESET}"
else
    echo -e "  ${RED}✗ Main Module compilation failed!${RESET}"
    exit 1
fi

echo "Starting Demonstration..."
cd bin 
./main_menu




