{
  description = "bunifdef - better unifdef";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      flake-parts,
      treefmt-nix,
      ...
    }@inputs:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ treefmt-nix.flakeModule ];

      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      perSystem =
        { pkgs, ... }:
        rec {
          imports = [ ./nix/treefmt.nix ];
          packages =
            let
              bunifdef = pkgs.callPackage ./. { };
            in
            {
              inherit bunifdef;
              default = bunifdef;
            };
          checks = {
            inherit (packages) bunifdef;
          };
          devShells.default = (pkgs.mkShell.override { }) {
            nativeBuildInputs =
              packages.bunifdef.nativeBuildInputs
              ++ (with pkgs; [
                llvmPackages.bintools
                clang-tools
                lit
                filecheck
                act
                gdb
                lldb
                valgrind
                just
              ]);
            buildInputs = packages.bunifdef.buildInputs;
          };
        };
    };
}
