{
  stdenv,
  lib,
  fmt,
  boost,
  pkg-config,
  meson,
  cmake,
  ninja,
  flex,
  lit,
  filecheck,
  clang-tools,
  bison,
  pandoc,
  ...
}:
let
  fs = lib.fileset;
in
stdenv.mkDerivation {
  pname = "bunifdef";
  version = "0.0.0";
  src = fs.toSource {
    root = ./.;
    fileset = fs.unions [
      ./meson.build
      ./src
      ./tools
      ./include
      ./test
      ./docs
    ];
  };
  nativeBuildInputs = [
    meson
    cmake
    ninja
    pkg-config
    flex
    bison
    pandoc
  ];
  buildInputs = [
    fmt
    boost
  ];
  nativeCheckInputs = [
    lit
    filecheck
    clang-tools
  ];
  doCheck = true;
}
