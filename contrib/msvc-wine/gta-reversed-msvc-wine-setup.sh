#!/bin/bash
set -euo pipefail
export current_user=$(whoami)

setup_wineprefix(){
    wineboot --init
}

fix_opt_perms(){
    su -c 'chown -R ${current_user}:${current_user} /opt/msvc'
}

echo '==' installing dependencies
if pacman --version &> /dev/null; then
    su -c 'pacman -S --needed python cmake msitools wine python-pipx curl'
else
    echo expected arch linux: install python, cmake, msiextract '(msitools)', wine, pipx, curl manually
    read -r -p "press enter to continue if you are sure you have all of them..."
fi

if /opt/msvc/bin/x86/cl 2>&1 | head -3; then # don't need to compile it again if it's already there!
    echo "msvc installed and working!"
elif command -v yay >/dev/null 2>&1; then # also don't need to clone if on arch linux and aur helper is present
    echo '==' msvc not found, installing from aur...
    yay -S aur/msvc-wine-git
elif command -v paru >/dev/null 2>&1; then
    echo '==' msvc not found, installing from aur...
    paru -S aur/msvc-wine-git
else
    echo '==' msvc not found, cloning and installing... # should probably install the AUR package manually instead of this
    mkdir -p ./source
    git clone https://github.com/mstorsjo/msvc-wine.git ~/source/msvc-wine --depth 1
    su -c 'mkdir -p /opt/msvc'
    fix_opt_perms

    cd ./source/msvc-wine
    python vsdownload.py --accept-license --dest /opt/msvc

    setup_wineprefix
    wineserver --wait
    ./source/msvc-wine/install.sh /opt/msvc
    wineserver --wait
fi



#/opt/msvc/bin/x86/cl 2>&1 | head -3
if command -v conan >/dev/null 2>&1; then
    echo 'conan already installed!'
else
    echo '==' installing conan
    pipx install "conan>=2.0,<3.0"
fi

fix_opt_perms

conan profile detect --force

# Switched from wget to curl as it's more likely to already be included!
curl -fsSL "https://raw.githubusercontent.com/gta-reversed/gta-reversed/refs/heads/master/contrib/msvc-wine/test-toolchain.sh" | sh # probably symlinks to bash anyways!
curl -fsSL -o /opt/msvc/toolchain-msvc-wine.cmake "https://raw.githubusercontent.com/gta-reversed/gta-reversed/refs/heads/master/contrib/msvc-wine/toolchain-msvc-wine.cmake"
