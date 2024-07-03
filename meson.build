project('nuflux', 'cpp',
        version: '2.0.7',
        default_options: ['cpp_std=c++17'])

cfitsio = dependency('cfitsio')
photospline = dependency('photospline', method : 'cmake')

conf_data = configuration_data()
conf_data.set_quoted('NUFLUX_VERSION',meson.project_version())
if not get_option('pymodule')
  datadir = get_option('data_path')
  if datadir == ''
    datadir = get_option('prefix') / get_option('datadir') / meson.project_name()
  endif
  conf_data.set_quoted('DATA_DIR',datadir)
endif
configure_file(
  output: 'config.h',
  configuration: conf_data)
configuration_inc = include_directories('.')

inc = include_directories('src/include')
init_file = files('src/nuflux/__init__.py')
subdir('src/include/nuflux')
subdir('src/library')
subdir('src/pybindings')
subdir('src/nuflux/data')
subdir('tests')
subdir('docs')
