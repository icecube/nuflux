doxygen = find_program('doxygen',required:false)
sphinx_build = find_program('sphinx-build',required:false)

if sphinx_build.found() and doxygen.found()

  version = run_command('git', 'describe', '--tags', check:true).stdout().strip()
  docs_config = {
    'CPP_SOURCE_DIR' : meson.project_source_root() + '/src',
    'DOCS_SOURCE_DIR' : meson.current_source_dir(),
    'DOXYGEN_XML_DIR' : meson.current_build_dir() + '/doxygen_xml',
    'PROJECT_VERSION' : version,
  }

  Doxyfile = configure_file(
    input : 'Doxyfile.in',
    output : 'Doxyfile',
    configuration : docs_config,
    )

  doxygen_xml = custom_target('doxygen',
      input : [Doxyfile],
      output : ['doxygen'],
      command : [doxygen, '@INPUT@']
      )

  sphinx_conf = configure_file(
    input : 'conf.py.in',
    output : 'conf.py',
    configuration : docs_config,
    )

  html = custom_target('sphinx',
    depends : doxygen_xml,
    input : [sphinx_conf, 'about.rst', 'development.rst', 'fluxes.rst',
             'installation.rst', 'pythonapi.rst', 'cppapi.rst', 'evaluation.rst',
             'index.rst', 'overview.rst', 'usage.rst' ],
    output : ['html'],
    command : ['sphinx-build', '-c', meson.current_build_dir(), meson.current_source_dir(),
              meson.current_build_dir() + '/html'],
    )
else
  warning('Sphinx or Doxygen not found: skipping documentation build')
endif