# Perimeter

Chat for the discussions: https://t.me/PerimeterGame

(с) ООО "KD VISION" (Kaliningrad)

The whole code, except 3rd-party libraries, is published under the GPLv3 license. Code of 3rd-party libraries (where another license is specified) is published under the license of these libraries.

## Repository contents

* Source/ - tested branch, which works with resources from Steam
* XLibs.Net/ - header-only libraries used by the game, actually used:
  * boost/
  * stl/ - in-house wrapper around standard library
  * VC7.1 - SDK from Microsoft (only libraries are needed)
  * VC8 - SDK from Microsoft (only libraries are needed)
  * XPrm.exe - generation utility, only required if you want to compile Scripts
* MSDXSDK_02_06 - DirectX SDK used by the game

## What is required

Tested that game builds in Windows XP + Visual Studio 2005 environment. Besides that, it's required
to install Windows SDK 7.1 for video playback could be downloaded from the link:

https://developer.microsoft.com/ru-ru/windows/downloads/sdk-archive/

There is a Windows XP version at the very bottom.

## Compilation

**Everything below is already done for the Source directory (probably, it might be needed to correct paths and install required SDKs). In Visual Studio 2005 it's enough to just select the Game project and Rebuild.**

Game compiles without serious problems, the main thing is to correctly set up **Additional Include Directories** and **Additional Link Directories**,
read below about that. The main problem was with code snippets written in style:

```C++
for (int i = ...) {
  //...
}

//afterward goes the usage of i
```

But modern compilers don't allow that, it could be simply fixed, - it's enough to move variable definition outside of the loop.

## Step-by-step build instruction

Open `VS2005` solution  `Source/PerimetrAddon.sln`.


**IMPORTANT** If you performed clean, then you have to restore files triggereditor.* from the repository, because it doesn't compile!

Sequence to build projects:
* Scripts
* PluginMax
* Terra
* Tx3D Library
* ~~TriggerEditor~~ - **there are errors, might be skipped**
* Peer
* Render
* PerimeterSound
* Game

Choose the sub-project within the Visual Studio menu and perform `Project Only -> Build Only <Project Name>`, just `Build` won't work as there are errors in `TriggerEditor`,
the game itself is the `Game` project and its output is `Game.exe`.

![image](https://user-images.githubusercontent.com/1727152/114376550-e9dba480-9baf-11eb-822e-49e47d9853fe.png)

**For Source dependencies are set and built into Win7. Perhaps, corrections will be needed.**

For **each project** it's needed to add the following search paths for `*.h*` files (Project Properties | C++ | General | Additional Include Directories):
* **XLibs.Net\boost**
* **XLibs.Net\stl**
* **XLibs.Net**

**IMPORTANT** Place them to the very end of the list.
![image](https://user-images.githubusercontent.com/1727152/114377393-d0872800-9bb0-11eb-8991-1668535b9ded.png)

After that, the selected project should be built.

For project **Render** besides `XLibs.Net` it's needed to be added:
* **Microsoft SDKs\Windows\v7.1\Include**
* **MSDXSDK_02_06\Include - Must be specified after v7.1**

For projects **PerimeterSound, **Game**, besides `XLibs.Net`, it's needed to be added:
* **Microsoft SDKs\Windows\v7.1\Include**

All projects are built, except **Game** - it doesn't link, but compiles. In order to fix that, it's needed to add the following search paths for `*.lib` files (Project Properties | C++ | Linker | Additional Library Directories):
* **Microsoft SDKs\Windows\v7.1\Include**
* **MSDXSDK_02_06\Lib\x86**
* **XLibs.Net\VC7.1**
* **XLibs.Net\VC8**

## Launching the game
Copy files GameDBG.exe, PrmEdit.dll, ScriptEditor.dll, TriggerEditor.dll, and Scripts\GameShellSq.prm to the game directory from Steam.
