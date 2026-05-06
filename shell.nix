{ pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/refs/heads/nixos-24.11.tar.gz") {} }:

let
  nix-clangd = pkgs.writeShellScriptBin "clangd" ''
    exec ${pkgs.clang-tools}/bin/clangd --query-driver="/nix/store/*-gcc-wrapper-*/bin/g++" "$@"
  '';
in
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    ninja
    pkg-config
    qt6.wrapQtAppsHook
    qtcreator
    nix-clangd
  ];

        buildInputs = with pkgs; [
            qt6.qtbase
            qt6.qtdeclarative
            qt6.qtimageformats
            qt6.qtcharts
            libGL
            muparser
            ncurses
        ];

  hardeningDisable = [ "fortify" ];

  shellHook = ''
    echo "CompileFlags:" > .clangd
    echo "  Add: [-isystem, ${pkgs.qt6.qtbase.dev}/include]" >> .clangd

    export QML2_IMPORT_PATH="${pkgs.qt6.qtdeclarative}/lib/qt-6/qml"
    export QT_PLUGIN_PATH="${pkgs.qt6.qtbase}/${pkgs.qt6.qtbase.qtPluginPrefix}:${pkgs.qt6.qtimageformats}/${pkgs.qt6.qtbase.qtPluginPrefix}"
  '';
}
