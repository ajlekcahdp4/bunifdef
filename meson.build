project('bunifdef', 'cpp',
    default_options: ['cpp_std=c++23', 'warning_level=3'],
  )

boost_dep = dependency('boost', modules: ['program_options', 'regex', 'filesystem'], required: true)
fmt_dep = dependency('fmt', required: true)
bunifdef_headers_dep = declare_dependency(
    include_directories: include_directories('include'),
)
subdir('src')
subdir('tools')
lit = find_program('lit', required: false)
if lit.found()
  message('Found lit. Building tests.')
  subdir('test')
else
  warning('No lit tool found. Skipping tests')
endif
