bunifdef_backend = library('bunifdef_backend',
    ['evaluator.cpp', bunifdef_frontend_headers],
    dependencies: [
        boost_dep,
        fmt_dep,
        bunifdef_headers_dep,
        bunifdef_frontend_dep
    ],
    install: true,
)

bunifdef_backend_dep = declare_dependency(
    link_with: bunifdef_backend,
    dependencies: bunifdef_headers_dep,
)
