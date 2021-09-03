# Периметр | Perimeter

![Perimeter](https://cdn.akamai.steamstatic.com/steam/apps/289440/header.jpg)

[![Linux Build](https://github.com/KranX/Perimeter/actions/workflows/linux_build.yml/badge.svg)](https://github.com/KranX/Perimeter/actions/workflows/linux_build.yml)
[![Windows MSVC amd64 Build](https://github.com/Kranx/Perimeter/actions/workflows/windows_msvc_x86_build.yml/badge.svg)](https://github.com/Kranx/Perimeter/actions/workflows/windows_msvc_x86_build.yml)
[![Windows MSVC amd64 Build](https://github.com/Kranx/Perimeter/actions/workflows/windows_msvc_amd64_build.yml/badge.svg)](https://github.com/Kranx/Perimeter/actions/workflows/windows_msvc_amd64_build.yml)
[![Windows MSYS amd64 Build](https://github.com/Kranx/Perimeter/actions/workflows/windows_msys_32_build.yml/badge.svg)](https://github.com/Kranx/Perimeter/actions/workflows/windows_msys_32_build.yml)
[![Windows MSYS amd64 Build](https://github.com/Kranx/Perimeter/actions/workflows/windows_msys_64_build.yml/badge.svg)](https://github.com/Kranx/Perimeter/actions/workflows/windows_msys_64_build.yml)

[![Join the chat at https://t.me/PerimeterGame](https://patrolavia.github.io/telegram-badge/chat.svg)](https://t.me/PerimeterGame)

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

## Enhancements from original game

- 64 bits support
- Frame selection in battle/multiplayer menu

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
  * HT - _Working_ - From "HyperTreading", contains some threading related stuff
  * Network - _Windows_ - Contains high abstraction networking code which is used from other parts of game and 
    also low level networking code used by higher abstraction networking code.
  * PlayOgg - _Windows_ - Handles .ogg files loading and playing using a dedicated thread.
  * PluginMAX - _Working_ - Handles game 3D models loading from M3D/L3D files. Name comes from "3ds Max" which was used
    originally for creating and exporting these models.
  * Render - _Working_ - Provides graphics/rendering abstraction API, uses DirectX APIs under the hood which is provided
    by dxvk-native and Exodus outside Windows platform.
  * Scripts/ - _Working_ - Contains .prm files that are compiled using XPrm tool, *Library files that are loaded by game at runtime
    which contain some game configs/data and Texts.tdb containing ingame texts. Some game copies may contain a copy of this folder.
    * Triggers - Trigger chains containing maps and AI triggers/behaviors.
  * Sound - _Windows_ - Provides sound effects abstraction API, uses DirectSound under the hood.
  * Terra - _Working_ - Seems to contain deformable terrain related code.
  * TriggerEditor - _Unknown_ - A GUI providing library which could be called from debug builds of game to edit Trigger chains.
  * tx3d - _Working_ - "Procedural 3D Texture Library", used by Terra and SCodeInterpreter
  * UserInterface/ - _Working_ - Handles game UI such as main menu and submenus, ingame HUD and inputs. 
    * SCodeInterpreter - _Working_ - Interpreter for chain history files seen during main menu and campaign.
  * Util - _Working_ - Utilities for game and other modules.
  * XPrm - _Working_ - Tool for compiling .prm into declaration/implementation files such as hi/cppi.
  * XUtil - _Working_ - Previously a separate library containing various X* named helpers for different projects,
    currently only the essential stuff for the game remain. Some removed stuff may be need to ported from original
    XUtil sources for the rest of tools.
  
* XLibs.Net/ - Contains some library headers and binaries
* MSDXSDK_02_06/ - DirectX SDK used by the game

**Unknown** - Not tested, not compilable with modern compilers nor available in more platforms than Windows.`

**Windows** - Compilable with modern compilers but not available in more platforms than Windows.

**Working** - Compilable with modern compilers and available in other platforms such as Windows and Linux.

Meta issue for tracking code modernization/porting progress: https://github.com/KranX/Perimeter/issues/58
