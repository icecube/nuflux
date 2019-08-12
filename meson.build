project('nuflux', 'cpp',
        version : '0.0.1',
        default_options : ['cpp_std=c++11'])

cfitsio = dependency('cfitsio')
photospline = dependency('photospline', method : 'cmake')

datadir = get_option('prefix') / get_option('datadir') / meson.project_name()
conf_data = configuration_data()
conf_data.set_quoted('DATA_DIR',datadir)
configure_file(output : 'config.h',
               configuration : conf_data)

install_subdir('nuflux/data/',
               install_dir:datadir,
               strip_directory:true)

inc = include_directories('src/include')
libnuflux = library(
  'nuflux',  
  'src/library/ANFlux.cpp',
  'src/library/IPLEFlux.cpp',
  'src/library/LegacyConventionalFlux.cpp',
  'src/library/SplineFlux.cpp',
  'src/library/FluxFunction.cpp',
  'src/library/LegacyPromptFlux.cpp',
  'src/library/logging.cpp',
  'src/library/detail.cpp',
  include_directories : inc,
  dependencies : [photospline,cfitsio],
  install : true)

test('test_basic',
     executable('test_basic',
                'tests/test_basic.cxx',
                include_directories : inc,
                link_with:libnuflux))

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  libraries : libnuflux,
  version : meson.project_version(),
  name : meson.project_name(),
  filebase :  meson.project_name(),
  url : 'https://github.com/IceCubeOpenSource/nuflux',
  description : 'A library for calculating atmospheric neutrino fluxes')

cppcomp = meson.get_compiler('cpp')
pymod = import('python')

foreach v : ['2','3']
  python = pymod.find_installation('python'+v,disabler:true,required:false)
  pydep = python.dependency()
  boost_python = cppcomp.find_library(
    'boost_python'+''.join(python.language_version().split('.')),
    disabler:true,required:false )
  if python.found() and boost_python.found()
    message( 'Building package for python version '+python.language_version() + ': ' + python.get_install_dir())

    python.install_sources('nuflux/__init__.py',subdir:'nuflux')
    
    python.extension_module(
      '_nuflux',
      ['src/pybindings/module.cxx'],
      include_directories :inc,
      dependencies : [photospline,pydep, boost_python],
      link_with : libnuflux,
      install_dir : python.get_install_dir() / 'nuflux',
      install:true)

    test('test_fluxes',find_program('tests/test_fluxes.py'))
    
  else
    warning( 'Can\'t find dependencies for python '+v+
             ', Skipping python package')
  endif
endforeach
  
