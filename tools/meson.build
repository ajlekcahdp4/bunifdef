clang_dep = dependency('Clang', version: '>= 16', modules: ['libclang'])

bunifdef = executable('bunifdef', ['bunifdef.cpp', bunifdef_frontend_headers],
  dependencies: [boost_dep, fmt_dep, clang_dep, bunifdef_headers_dep, bunifdef_frontend_dep],
  install: true
  )
