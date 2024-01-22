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

  libnuflux = library(
    'nuflux',
    lib_src + 'detail.cpp',
    include_directories: [inc, configuration_inc],
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
