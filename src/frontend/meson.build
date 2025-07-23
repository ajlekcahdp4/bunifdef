flex = find_program('flex')
bison = find_program('bison')

parser_generated = custom_target(
    input: 'parser.y',
    output: ['bunifdef_parser.cpp', 'bunifdef_parser.hpp', 'location.hpp'],
    command: [bison, '-v', '--output=@OUTPUT0@', '@INPUT@', '-d'],
)

scanner_generated = custom_target(
    input: ['scanner.l', parser_generated],
    output: ['lexer_generated.cpp', 'lexer_generated.hpp'],
    command: [
        flex,
        '--full',
        '--outfile=@OUTPUT0@',
        '--header-file=@OUTPUT1@',
        '@INPUT0@',
    ],
)

bunifdef_frontend_headers = [
    parser_generated.to_list()[1],
    parser_generated.to_list()[2],
    scanner_generated.to_list()[1],
]

bunifdef_frontend = library(
    'bunifdef_frontend',
    [
        'dumper.cc',
        'expr_expander.cpp',
        parser_generated,
        scanner_generated,
    ],
    dependencies: [boost_dep, fmt_dep, bunifdef_headers_dep],
    install: true,
)

bunifdef_frontend_dep = declare_dependency(
    link_with: bunifdef_frontend,
    dependencies: bunifdef_headers_dep,
)
