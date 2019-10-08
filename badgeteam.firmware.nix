with (import <nixpkgs> {});
pkgs.mkShell {
  name = "badgeteam.firmware";
  builder = "${bash}/bin/bash";
  args = [ ./builder.sh ];
  setup = ./setup.sh;
  system = builtins.currentSystem;

  buildInputs = [
    unzip gnutar
    coreutils bash gnumake git python gcc gnused
    binutils-unwrapped ncurses.dev which bison flex
    gperf
  ];
}
