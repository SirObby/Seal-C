#!/bin/sh

# Define Colors
Esc="\e"
reset="${Esc}[m"
bold="${Esc}[1m"
red="${Esc}[31m"
green="${Esc}[32m"
yellow="${Esc}[33m"
blue="${Esc}[34m"
magenta="${Esc}[35m"
cyan="${Esc}[36m"
white="${Esc}[37m"
bold_red="${Esc}[1;31m"
bold_green="${Esc}[1;32m"
bold_yellow="${Esc}[1;33m"
bold_blue="${Esc}[1;34m"
bold_magenta="${Esc}[1;35m"
bold_cyan="${Esc}[1;36m"
bold_white="${Esc}[1;37m"

echo "${cyan}| Setting up ${magenta}...${reset}"
echo "${cyan}| Setting up ${blue}dependencies ${magenta}...${reset}"
mkdir libs >/dev/null 2>&1
cd libs
echo "${cyan}| Checking for updates ${blue}git pull ${magenta}...${reset}"
git pull
echo "${cyan}| Generating ${blue}cmake ${magenta}...${reset}"
mkdir build
cd build
cmake ..
echo "${cyan}| Building ${blue}cmake ${magenta}...${reset}"
make
echo "${cyan}| Finishing up${blue}${magenta}...${reset}"
cmake ..