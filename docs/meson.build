bunifdef_man = custom_target(
  input: ['bunifdef.md'],
  output: ['bunifdef.1'],
  command: [pandoc, '-s', '--to=man', '--output=@OUTPUT@', '@INPUT@'],
  install: true,
  install_dir: get_option('mandir') / 'man1',
)
