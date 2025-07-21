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
  bison,
  llvmPackages,
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
      ./CMakeLists.txt
      ./src
      ./tools
      ./include
      ./test
    ];
  };
  nativeBuildInputs = [
    cmake
    meson
    ninja
    pkg-config
    flex
    bison
  ];
  buildInputs = [
    fmt
    boost
    llvmPackages.clang-unwrapped.dev
    llvmPackages.llvm
  ];
}
