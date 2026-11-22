All the scripts are based off of from j0y's [differential testing utility](https://github.com/j0y/gta-reversed-diff-test). Thank you!

> [!IMPORTANT]
> This is for building **Windows versions** of the ASI using Linux. It's meant for developers using Linux only!

> [!NOTE]
> You can use `gta-reversed-msvc-wine-setup.sh` script to do these automatically. You **must** check its behavior though, **no guarantees**!

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

Check if the `cl` wrapper works: `/opt/msvc/bin/x86/cl 2>&1 | head -3`. You should see the MS branding title.

### 3. Install Conan using `pipx` or preferably your distro's package manager.
`pipx install "conan>=2.0,<3.0"`

### 4. Let Conan detect your profile
`conan profile detect --force`

### 5. Test the installed toolchain
`./test-toolchain.sh`

### 6. Copy `toolchain-msvc-wine.cmake` into the `/opt/msvc` directory:
`cp toolchain-msvc-wine.cmake /opt/msvc`

### 7. Profit
You should be able to run `python setup.py` to setup and build with the `--build` parameter.
