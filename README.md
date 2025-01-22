## gtasa-reversed [![Build Status](https://github.com/gta-reversed/gta-reversed-modern/workflows/Build/badge.svg?event=push&branch=master)](https://github.com/gta-reversed/gta-reversed-modern/actions?query=workflow%3ABuild)

A project to reverse Grand Theft Auto San Andreas completely, and to rewrite and document every function.

### Community
Please join our community Discord: [GTA Groupies](https://discord.gg/FG8XJ5Npqe) [The invite is permanent, feel free to share it!]

### What exactly is this project? 
Building this project will result in a DLL file that can be injected into GTA:SA using any ASI loader out there. After the DLL file has been injected, the ingame functions will be replaced by the reversed ones. The game will behave the same. Now if we make any changes to the reversed code, it will take effect in the game. The goal is to keep reversing until we have the entire game reversed to compile a standalone executable.

### Progress
The progress of reversed classes can be tracked [here](docs/ReversedClasses.MD). (needs to be updated)
We currently estimate that about 50-60% of the code is done.
Since this project is done as a hobby, and worked on at irregular intervals, there's no ***time estimate*** on when it'll be finished.

### Coding Guidelines 
Before you start writing code, please make sure to read the [coding guidelines](docs/CodingGuidelines.MD) for this project.


### Build Instructions
#### 0. Requirements/Prerequisites
* **Latest [Visual Studio 2022](https://visualstudio.microsoft.com/en/downloads/)** (Community Edition is enough)
* **Premake5** [Included, but any official version would work]

#### 1. Initialize/Update submodules
First clone the project, including the submodules:
```shell
git clone --recurse-submodules https://github.com/gta-reversed/gta-reversed-modern.git
```
Or if already cloned:
```shell
git submodule update --init --recursive
```
#### 2. Generate solution using Premake5
1) Execute `premake5.bat`
2) You'll find gta_reversed.sln shortcut in the same folder as premake5.
3) Open it, and once the project has loaded, just hit `CTRL + SHIFT + B`

#### Generate solution using CMake
The CMake build is currently not supported, and is used only for the CI.
The DLLs it produces **do not work**.

### Game
1. GTA SA ***Compact exe***: Our code requires you to use this exe, otherwise you will experience random crashes.

2. To install all the necessary files (after building the project!), run `install.py` with **__administrator privileges__** [Necessary to create symlinks on Windows] in the root directory.
Alternatively, you can install them by yourself:
    * [ASI Loader](https://gtaforums.com/topic/523982-relopensrc-silents-asi-loader/)
    * [III.VC.SA.WindowedMode.asi](https://github.com/ThirteenAG/III.VC.SA.WindowedMode)
    * Mouse Fix (**dinput8.dll**) [Can be found in the zip in `./contrib`]

    You can download them in a single [archive](https://github.com/gta-reversed/gta-reversed-modern/blob/master/contrib/plugins.zip).

### Other plugins
Using other (than the ones we've tested) plugins is strongly discouraged and we provide __**no support**__.

### Debugging
To run the game with the Modern plugin, you just need to run the "Local Windows Debugger" in Visual Studio.

In case that doesn't suit your needs, use the [ReAttach plugin](https://marketplace.visualstudio.com/items?itemName=ErlandR.ReAttach) to make your life easier.

### What to work on?
Check out [this discussion](https://github.com/gta-reversed/gta-reversed-modern/discussions/402) for some inspiration ;)
    
### Credits
- All contributors of the [plugin-sdk](https://github.com/DK22Pac/plugin-sdk) project.
- All contributors of the [original project](https://github.com/codenulls/gta-reversed).
- Hundreds of great people on [gtaforums.com](https://gtaforums.com/topic/194199-documenting-gta-sa-memory-addresses).
- [re3 team](https://github.com/GTAmodding/re3).
- And everyone who contributed to GTA:SA reversing.
