{ pkgs ? import <nixpkgs> {} }:

let
  # Pin Qt to 6.8 (nixos-24.11) to avoid debug/release ABI mismatch with newer Qt
  pkgs-24_11 = import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/nixos-24.11.tar.gz") {
    system = pkgs.system;
  };
  qt6 = pkgs-24_11.qt6;

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
    nix-clangd
    qtcreator
  ];

  buildInputs = with pkgs; [
    qt6.qtbase
    qt6.qtdeclarative
    qt6.qtimageformats
    qt6.qtcharts
    qt6.qtwayland
    eigen_3_4_0
    boost
    libGL
    muparser
    ncurses
    cppcheck
  ];

  hardeningDisable = [ "fortify" ];

  shellHook = ''
    echo "CompileFlags:" > .clangd
    echo "  Add: [-isystem, ${qt6.qtbase.dev}/include]" >> .clangd

    export QML2_IMPORT_PATH="${qt6.qtdeclarative}/lib/qt-6/qml"
    export QT_PLUGIN_PATH="${qt6.qtbase}/${qt6.qtbase.qtPluginPrefix}:${qt6.qtimageformats}/${qt6.qtbase.qtPluginPrefix}:${qt6.qtwayland}/${qt6.qtbase.qtPluginPrefix}"
  '';
}
