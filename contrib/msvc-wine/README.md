All the scripts are based off of from j0y's [differential testing utility](https://github.com/j0y/gta-reversed-diff-test). Thank you!

> [!IMPORTANT]
> This is for building **Windows versions** of the ASI using Linux. It's meant for developers using Linux only!

### 0. Requirements
python, cmake, msiextract

### 1. Clone msvc-wine and install VS
> [!NOTE]
> You *should* install into `/opt/msvc`, or comply to following notes.
>
> Also don't forget to own the dir since `/opt` is typically owned by `root`.

`./vsdownload.py --accept-license --dest /opt/msvc`

### 2. Install the MSVC wrappers
```
wineboot --init
wineserver --wait
./install.sh /opt/msvc
wineserver --wait
```

### 3. Patch the wrappers by running the `patch-wrappers.sh` script.
> [!NOTE]
> You need to modify the path if you haven't installed MSVC into `/opt/msvc`!

Check if the `cl` wrapper works: `/opt/msvc/bin/x86/cl 2>&1 | head -3`. You should see the MS branding title.

### 4. Install Conan using `pipx` or preferably your distro's package manager.
`pipx install "conan>=2.0,<3.0"`

### 5. Let Conan detect your profile
`conan profile detect --force`

### 6. Test the installed toolchain
`./test-toolchain.sh`

### 7. Copy `toolchain-msvc-wine.cmake` into the `/opt/msvc` directory:
`cp toolchain-msvc-wine.cmake /opt/msvc`

### 8. Copy the profile into the project root directory:
`cp conanprofile-wine.txt ../..`

### 9. Profit
You should be able to run `python setup.py` to setup and build with the `--build` parameter.
