#!/bin/bash

echo "[1] Arch, [2] Debian-based, [3] Fedora"
read op

if [ $op == 1 ]; then
	echo -e "\e[32mInstalling SDL2libraries\e[39m"
	sudo pacman -S sdl2
	echo -e "\e[32mBuilding chip8 interpreter...\e[39m"
	make
elif [ $op == 2 ]; then
	echo -e "\e[32mInstalling SDL2 Dev libraries\e[39m"
	sudo apt install libsdl2-dev
	echo -e "\e[32mBuilding chip8 interpreter...\e[39m"
	make
elif [ $op == 3 ]; then
	echo -e "\e[32mInstalling SDL2 Dev libraries\e[39m"
	sudo yum install SDL2-devel
	echo -e "\e[32mBuilding chip8 interpreter...\e[39m"
	make
else
	echo "Wrong option"
fi


