cppcomp = meson.get_compiler('cpp')
pymod = import('python')
python = pymod.find_installation(get_option('python'), required: false, pure: false)
test_python = false

if not python.found()
  warning('Can\'t find python: "' + get_option('python') + '", skipping python build')
else
  pydep = python.dependency()
  module_inc = [inc]

  # Find numpy
  numpy = run_command(
    python,'-c',
    'import importlib.util,os;print(os.path.join(importlib.util.find_spec("numpy").submodule_search_locations[0],"core","include"))',
    check: true,
    )
  if numpy.returncode()==0
    module_inc += include_directories(numpy.stdout().strip())
    message ('Numpy found: '+ numpy.stdout().strip())
  else
    warning ('Numpy not found: building without arrays')
  endif

  python_name = 'python' + ''.join(python.language_version().split('.'))
  boost = dependency('boost', modules: [python_name])

  if not boost.found()
    warning( 'Can\'t find boost python, Skipping python build')
  else
    message('Building package for python version ' +
            python.language_version() + ': ' + python.get_install_dir())
    python.install_sources(init_file, subdir: 'nuflux')

    pysrc = ['module.cxx']
    pymodule_link = []
    if get_option('pymodule')
      pysrc += lib_src
      pysrc += 'detail.cxx'
      module_inc += configuration_inc
    else
      pymodule_link += libnuflux
    endif

    pymodule = python.extension_module(
      '_nuflux',
      pysrc,
      include_directories: module_inc,
      dependencies: [photospline, pydep, boost],
      link_with: pymodule_link,
      cpp_args: '-Wno-deprecated',
      install: true)

    test_python = true
  endif
endif
