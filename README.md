## gtasa-reversed [![Build Status](https://github.com/gta-reversed/gta-reversed/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/gta-reversed/gta-reversed/actions?query=workflow%3ABuild)

A project to reverse Grand Theft Auto San Andreas completely, and to rewrite and document every function.

### Community
Please join our community Discord: [GTA Groupies](https://discord.gg/FG8XJ5Npqe) [The invite is permanent, feel free to share it!]

## About the project
Building this project will result in a DLL file that can be injected into GTA:SA using any ASI loader out there. After the DLL file has been injected, the ingame functions will be replaced by the reversed ones. The game will behave the same. Now if we make any changes to the reversed code, it will take effect in the game. The goal is to keep reversing until we have the entire game reversed to compile a standalone executable.

## Progress
The progress of reversed classes can be tracked [here](docs/ReversedClasses.MD). (needs to be updated)
We currently estimate that about 50-60% of the code is done.
Since this project is done as a hobby, and worked on at irregular intervals, there's no ***time estimate*** on when it'll be finished.

## Build Instructions
#### 0. Requirements/Prerequisites
* **Latest [Visual Studio 2022](https://visualstudio.microsoft.com/en/downloads/)**
* [Python](https://www.python.org/downloads/) >= 3.x (For Conan)
* [Conan](https://docs.conan.io/2/installation.html#install-with-pip-recommended) (>= 2.x)
* [CMake](https://cmake.org/download/)

#### 1. Install Python, and Conan
* Install the [latest Python](https://www.python.org/downloads/) [When installing make sure to tick the "Add to PATH" checkbox!]
* Install Conan using `pip install conan` [You might need to reload the console if you've freshly installed Python.]

#### 2. Setting up VS
0) Run: `install` depending on the environment with the `Debug` argument (Or any of `Debug`, `Release`, `RelWithDebInfo`) (e.g. `install.bat Debug`)
1) Open the folder with VS
2) Select `conan-debug (conan-default-conan-debug)` or `conan-release (conan-default-conan-release)` (Depending on what you've spcified in the command above), other builds will result in CMake errors [TODO: Fix this somehow... We aren't proeficient with Conan/CMake sadly]
3) Alternatively you can type `cmake --preset conan-debug/release` and build with `cmake --build build` in CLI.

### 3. Set up the game and ASI
0. GTA SA ***Compact exe***: Our code requires you to use this exe, otherwise you will experience random crashes. You must own the original game and assets for this to work.

1. To install all the necessary files (after building the project!), run `install.py` with **__administrator privileges__** [Necessary to create symlinks on Windows] in the root directory.
Alternatively, you can install them by yourself:
    * [ASI Loader](https://gtaforums.com/topic/523982-relopensrc-silents-asi-loader/)
    * [III.VC.SA.WindowedMode.asi](https://github.com/ThirteenAG/III.VC.SA.WindowedMode)
    * Mouse Fix (**dinput8.dll**) [Can be found in the zip in `./contrib`]

    You can download them in a single [archive](https://github.com/gta-reversed/gta-reversed-modern/blob/master/contrib/plugins.zip).

### Debugging
0) Make sure the latest DLL is in the `scripts` folder of your GTASA installation - Skip this step if you've used `install.py` (As it uses symlinks!)
1) Launch the game
2) Attch using the [ReAttach plugin](https://marketplace.visualstudio.com/items?itemName=ErlandR.ReAttach) to make your life easier

### Other plugins
Using other (than the ones we've tested) plugins is strongly discouraged and we provide __**no support**__.

## Contributing

### Coding Guidelines 
Before you start writing code, please make sure to read the [coding guidelines](docs/CodingGuidelines.MD) for this project.

### What to work on?
Check out [this discussion](https://github.com/gta-reversed/gta-reversed-modern/discussions/402) for some inspiration ;)
    
# Credits
- All contributors of the [plugin-sdk](https://github.com/DK22Pac/plugin-sdk) project.
- All contributors of the [original project](https://github.com/codenulls/gta-reversed).
- Hundreds of great people on [gtaforums.com](https://gtaforums.com/topic/194199-documenting-gta-sa-memory-addresses).
- [re3 team](https://github.com/GTAmodding/re3).
- And everyone who contributed to GTA:SA reversing.
