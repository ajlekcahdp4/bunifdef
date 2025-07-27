# bunifdef - better (not yet) unifdef tool

Selectively remove C preprocessor conditionals

This project is a result of me stumbling upon bugs, *features* and outdated documentation for `unifdef` while having too much free time, I guess...

## How To

### Build with [nix](https://nixos.org/download/)

```sh
nix build
result/bin/bunifdef --help
```

### Build manualy

```sh
meson setup build --buildtype=release
meson compile -C build
```

### Use

See [manual](./docs/bunifdef.md)
