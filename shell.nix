{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  name = "linux-resource-monitor-dev";

  buildInputs = with pkgs; [
    # Build tools
    cmake
    cmake-language-server
    pkg-config

    # SDL2 and OpenGL dependencies
    SDL2
    SDL2.dev
    libGL
    mesa-demos # For glxinfo to verify OpenGL

    # X11 dependencies for XWayland fallback
    xorg.libX11
    xorg.libXext
    xorg.libXcursor
    xorg.libXrandr
    xorg.libXinerama

    # C++ development and debugging
    gcc13
    clang-tools_16
    cppcheck
    gdb

    # Formatting
    cmake-format
  ];

  shellHook = ''
    export PKG_CONFIG_PATH="${pkgs.SDL2.dev}/lib/pkgconfig:${pkgs.libGL}/lib/pkgconfig:${pkgs.xorg.libX11}/lib/pkgconfig:${pkgs.xorg.libXext}/lib/pkgconfig:${pkgs.xorg.libXcursor}/lib/pkgconfig:${pkgs.xorg.libXrandr}/lib/pkgconfig:${pkgs.xorg.libXinerama}/lib/pkgconfig:$PKG_CONFIG_PATH"
    export SDL_VIDEODRIVER=wayland
    export GTK_MODULES=""
    echo "SDL2 version: $(pkg-config --modversion sdl2)"
    echo "OpenGL version: $(glxinfo | grep 'OpenGL version' || echo 'glxinfo not installed, check OpenGL support')"
    echo "X11 libraries: $(pkg-config --libs x11 xext xcursor xrandr xinerama)"
    echo "Development environment for linux-resource-monitor ready!"
    echo "Running in Hyprland (Wayland). Use XDG_BACKEND=x11 for XWayland fallback if needed."
  '';
}