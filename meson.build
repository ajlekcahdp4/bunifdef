project('bunifdef', 'cpp',
    default_options: ['cpp_std=c++23', 'warning_level=3'],
  )

boost_dep = dependency('boost', modules: ['program_options', 'regex'], required: true)
fmt_dep = dependency('fmt', required: true)
bunifdef_headers_dep = declare_dependency(
    include_directories: include_directories('include'),
)
subdir('src')
subdir('tools')
