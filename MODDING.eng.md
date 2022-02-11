# Modding

Thanks to the availability of code we can modify any aspects of game and introduce some facilities for modding the game
in a easier and cleaner manner than previously was possible when dealing with game assets.

This file intends to document about some engine internals and changes done recently, any corrections or 
updates to reflect current state of capabilities are welcomed.

## Mods folder

Game now loads mods (each is a folder containing everything) that are located inside "mods" folder.

Mods can be enabled/disabled in main menu by adding ".off" at end of folder name, can be done manually too.

Perimeter Emperor Testament game folder can also be placed as a mod to include extra units in main game and missions,
this case is handled in special manner to allow loading certain files only when switching to ET campaign.

Order of mods loading is done alphabetically (log file can be checked for actual order). This is important as a later
loaded mod may replace same file that was replaced by previous mods.

## Overlay system

Files places inside mod folder overlays (places/replaces without changing real files structure) the previous
game files, allowing to update old assets with tweaked versions, adding new files like maps, translations,
custom textures, models, resolutions, effects, scripts...

Examples:

- File placed in `Mods/MyLogoMod/Resource/Icons/logo.tga` will be used when `Resource/Icons/logo.tga` file is read.
- Files placed in `Mods/MyMap/Resource/Multiplayer/MyCoolMap.spg` etc, will appear like they were placed in 
  `Resource/Multiplayer/MyCoolMap.spg` thus allowing players to use this multiplayer map.

## Content mapping

Text file named `content_mapping.txt` can be placed in mod folder to tell game to map certain paths to another path
to avoid distributing duplicated or copyrighted assets, one example would be mapping russian briefings and tips into
english language by setting following in `content_mapping.txt` file:
```
Resource/LocData/Russian/Voice/Tips=Resource/LocData/English/Voice/Tips
Resource/LocData/Russian/Voice/Briefings=Resource/LocData/English/Voice/Briefings
```

Another use is to distribute one copy of map at `Resource/Multiplayer/CoolMap` and make it available both in Battle and
Multiplayer:
```
Resource/Multiplayer/CoolMap.spg=Resource/Battle/CoolMap.spg
Resource/Multiplayer/CoolMap.bin=Resource/Battle/CoolMap.bin
```

## Custom normal maps

Currently game uses grayscale TEXTURENAME_bump.tga files to generate DX9 compatible normal map textures, this is usually
enough but game allows loading DX9 textures directly when TEXTURENAME_normal.tga file exists alongside TEXTURENAME.tga.

## Custom resolutions

Currently game uses the most approximate resolution assets (source resolution/UI) and scales it to screen/window
resolution maintaining the proper aspect ratio of source resolution inside screen/window by doing a vertical fit and
multiplying ratio to scaled width.

To add assets for custom source resolution other than the 4 resolutions available in released games
(for example 2048x1536), the following folders need to be created inside mod with corresponding files inside:
```
Resource/intf/2048x1536
Resource/MainMenu/x1536
Resource/Portraits/x1536
```

Only the intf contains width of resolution, this is because Main Menu and Portraits are always 4:3 aspect ratio, this
allows using different width intf resolutions with same set of Main Menu/Portraits assets.

Some adjustments may be need on game code, configs or textures itself to support non 4:3 resolutions in intf assets.

## XPrm and .prm files

XPrm is a compiler for .prm files that serve as custom engine file format, during development the .prm can be converted
to .hppi/.cppi generated files and included from source code to use inside binary. This may be a inconvenience when
.prm files need to be adjusted, the game has a runtime XPrm compiler to allow hot loading .prm file changes in Scripts
and apply values while game is running.

However, this runtime compiler has limitations such as not able to load changes to array or object structures like
changing size, adding or removing fields so those changes need to be applied by calling ScriptsCompile step or
OPTION_PROCESS_SCRIPTS in cmake and then recompiling the game executable.
