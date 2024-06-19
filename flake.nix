{
  description = "A Nix flake for basic raytracing in C++";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in
    {
      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell.override {
          # Override stdenv in order to change compiler:
          # stdenv = pkgs.clangStdenv;
        }
        {
          packages = with pkgs; [
            just
            bear
            rtags
            clang-tools
            cmake
            codespell
            conan
            cppcheck
            doxygen
            gtest
            lcov
            # vcpkg
            # vcpkg-tool

            # Graphics/ImGui/raytracing tutorial
            glm
            wayland-scanner
            libxkbcommon
            xorg.libXrandr
            pkg-config
            SDL2
            glew
            libGLU
            # glfw
            freetype
            shaderc             # GLSL to SPIRV compiler - glslc
            renderdoc           # Graphics debugger
            tracy               # Graphics profiler
            # (pkgs.writeShellScript "rdm_init" ''pgrep rdm >> /dev/null || rdm & return 0'')
          ] ++ (if system == "aarch64-darwin" then [ ] else [ gdb ]);
          shellHook = ''
            export CMAKE_EXPORT_COMPILE_COMMANDS=1
          '';
        };
      });
    };
}
