with (import <nixpkgs> {});
derivation {
  name = "badgeteam.firmware";
  builder = "${bash}/bin/bash";
  args = [ ./builder.sh ];
  setup = ./setup.sh;
  system = builtins.currentSystem;

  baseInputs = [ unzip gnutar coreutils bash gnumake git python gcc gnused binutils-unwrapped ncurses.dev which ];
}
