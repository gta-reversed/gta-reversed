## gtasa-reversed [![Build Status](https://github.com/gta-reversed/gta-reversed/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/gta-reversed/gta-reversed/actions?query=workflow%3ABuild)

A project to reverse Grand Theft Auto San Andreas completely, and to rewrite and document every function.

### Community
Please join our community Discord: [GTA Groupies](https://discord.gg/FG8XJ5Npqe) [The invite is permanent, feel free to share it!]

## About the project
Building this project will result in a DLL file that can be injected into GTA:SA using any ASI loader out there. After the DLL file has been injected, the ingame functions will be replaced by the reversed ones. The game will behave the same. Now if we make any changes to the reversed code, it will take effect in the game. The goal is to keep reversing until we have the entire game reversed to compile a standalone executable.

## Contributing

### Progress
The progress of reversed classes can be tracked [here](docs/ReversedClasses.MD). (needs to be updated)
We currently estimate that about 50-60% of the code is done.
Since this project is done as a hobby, and worked on at irregular intervals, there's no ***time estimate*** on when it'll be finished.

### Coding Guidelines 
Before you start writing code, please make sure to read the [coding guidelines](docs/CodingGuidelines.MD) for this project.

### What to work on?
Check out [this discussion](https://github.com/gta-reversed/gta-reversed-modern/discussions/402) for some inspiration ;)

## Build Instructions

### Set up the game and ASI
#### 1. Requirements/Prerequisites
* GTA SA 1.0 US
* GTA SA ***Compact exe***: Our code requires you to use this exe, otherwise you will experience random crashes. You must own the original game and assets for this to work.

#### 2 Setting up Game Files
0) Copy game files to `game` subdirectory
1) Replace `gta_sa.exe` with compact version
2) Install ASI Loader

1. To install all the necessary files (after building the project!), run `install.py` with **__administrator privileges__** [Necessary to create symlinks on Windows] in the root directory.
Alternatively, you can install them by yourself:
    * [ASI Loader](https://gtaforums.com/topic/523982-relopensrc-silents-asi-loader/)
    * [III.VC.SA.WindowedMode.asi](https://github.com/ThirteenAG/III.VC.SA.WindowedMode)
    * Mouse Fix (**dinput8.dll**) [Can be found in the zip in `./contrib`]

    You can download them in a single [archive](https://github.com/gta-reversed/gta-reversed-modern/blob/master/contrib/plugins.zip).

#### Other plugins
Using other (than the ones we've tested) plugins is strongly discouraged and we provide __**no support**__.

### Development Environment
#### 1. Requirements/Prerequisites
* **Latest [Visual Studio 2022](https://visualstudio.microsoft.com/en/downloads/)**
* [CMake](https://cmake.org/download/)
* Clone submodules `git submodule update --init --recursive`
* GTA SA ***Compact exe***: Our code requires you to use this exe, otherwise you will experience random crashes. You must own the original game and assets for this to work.

> [!IMPORTANT]
> GTA SA ***Compact exe*** is not the same as ***1.0 US exe***, so check that your executable is exactly `5'189'632 bytes` (4.94 MiB).

> [!IMPORTANT]
> Run `install` if game files are added after configuration.
> For manual configuration use the CMake option `GTASA_GAME_DIRECTORY`

#### Setting up Visual Studio
0) Run: `install.bat`
1) Open the `build/GTASA.sln` file

#### Manual Configuration
0) Setup with `cmake -S . -A Win32 -B build`
1) Build with `cmake --build build`

#### Debugging with Visual Studio
0) Make sure the game directory is configured correctly
1) Set `gta_reversed` as startup project and run

#### Debugging Manually
0) Make sure the latest DLL is in the `scripts` subdirectory of your GTASA installation - Skip this step if you've used `install.py` (As it uses symlinks!)
1) Launch the game
2) Attach using the [ReAttach plugin](https://marketplace.visualstudio.com/items?itemName=ErlandR.ReAttach) to make your life easier
    
# Credits
- All contributors of the [plugin-sdk](https://github.com/DK22Pac/plugin-sdk) project.
- All contributors of the [original project](https://github.com/codenulls/gta-reversed).
- Hundreds of great people on [gtaforums.com](https://gtaforums.com/topic/194199-documenting-gta-sa-memory-addresses).
- [re3 team](https://github.com/GTAmodding/re3).
- And everyone who contributed to GTA:SA reversing.
