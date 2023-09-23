{
  src,
  pkgs,
  system,
  isStatic ? false,
  flag_debug ? false,
  flag_sokol,
  flag_d3d9
}: let 
  lib = pkgs.lib;
  platform = pkgs.stdenv.hostPlatform;
  flag_static = isStatic || platform.isStatic;
  flag_dxvk = !platform.isWindows && flag_d3d9;
  link_libraries = []
  ++ lib.optionals flag_dxvk [ "vulkan" ]
  ++ lib.optionals (platform.isLinux && flag_sokol) [ "GL" ]
  ++ lib.optionals platform.isLinux [ "asound" ]
  ;
  
  
  # Dependencies that CMake fetches with FetchContent
  dxvk_git = builtins.fetchGit (if pkgs.stdenv.isDarwin then {
    url = "https://github.com/IonAgorria/dxvk-native.git";
    rev = "43aedc756cbd620b9ee8b1cf2c17b17cc49b3781";
    ref = "master";
  } else {
    url = "https://github.com/doitsujin/dxvk.git";
    rev = "80e075406b1b7d9d2508c9dc05e9745b3d3cf7e2";
    ref = "master";
    submodules = true; 
  });
  sokol_git = builtins.fetchGit {
    url = https://github.com/floooh/sokol;
    rev = "c0e54485457b2e1645f2d809394753b53bf50cd4";
    ref = "master";
  };
  gamemath_git = builtins.fetchGit {
    url = "https://github.com/TheAssemblyArmada/GameMath";
    rev = "59f7ccd494f7e7c916a784ac26ef266f9f09d78d";
    ref = "master";
  };
  simpleini_git = builtins.fetchGit {
    url = "https://github.com/brofield/simpleini";
    rev = "7350fcc9228f410309734c3fb6dae2bf513cdd98";
    ref = "master";
  };
  pevents_git = builtins.fetchGit {
    url = "https://github.com/neosmart/pevents";
    rev = "d6afcbc629cf806f6465238849278e530e1d56fb";
    ref = "master";
  };
in pkgs.stdenv.mkDerivation {
  pname = "perimeter";
  version = "3.0.11";
  meta = with lib; {
    homepage = "https://github.com/KD-lab-Open-Source/Perimeter/";
    description = "Perimeter - A open-source RTS game from 2004 by K-D LAB";
    license = licenses.gpl3;
    maintainers = with maintainers; [];
    platforms = [ system ];
  };
  src = src;
  
  # Build only dependencies
  nativeBuildInputs = with pkgs.pkgsBuildHost; [
    binutils #For strip
    gitMinimal
    ninja
    pkg-config
    cmake
  ]
  ++ lib.optionals flag_dxvk [ meson glslang ]
  ++ lib.optionals flag_sokol [ sokol ];
  
  # Build / Runtime dependencies
  buildInputs = with (if flag_static then pkgs.pkgsStatic else pkgs); [
    zlib
    boost
    ffmpeg
    SDL2
    SDL2_mixer
    SDL2_image
    SDL2_net
  ]
  ++ lib.optionals (flag_sokol && !platform.isWindows) [ libglvnd ]
  ++ lib.optionals (platform.isLinux) [ alsa-lib ]
  ++ lib.optionals (flag_dxvk) [ vulkan-loader ]
  ;
  
  configurePhase = (lib.optionalString flag_dxvk ''
    cp -a ${dxvk_git} dxvk_src
    if [[ -f "dxvk_src/subprojects/libdisplay-info/tool/gen-search-table.py" ]]; then
      substituteInPlace "dxvk_src/subprojects/libdisplay-info/tool/gen-search-table.py" \
        --replace "/usr/bin/env python3" "${lib.getBin pkgs.pkgsBuildHost.python3}/bin/python3"
    fi
  '') + ''
    mkdir build
    cmake -S . -B build -G Ninja \
      -DOPTION_STATIC_LIB_STD=ON \
      -DOPTION_STATIC_BUILD=${if flag_static then "ON" else "OFF"} \
      -DOPTION_D3D9=${if flag_d3d9 then "ON" else "OFF"} \
      -DOPTION_SOKOL=${if flag_sokol then "ON" else "OFF"} \
      -DOPTION_LINK_LIBS="${builtins.concatStringsSep ";" link_libraries}" \
      -DFETCHCONTENT_FULLY_DISCONNECTED=True \
      -DFETCHCONTENT_SOURCE_DIR_SIMPLEINI=${simpleini_git} \
      -DFETCHCONTENT_SOURCE_DIR_GAMEMATH=${gamemath_git} \
      -DFETCHCONTENT_SOURCE_DIR_PEVENTS=${pevents_git} \
  '' + (lib.optionalString flag_sokol ''
      -DFETCHCONTENT_SOURCE_DIR_SOKOL=${sokol_git} \
  '') + (lib.optionalString flag_dxvk ''
      -DOPTION_DXVK_SOURCE_DIR=$PWD/dxvk_src \
      -DCMAKE_SKIP_BUILD_RPATH=ON \
  '') + (lib.optionalString (platform.isWindows) ''
      -DMINGW=${if platform.isMinGW then "ON" else "OFF"} \
      -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
  '') + ''
      -DCMAKE_BUILD_TYPE=${if flag_debug then "Debug" else "Release"}
  '';
  
  buildPhase = ''
    cd build
    ninja dependencies
    ninja
  '';
  
  installPhase = (lib.optionalString flag_dxvk ''
    mkdir -p $out/lib
    cp Source/dxvk-prefix/src/dxvk-build/src/d3d9/libdxvk_d3d9.so $out/lib;
  '') + ''
    strip -g -x Source/perimeter
    mkdir -p $out/bin
    install -m755 -D Source/perimeter $out/bin/perimeter
  '';
  
  # fixup phase
  dontStrip = true;
}