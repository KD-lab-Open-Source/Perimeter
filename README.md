# Периметр | Perimeter

![Perimeter](https://cdn.akamai.steamstatic.com/steam/apps/289440/header.jpg)

[![Linux Build](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/linux_build.yml/badge.svg)](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/linux_build.yml)
[![MacOS Build](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/macos_build.yml/badge.svg)](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/macos_build.yml)
[![Windows MSVC Build](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/windows_msvc_build.yml/badge.svg)](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/windows_msvc_build.yml)
[![Windows MSYS Build](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/windows_msys_build.yml/badge.svg)](https://github.com/KD-lab-Open-Source/Perimeter/actions/workflows/windows_msys_build.yml)

[![Join the chat at https://t.me/PerimeterGame](https://patrolavia.github.io/telegram-badge/chat.svg)](https://t.me/PerimeterGame)

Discord: https://discord.com/invite/jg9G7cp

## About

Perimeter is a real-time strategy video game with unique gameplay elements such as terraforming deformable terrain,
morphing units, energy network, protective shield and surreal worlds.

Game needs assets/resources (3D models, textures, sounds, texts, configs, etc.) to run which can be obtained from
purchased games in physical copies or digital stores.

Some parts may still need fixing or be experimental, expect some unexpected behavior, artifacts, bugs or crashes.

Issues for feature requests, suggestions, propositions, bugs, pull requests and other contributions are welcomed.

## Лицензией | License

© ООО "КД ВИЖЕН" (Калининград) | © LLC "KD VISION" (Kaliningrad)

Весь код, за исключением сторонних библиотек, публикуется под лицензией GPLv3.
Код сторонних библиотек (где указана иная лицензия) публикуется под лицензией этих библиотек.

The whole code, except 3rd-party libraries, is published under the GPLv3 license.
Code of 3rd-party libraries (where another license is specified) is published under the license of these libraries.)

## Compiling and installing

There is instructions available in:

[English](INSTALL.eng.md)

## Supported games

- Perimeter
- Perimeter: Emperor Testament

## Enhancements from original game

- Modding support
  - [English](MODDING.eng.md)
- Bugfixes
- Right click on morph units to request basic units required
- Automatic game assets searching in these paths order:
  - Path from content= command argument
  - Linux: $HOME/.local/share/KD Vision/Perimeter/Content
  - MacOS: $HOME/Library/Application Support/KD Vision/Perimeter/Content
  - Executable path
  - Windows: %AppData%\\Roaming\\KD Vision\\Perimeter\\Content
  - Previous used content stored in settings
- Support for 64 bits and other CPUs architectures than x86
- Frame selection in battle/multiplayer menu and new colors
- Experimental widescreen and custom resolutions support:
  - Game scans Resource/Icons/intf for XxY named folders such as 1920x1080 and adds as selectable option
  - Custom resolutions uses the height to load Resource/Icons/MainMenu/ and Resource/Icons/Portraits/ 
    with folder name xY such as x1080 with above example. This allows reusing same height menu elements
    that still use 4:3 aspect ratio
  - Both .ini and resx= resy= command arguments accept custom resolutions, these will use
    the closest UI resolutions upscaled/centered to maintain visual consistency.
- Experimental resizable windows
- Better save game map / content checking
- Enhancements for multiplayer:
  - TCP/IP protocol instead of DirectPlay8
  - Cross platform multiplayer
  - Host Scripts transfer and basic l3d files checksum to ensure matching gameplay between players
  - Allow players to watch game after being defeated
  - Support for saving and continuing multiplayer saves
  - Experimental recovery mechanism for desyncs
- Experimental support for MacOS
- More than 4 player maps supported

## Game settings:

Application preferences paths: 
- Linux: $HOME/.local/share/KD Vision/Perimeter/
- MacOS: $HOME/Library/Application Support/KD Vision/Perimeter/
- Windows: %AppData%\\Roaming\\KD Vision\\Perimeter\

NOTE: Some settings like current language, active profile, multiplayer settings and game content path can be found now at file Settings.ini inside application preferences directory.
This file is automatically created/updated by the game.

## Repository contents

* Source/ - Contains source code for the game and tools, most folders purpose can be guessed but some noteworthy:
  * Configurator - _Unknown_ - GUI utility distributed with game that allows players to change language and other settings.
  * EasyMap - _Unknown_ - Some kind of map demo.
  * EFFECTTOOL - _Unknown_ - Seems to be a tool for editing effects
  * Exodus - _Working_ - Contains helper code and libraries for porting game outside Windows/DirectX
    (unrelated to game Exodus faction)
  * Game/ - _Working_ - Contains entry point for game in Runtime.cpp and other game related code.
    * Scripts - _Working_ - Destination folder for generated hi/cppi files from .prm script files, these are
      compiled using XPrm
  * HT - _Working_ - From "HyperTreading", contains some multithreading related stuff.
  * Network - _Working_ - Contains high abstraction networking code which is used from other parts of game and 
    low level networking code used by higher abstraction networking code. TCP sockets are provided by SDL_net.
  * PluginMAX - _Working_ - Handles game 3D models loading from M3D/L3D files. Name comes from "3ds Max" which was used
    originally for creating and exporting these models.
  * Render - _Working_ - Provides graphics/rendering abstraction API, uses DirectX APIs under the hood which is provided
    by dxvk and Exodus outside Windows platform.
  * Scripts/ - _Working_ - Contains .prm files that are compiled using XPrm tool, *Library files that are loaded by game at runtime
    which contain some game configs/data and Texts.tdb containing ingame texts. Some game copies may contain a copy of this folder.
    * Triggers - Trigger chains containing maps and AI triggers/behaviors.
  * Sound - _Working_ - Provides sound effects abstraction and music control API, uses SDL_mixer.
  * Terra - _Working_ - Seems to contain deformable terrain related code.
  * TriggerEditor - _Unknown_ - A GUI providing library which could be called from debug builds of game to edit Trigger chains.
  * tx3d - _Working_ - "Procedural 3D Texture Library", used by Terra and SCodeInterpreter
  * UserInterface/ - _Working_ - Handles game UI such as main menu and submenus, ingame HUD and inputs. 
    * SCodeInterpreter - _Working_ - Interpreter for chain history files seen during main menu and campaign.
  * Util - _Working_ - Utilities for game and other modules.
  * XPrm - _Working_ - Tool for compiling .prm into declaration/implementation files such as hi/cppi.
  * XTool - _Working_ - Previously a separate library containing various X* named helpers for different projects,
    currently only the essential stuff for the game remain. Some removed stuff may be need to ported from original
    XTool/XUtil sources for the rest of tools.
  
* XLibs.Net/ - Contains some library headers and binaries
* MSDXSDK_02_06/ - DirectX SDK used by the game

**Unknown** - Not tested, not compilable with modern compilers nor available in more platforms than Windows.`

**Windows** - Compilable with modern compilers but not available in more platforms than Windows.

**Working** - Compilable and available in other platforms such as Windows, MacOS and Linux.

Meta issue for tracking code modernization/porting progress: https://github.com/KD-lab-Open-Source/Perimeter/issues/58
