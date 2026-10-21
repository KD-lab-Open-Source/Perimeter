# Building a macOS app

The engine source does not include the game data. Start with a complete installation of Perimeter or Emperor's Testament from your own copy of the game, and keep the two installations in separate directories.

## Russian game data from Steam

Before copying the Windows installation to your Mac, open the game's Properties in Steam, select Russian in the language setting, and wait for Steam to finish downloading the language files. Then copy the complete game directory. Repeat this for Emperor's Testament if you use that game too.

Changing the language in Perimeter's main menu selects resources that are already installed; it cannot download missing speech or repair a partially copied installation. In our test, the missing Russian speech came from an incomplete Steam download. No text conversion was needed once the complete resources were present. Steam documents the per-game language setting [here](https://help.steampowered.com/en/faqs/view/4984-C127-121D-B3F2).

Text or speech baked into a movie will only change if the installation includes another version of that movie. A Russian-only video does not become English when the menu language changes.

## Build the engine

These commands target Apple Silicon and use Homebrew dependencies. Intel and universal builds need separate validation; selecting two architectures in CMake is not enough because DXVK uses a single-architecture Meson cross file.

Install Xcode Command Line Tools if needed, then install the build dependencies:

```sh
xcode-select --install
brew install cmake ninja meson pkgconf boost ffmpeg zlib lld sdl2 sdl2_image sdl2_mixer sdl2_net vulkan-headers vulkan-loader molten-vk
```

From the repository root, configure a fresh build directory:

```sh
cmake -S . -B build-macos -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_FIND_FRAMEWORK=LAST -DOPTION_D3D9=ON -DOPTION_FFMPEG_MOVIES=ON
cmake --build build-macos --parallel 8
```

The executable is `build-macos/Source/perimeter.app/Contents/MacOS/perimeter`. The build tree still uses libraries from the build machine; package it before treating it as a standalone application.

If your checkout also contains the experimental macOS shader patch, add `-DOPTION_MACOS_D3D9_SHADERS=ON` when configuring. That option patches the pinned DXVK source and enables the R32F shadow-map path. It is off by default and is not part of unmodified upstream main. Do not enable the renderer changes against an unrelated unpatched DXVK library.

The macOS FFmpeg finder patch uses pkg-config for headers and libraries together. If you use a custom FFmpeg installation, set `FFMPEG_ROOT` to its prefix or configure `PKG_CONFIG_PATH` for its `.pc` files. If CMake previously selected headers from GStreamer and libraries from Homebrew, start with a fresh build directory.

## Package the app

`macos/bundle.py` copies the built app, recursively bundles its linked non-system libraries, adds MoltenVK and its ICD manifest, rewrites library paths, creates the Finder launcher, and applies an ad-hoc signature. It leaves the source build untouched and refuses to overwrite an existing destination.

```sh
python3 macos/bundle.py --app build-macos/Source/perimeter.app --output dist/Perimeter.app --icd "$(brew --prefix molten-vk)/etc/vulkan/icd.d/MoltenVK_icd.json" --library-dir build-macos/Source/Render/dxvk-prefix/src/dxvk-build/src/d3d9
```

To include your game data in a personal copy of the app, add `--content "/absolute/path/to/Perimeter"` to that command. The data is copied into `Contents/Resources/Content` before signing. Use a separate output app for Emperor's Testament, with its own content directory. Game data is not included in this repository or in the patch.

For an app without embedded data, supply the directory on first launch:

```sh
open dist/Perimeter.app --args "content=/absolute/path/to/Perimeter"
```

The engine stores the selected content path in its preferences. An app with embedded data can be opened directly in Finder. The launcher uses absolute paths so spaces in the app's location do not change how libraries or content are resolved.

The minimum macOS version comes from the newest requirement among the executable and bundled libraries, which the packager records in Info.plist. A low CMake deployment target does not make newer Homebrew libraries work on older macOS releases.

Homebrew's full FFmpeg build can bring many dependent libraries into the bundle. A smaller release would need a separately configured FFmpeg build and a review of the bundled libraries' distribution requirements. Ad-hoc signing is for local use; it is not Developer ID signing or notarization.

## Check the result

The packager verifies the signature and checks for remaining absolute non-system library dependencies. Move the app to a different directory, open it from Finder, and test the intro, a mission briefing, speech, language switching, and the exit movie. Test switching to another application and back as a separate case.

Local validation has covered dependency bundling and loading the bundled DXVK and MoltenVK libraries. The engine and shader changes have also been exercised in the game, including Emperor's Testament, but a Finder launch of the newly packaged app and a run on a Mac without the build dependencies still need to be checked. The fullscreen recovery workaround is a separate unfinished patch and is not included here.
