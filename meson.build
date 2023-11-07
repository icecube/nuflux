project('nuflux', 'cpp',
        version: '2.0.4',
        default_options: ['cpp_std=c++17'])

cfitsio = dependency('cfitsio')
photospline = dependency('photospline', method : 'cmake')

inc = include_directories('src/include')
init_file = files('src/nuflux/__init__.py')
subdir('src/include/nuflux')
subdir('src/library')
subdir('src/pybindings')
subdir('src/nuflux/data')
subdir('tests')
subdir('docs')
