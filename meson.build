project('nuflux', 'cpp',
        version : '0.0.1',
        default_options : ['cpp_std=c++11'])

cppcomp = meson.get_compiler('cpp')
photospline  = cppcomp.find_library('photospline')
boost_python = cppcomp.find_library('boost_python27-mt')
cfitsio = dependency('cfitsio', method : 'pkg-config')
python = import('python').find_installation('python2')

datadir = get_option('prefix') / get_option('datadir') / meson.project_name()
conf_data = configuration_data()
conf_data.set_quoted('DATA_DIR',datadir)
configure_file(output : 'config.h',
               configuration : conf_data)

install_subdir('nuflux/data/',
               install_dir:datadir,
               strip_directory:true)

inc = include_directories('src/include')
lib_sources = ['src/library/ANFlux.cpp',
               'src/library/IPLEFlux.cpp',
               'src/library/LegacyConventionalFlux.cpp',
               'src/library/SplineFlux.cpp',
               'src/library/FluxFunction.cpp',
               'src/library/LEFlux.cpp',
               'src/library/LegacyPromptFlux.cpp',
               'src/library/logging.cpp',
               'src/library/detail.cpp']

libnuflux = library(
  'nuflux',
  lib_sources,
  include_directories : inc,
  dependencies : [photospline,cfitsio],
  install : true)

python.extension_module(
  '_nuflux',
  ['src/pybindings/module.cxx'],
  include_directories :inc,
  dependencies : [python.dependency(),
                  boost_python],
  link_with : libnuflux,
  install_dir : python.get_install_dir(),
  install:true,
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : libnuflux,
                 version : meson.project_version(),
                 name : meson.project_name(),
                 filebase :  meson.project_name(),
                 url : 'https://github.com/IceCubeOpenSource/nuflux',
                 description : 'A collection of routines for caluclating atmospheric neutrino fluxes')
