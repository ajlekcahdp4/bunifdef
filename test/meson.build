
configure_file(
  input: 'lit.site.cfg.py.in',
  output: 'lit.site.cfg.py',
  configuration: {
    'BINARY_DIR': meson.build_root(),
    'SOURCE_DIR': meson.source_root(),
  }
)

test('lit-bunifdef-tests',
  lit,
  args: ['--verbose', meson.current_build_dir()],
  suite: 'lit',
)

