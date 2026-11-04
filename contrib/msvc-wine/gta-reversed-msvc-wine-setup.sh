#!/bin/bash
set -euo pipefail

echo '==' installing dependencies
if pacman --version &> /dev/null; then
    sudo pacman -S --needed python cmake msitools wine python-pipx wget
else
    echo expected arch linux: install python, cmake, msiextract '(msitools)', wine, pipx, wget manually
    read -r -p "press enter to continue if you are sure you have all of them..."
fi

echo '==' installing msvc
mkdir -p ~/source
git clone https://github.com/mstorsjo/msvc-wine.git ~/source/msvc-wine --depth 1

sudo mkdir -p /opt/msvc
sudo chown "$USER":"$USER" /opt/msvc

cd ~/source/msvc-wine
python vsdownload.py --accept-license --dest /opt/msvc

wineboot --init
wineserver --wait
~/source/msvc-wine/install.sh /opt/msvc
wineserver --wait

/opt/msvc/bin/x86/cl 2>&1 | head -3

echo '==' installing conan
pipx install "conan>=2.0,<3.0"

conan profile detect --force

wget -O - "https://raw.githubusercontent.com/gta-reversed/gta-reversed/refs/heads/master/contrib/msvc-wine/test-toolchain.sh" | bash

wget -P /opt/msvc "https://raw.githubusercontent.com/gta-reversed/gta-reversed/refs/heads/master/contrib/msvc-wine/toolchain-msvc-wine.cmake"
