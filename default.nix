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
    ];
  };
  nativeBuildInputs = [
    meson
    cmake
    ninja
    pkg-config
    flex
    bison
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
