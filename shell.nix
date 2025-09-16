{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  buildInputs = with pkgs; [
    cmake
    gcc
    pkg-config
    glfw
    libGL
    libGLU
    # GUI/graphics dependencies
    wayland
    # glew
    # libGL
    # libxkbcommon
    # xorg.libX11
    # xorg.libXrandr
    # xorg.libXinerama
    # xorg.libXcursor
    # xorg.libXi

    # Optional depending on your setup
    # SDL2
    # freetype
    # fontconfig
    premake5
  ];

  shellHook = ''
    nvim
  '';
}
