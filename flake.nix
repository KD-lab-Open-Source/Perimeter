{
  inputs = {
    nixpkgs = {
      url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    };
    #nix-appimage = {
    #  url = "github:ralismark/nix-appimage";
    #};
    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, ... }:
    let
      # Systems supported to build this package
      buildSystems = [
          "i686-linux"
          "x86_64-linux"
          "x86_64-darwin"
          "aarch64-linux"
          "aarch64-darwin"
          "powerpc64le-linux"
          #"riscv64-linux"
      ];
      
      #Systems that don't need overlay
      stableSystems = [
          "x86_64-linux"
      ];

      # Systems targetted for this package
      crossSystems = (map (system: { name = system; inherit system; } ) buildSystems) ++ [
        { 
          name = "x86_64-linux-musl";
          system = {
            config = "x86_64-unknown-linux-musl";
          };
        }
        {
          name = "aarch64-linux-musl";
          system = {
            config = "aarch64-unknown-linux-musl";
          };
        }
        { 
          name = "x86_64-mingwW64";
          system = {
            config = "x86_64-w64-mingw32";
            libc = "msvcrt";
            #isStatic = true;
          };
        }
      ];

      # Helper to provide system-specific attributes
      forSystems = systems: f: nixpkgs.lib.genAttrs systems (system: f { inherit system; });
      
      # Handle feature variants
      mkPerimeterPackages = {
        prefix ? "",
        isStatic ? false,
        pkgs,
      }: let
        prefix_sep = if prefix == "" then "" else prefix + "_";
      in [
        {
          name = if prefix == "" then "default" else prefix;
          value = pkgs.callPackage ./perimeter.nix {
            inherit isStatic;
            src = self;
            flag_d3d9 = true;
            flag_sokol = true;
          };
        }
        {
          name = "${prefix_sep}d3d9";
          value = pkgs.callPackage ./perimeter.nix {
            inherit isStatic;
            src = self;
            flag_d3d9 = true;
            flag_sokol = false;
          };
        }
        {
          name = "${prefix_sep}sokol";
          value = pkgs.callPackage ./perimeter.nix {
            inherit isStatic;
            src = self;
            flag_d3d9 = false;
            flag_sokol = true;
          };
        }
        {
          name = "${prefix_sep}debug";
          value = pkgs.callPackage ./perimeter.nix {
            inherit isStatic;
            src = self;
            flag_debug = true;
            flag_d3d9 = true;
            flag_sokol = true;
          };
        }
      ];
      
      #Overlays to apply
      overlays = [
        #Allow all platforms for these pkgs
        (self: super: (
          nixpkgs.lib.genAttrs [ "SDL2_mixer" "SDL2_image" "SDL2_net" ] (name: 
            super.${name}.overrideAttrs (_: rec {
              meta.platforms = super.lib.platforms.all;  
            })
          )
        ))
        
        #Overlay to trim dependencies pulling too much subdependencies
        (self: super: {
          #Stop pulling whole linux for a game
          systemdMinimal = null;
          systemd = null;
          libusb1 = null;
          gst_all_1 = null;
          modemmanager = null;
          pipewire = null;
          e2fsprogs = null;
          flac = null;
          libavif = null;
          
          #Populate osxMinVersion in stdenv for libvpx
          libvpx = super.libvpx.override {
            stdenv = super.stdenv // (if (!super.stdenv.hostPlatform.isDarwin || (builtins.hasAttr "osxMinVersion" super.stdenv.hostPlatform)) then {} else {
              hostPlatform = super.stdenv.hostPlatform // {
                osxMinVersion = super.stdenv.hostPlatform.darwinMinVersion;
              };
            });
          };
          
          dbus = super.dbus.override {
            enableSystemd = false;
          };
          
          pulseaudio = super.pulseaudio.override {
            libOnly = true;
            udevSupport = false;
            bluetoothSupport = false;
            useSystemd = false;
          };
          
          #FFMPEG, only pick what actually needs
          ffmpeg = (super.ffmpeg.override {
            ffmpegVariant = "headless";
            
            withHeadlessDeps = false;
            withOgg = true;
            withVorbis = true;
            withZlib = true;
            withVpx = !super.stdenv.hostPlatform.isMinGW
                   && !super.stdenv.hostPlatform.isDarwin;
            
            buildAvcodec = true;
            buildAvdevice = false;
            buildAvfilter = true;
            buildAvformat = true;
            buildAvresample = true;
            buildAvutil = true;
            buildPostproc = false;
            buildSwresample = true;
            buildSwscale = true;
          }).overrideAttrs (attrs: rec {
            #Disable checks: libavutil/tests/pixelutils.c:68:41: error: 'sad_c' undeclared (first use in this function)
            doCheck = false;
            
            configureFlags = attrs.configureFlags ++ [
              "--disable-everything"
              "--enable-demuxer=rawvideo,bink,avi,matroska"
              "--enable-decoder=rawvideo,bink,binkaudio_dct,vorbis,opus,png,vp9"
              "--enable-parser=png,vp9"
              "--enable-filter=aresample,aformat"
              "--enable-protocol=file"
              "--disable-ffplay"
              "--disable-ffprobe"
              "--disable-ffmpeg"
              "--disable-iconv"
            ];
          });
          
          SDL2 = super.SDL2.override {
            #Apparently on MinGW we need to add static generation
            withStatic = super.stdenv.hostPlatform.isStatic || super.stdenv.hostPlatform.isWindows;
            pipewireSupport = false;
            libdecorSupport = false;
          };
          
          SDL2_mixer = super.SDL2_mixer.override {
            flac = null;
            fluidsynth = null;
            libmodplug = null;
            mpg123 = null;
            opusfile = null;
            smpeg2 = null;
            timidity = "";
          };
          
          SDL2_image = super.SDL2_image.override {
            libpng = null;
            libjpeg = null;
            libtiff = null;
            giflib = null;
            libwebp = null;
            libXpm = null;
          };
        })
      ];
    in {
      # Main packages
      packages = forSystems buildSystems ({ system }:
        let
          localSystem = system;
        in (
          builtins.listToAttrs (
            mkPerimeterPackages {
              pkgs = import nixpkgs {
                inherit system;
                overlays = if (builtins.elem system stableSystems) then [] else overlays;
              };
            } ++ (builtins.concatLists (map ({ name, system }: 
              let
                target_pkgs = import nixpkgs {
                  inherit localSystem;
                  crossSystem = system;
                  overlays = if (builtins.elem system stableSystems) then [] else overlays;
                };
                validPlatform = !target_pkgs.stdenv.hostPlatform.isDarwin
                  || (
                    #Add darwin cross only if build platform is darwin
                    target_pkgs.stdenv.buildPlatform.isDarwin && target_pkgs.stdenv.hostPlatform.isDarwin
                    #AARCH64 crosscompiling x86_64 is broken on darwin for now
                    #https://github.com/NixOS/nixpkgs/issues/180771
                    && !(localSystem == "aarch64-darwin" && name == "x86_64-darwin")          
                  )
                ;
              in target_pkgs.lib.optionals validPlatform (mkPerimeterPackages {
                pkgs = target_pkgs;
                prefix = "platform_${name}";
              })) crossSystems)
            )
          )
        )
      );

      # Dev envs
      devShells = forSystems buildSystems ({ system }: {
        default = let
          package = self.packages.${system}.default;
          pkgs = import nixpkgs { inherit system; };
        in pkgs.mkShell {
          buildInputs = package.buildInputs ++ package.nativeBuildInputs;
        };
      });
    };
}
