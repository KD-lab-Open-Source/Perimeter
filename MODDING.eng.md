# Modding

Thanks to the availability of code we can modify any aspects of game and introduce some facilities for modding the game
in a easier and cleaner manner than previously was possible when dealing with game assets.

This file intends to document about some engine internals and changes done since open-source publication, 
any corrections or updates to reflect current state of capabilities are welcomed.

## Command line

Running the game in console/terminal and adding /? or --help or -h displays available parameters that can be added
when launching the game. This may be useful to test some options or use alternate modes like Mission Editor.

## Mods folder

Game now loads mods (each is a folder containing everything) that are located inside `mods` folder at game folder.

Mods can be enabled/disabled in `Main Menu > Community > Mods` by adding `.off` at end of folder name, can be done manually too.

Perimeter: Emperor Testament game folder can also be placed as a mod to include extra units in main game and missions,
this case is handled in special manner to allow loading certain files only when switching to ET campaign.

Order of mods loading is done alphabetically based on Mod name in `mod.ini` (log file can be checked for actual order).
This is important as a later loaded mod may replace same file that was replaced by previous mods.

The mod name acts as identifier so no mods with same name can exist, if this happens only one will be loaded.

## Mod information in mod.ini

Each mod needs a file in the folder named "mod.ini" with at least "name" and "version" filled in "Mod" section.
This allows players and the game to know some basic info about it.

If current locale description as "description_LOCALE" like "description_english" isn't present the generic "description" will be used.

The following is a example for a mod having one generic description and translated descriptions while also having
minimum version for game required to run:
```
[Mod]
name=My first mod
version=1.0.0
description=&00FF00My first mod!\n&FFFFFFThis is a example of how to write &00FF00colored multiline\nmod description.
description_english=&00FF00My first mod!\n&FFFFFFThis is a example of how to write &00FF00colored multiline\nmod description.
description_russian=&00FF00Мой первый мод!\n&FFFFFFЭто пример написания цветного многострочного\nописания мода.
description_spanish=&00FF00¡Mi primer mod!\n&FFFFFFEste es un ejemplo de cómo escribir una descripción de mod\nmultilínea coloreada.
authors=First author name\nAnother author name\n3rd Author name
url=https://kdlab.com

[Content]
game_minimum_version=3.0.11
```

Content section optional fields:
```
- game_minimum_version
Minimum game version required
Example: 3.0.11

- required_content
Required game content to be present and active, may contain several enums
Example: PERIMETER|PERIMETER_ET

- disallowed_content
Disable mod if active/campaign game content matches exactly, may contain several enums
Example: PERIMETER_ET
```

## Overlay system

Files places inside mod folder overlays (places/replaces without changing real files structure) the previous
game files, allowing to update old assets with tweaked versions, adding new files like maps, translations,
custom textures, models, resolutions, effects, scripts...

Examples:

- File placed in `Mods/MyLogoMod/Resource/Icons/logo.tga` will be used when `Resource/Icons/logo.tga` file is read.
- Files placed in `Mods/MyMap/Resource/Multiplayer/MyCoolMap.spg` etc, will appear like they were placed in 
  `Resource/Multiplayer/MyCoolMap.spg` thus allowing players to use this multiplayer map.

Localizations also can supply certain resources adecuate to the provided language when placed as these examples:
- LocData/LocData/LANGUAGE/Video/Perimeter.bik is used instead of Resource/Video/Perimeter.bik if LANGUAGE is active
- LocData/LocData/LANGUAGE/MainMenu/1200/main_menu_logo.tga is used instead of Resource/Icons/MainMenu/1200/main_menu_logo.tga if LANGUAGE is active

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

## Extending AttributeLibrary

While one can place `Scripts/AttributeLibrary` in mod to replace the existing attribute library, is better to extend it
by creating `Scripts/AttributeLibraryExtra` which will be merged into already loaded attribute library replacing
any existing data. `Scripts/AttributeLibraryExtraCampaign` can also be used to set attributes only when in a campaign
mission. This also applies to RigidBodyPrmLibrary.

Is important to remember that mod loading order still apply here as ones loading last will set the final data.

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

## Locale based scenario.hst

Game will use `Resource/scenario_LOCALE.hst` such as `Resource/scenario_russian.hst` when language is russian and this
file exists instead of the generic `Resource/scenario.hst`, this is useful for russian language as the european and
russian campaigns are different.

## Custom texts and translations

Mods can supply new language/translation texts or replace existing texts with own version by creating a .txt file at
`Resource/LocData/LANGUAGE/Text/MyTexts.txt` such as `Resource/LocData/English/Text/Texts.txt` and providing each entry
as a line like this:
```
Game.Text.ID.To.Use=The visible text to show when game requests Text ID
Other.Text.ID=Another text entry
```

You can add a new line by adding `\n` like `TextID=Line 1\nLine 2\nLine 3`

If you need to show the ` \ ` character itself you can write `\\`

Game allows coloring text by using &RRGGBB format before the text to color, some examples:
`Previous color text&FF0000Red Text&0000FFBlue Text&FFFFFFWhite Text`

The .txt files must be encoded as UTF-8 for all languages, the game handles any required conversions internally.

Original custom `.btdb` format is also accepted but `.txt` may be lot easier to handle by modders.

If you don't wish to replace existing texts if game already has loaded them you can add `_noreplace` to the file name
such as `MyTexts_noreplace.txt` or `MyTexts_noreplace.btdb`

Is important to remember that mod loading order still apply here as ones loading last will set the final data.

## Stack Traces

Perimeter Release builds contain basic function names but it seems that in some OSes (Win10/11) the stacktrace
isn't properly printed, still is possible to reconstruct the stacktrace with proper symbol names by using the
a specific part of logfile.txt with same binary that stacktrace was made with. We can know what binary was by ensuring
that the version+date matches the one we are using like for example: `Version 3.0.10 Final (Apr 10 2022 00:39:40)`

Then we pick this line from logfile.txt:
`stack_reference=0x00007FF6176E4100 stack_frames=0x00007FF6176E4AFB,0x00007FF6176E5183,0x00007FF6176E545C,.......`

and call the binary with it like:
`perimeter stack_reference=0x00007FF6176E4100 stack_frames=0x00007FF6176E4AFB,0x0000.......`

which should print us a stacktrace with function names if your OS works with them, on Linux+WINE it seems to be possible
to reconstruct stacktraces with function names.
