lib_src = files( 
  'ANFlux.cpp',
  'IPLEFlux.cpp',
  'LegacyConventionalFlux.cpp',
  'SplineFlux.cpp',
  'SplineFlux2.cpp',
  'FluxFunction.cpp',
  'LegacyPromptFlux.cpp'
  )

if not get_option('pymodule')
  datadir = get_option('data_path')
  if datadir == ''
    datadir = get_option('prefix') / get_option('datadir') / meson.project_name()
  endif
  conf_data = configuration_data()
  conf_data.set_quoted('DATA_DIR',datadir)
  configure_file(output: 'config.h',
                configuration: conf_data)

  libnuflux = library(
    'nuflux',
    lib_src + 'detail.cpp',
    include_directories: inc,
    dependencies : [photospline, cfitsio],
    install: true)

  pkg_mod = import('pkgconfig')
  pkg_mod.generate(
    libraries: libnuflux,
    version: meson.project_version(),
    name: meson.project_name(),
    filebase:  meson.project_name(),
    url: 'https://github.com/icecube/nuflux',
    description: 'A library for calculating atmospheric neutrino fluxes')
endif