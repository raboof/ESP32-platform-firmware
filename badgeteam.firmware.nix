with (import <nixpkgs> {});
mkShell {
  name = "badgeteam.firmware";

  buildInputs = [
    unzip gnutar
    coreutils bash gnumake git python gcc gnused
    binutils-unwrapped ncurses.dev which bison flex
    gperf pkg-config stdenv.cc.cc.lib
  ];

  shellHook = let
    libPath = lib.makeLibraryPath [
      stdenv.cc.cc.lib zlib
    ];
  in ''
    # yolo
    for i in ./xtensa-esp32-elf/bin/* ./xtensa-esp32-elf/libexec/gcc/xtensa-esp32-elf/5.2.0/*; do \
    patchelf \
      --set-interpreter "$(cat $NIX_CC/nix-support/dynamic-linker)" \
      --set-rpath "${libPath}" \
      $i; \
    done
    '';
}
