sources = files(
    'src/main.cpp',
)

executable(
    'harbinger-editor',
    sources: sources,
    dependencies: [harbinger_engine_dep],
    include_directories: include_directories('src/'),

    install: true,
    install_dir: '.',
    install_tag: 'deploy',
)